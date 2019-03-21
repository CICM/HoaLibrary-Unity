//==============================================================================
// HoaLibrary for Unity - version 1.0.0
// https://github.com/CICM/HoaLibrary-Unity
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
// Thirdparty :
// - HoaLibrary-Light: https://github.com/CICM/HoaLibrary-Light
// - Unity nativeaudioplugins SDK: https://bitbucket.org/Unity-Technologies/nativeaudioplugins.
//==============================================================================

// Please note that this plugin will only work on Unity 5.2 or higher.

#include "HoaLibraryVR.h"

#include "AudioPluginInterface.h"
#include "AudioPluginUtil.h"

namespace HoaLibraryVR_Spatializer
{
    using namespace HoaLibraryVR;
    using source_id_t = HoaLibraryApi::source_id_t;
    using effect_definition_t = UnityAudioEffectDefinition;
    using param_definition_t = UnityAudioParameterDefinition;
    using effect_state_t = UnityAudioEffectState;
    
    //==============================================================================
    // Processor
    //==============================================================================
    
    //! @brief Sends audio and spatialization data to the HoaLibrary System
    class HoaAudioProcessor
    {
    public:
        
        // parameters
        enum Param
        {
            DistanceAttenuation,
            Gain,
            CustomFalloff,
            Optim,
            Size
        };
        
        HoaAudioProcessor() = default;
        ~HoaAudioProcessor() = default;
        
        static int registerEffect(effect_definition_t& definition)
        {
            const int numparams = Param::Size;
            definition.paramdefs = new param_definition_t[numparams];
            
            RegisterParameter(definition, "Attenuation", "",
                              0.0f, 1.0f, 1.0f, 1.0f, 1.0f, Param::DistanceAttenuation,
                              "AudioSource distance attenuation");
            
            RegisterParameter(definition, "Gain", "dB",
                              0.f, 20.f, 0.0f, 1.0f, 1.0f, Param::Gain,
                              "Additional gain");
            
            RegisterParameter(definition, "Custom Falloff", "",
                              0.0f, 1.0f, 0.0f, 1.0f, 1.0f, Param::CustomFalloff,
                              "Custom volume falloff amount (logarithmic)");
            
            RegisterParameter(definition, "Optim", "",
                              0.0f, 2.0f, 0.0f, 1.0f, 1.0f, Param::Optim,
                              "Ambisonic optimization (Basic | MaxRe | inPhase)");
            
            // required flag to be recognized as a spatialiser plugin by unity
            definition.flags |= UnityAudioEffectDefinitionFlags_IsSpatializer;
            
            return numparams;
        }
        
        //! @brief Called when the plugin is created
        void create(effect_state_t* state)
        {
            state->effectdata = this;
            
            if (isHostCompatible(state))
                state->spatializerdata->distanceattenuationcallback = DistanceAttenuationCallback;
            
            InitParametersFromDefinitions(registerEffect, p.data());
            
            m_source_id = HoaLibraryVR::CreateSource();
        }
        
        //! @brief Release ressources.
        void release()
        {
            HoaLibraryVR::DestroySource(m_source_id);
        }
        
        bool setFloatParameter(effect_state_t* state, int index, float_t value)
        {
            if (index >= Param::Size)
                return false;
            
            p[index] = value;
            return true;
        }
        
        bool getFloatParameter(effect_state_t* state, int index, float_t* value, char *valuestr)
        {
            if (index >= Param::Size)
                return false;
            
            if (value)
                *value = p[index];
            
            if (valuestr)
                valuestr[0] = 0;
            
            return true;
        }
        
        //! @brief Check host compatibility.
        //! @details because hostapiversion is only supported from SDK version 1.03
        //! (i.e. Unity 5.2) and onwards.
        //! Since we are only checking for version 0x010300 here, we can't use
        //! newer fields in the UnityAudioSpatializerData struct,
        //! such as minDistance and maxDistance.
        bool isHostCompatible(effect_state_t* state) const
        {
            return (state->structsize >= sizeof(effect_state_t)
                    && state->hostapiversion >= 0x010300);
        }
        
        float getAttenuation(effect_state_t* state, float_t distanceIn, float_t attenuationIn)
        {
            return (p[Param::DistanceAttenuation] * attenuationIn +
                    p[Param::CustomFalloff] * (1.0f / FastMax(1.0f, distanceIn)));
        }
        
        void process(effect_state_t* state,
                     float_t* inputs, float_t* outputs, unsigned int length,
                     int numins, int numouts)
        {
            // Check that I/O formats are right and that the host API supports this feature
            if ((numins != 2 || numouts != 2)
                || (!isHostCompatible(state) || !state->spatializerdata)
                || m_source_id == HoaLibraryApi::invalid_source_id)
            {
                std::fill(outputs, outputs + length * numouts, 0.f);
                return;
            }
            
            const auto& spatinfos = *state->spatializerdata;
            const auto pan = spatinfos.stereopan; // [-1 to 1]
            const int optimization = static_cast<int>(p[Param::Optim]);
            
            auto const* sm = spatinfos.sourcematrix;
            auto const* lm = spatinfos.listenermatrix;
            
            // Currently we ignore source orientation and only use the position
            const float_t pos_x = sm[12];
            const float_t pos_y = sm[13];
            const float_t pos_z = sm[14];
            
            const float_t dir_x = lm[0] * pos_x + lm[4] * pos_y + lm[ 8] * pos_z + lm[12];
            const float_t dir_y = lm[1] * pos_x + lm[5] * pos_y + lm[ 9] * pos_z + lm[13];
            const float_t dir_z = lm[2] * pos_x + lm[6] * pos_y + lm[10] * pos_z + lm[14];
            
            const auto gain = std::powf(10.f, p[Param::Gain] * 0.05f);
            HoaLibraryVR::SetSourceGain(m_source_id, gain);
            HoaLibraryVR::SetSourcePan(m_source_id, pan);
            HoaLibraryVR::SetSourcePosition(m_source_id, dir_x, dir_y, dir_z);
            HoaLibraryVR::SetSourceOptim(m_source_id, optimization);
            HoaLibraryVR::ProcessSource(m_source_id, length, inputs);
            
            // Copy inputs to outputs to allow post processing/analysis features in Unity.
            std::memcpy(outputs, inputs, length * sizeof(float_t) * numouts);
        }
        
    private:
        
        static UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
        DistanceAttenuationCallback(effect_state_t* state,
                                    float_t distanceIn, float_t attenuationIn, float_t* attenuationOut)
        {
            auto* processor = state->GetEffectData<HoaAudioProcessor>();
            *attenuationOut = processor->getAttenuation(state, distanceIn, attenuationIn);
            return UNITY_AUDIODSP_OK;
        }
        
    private:
        
        std::array<float_t, Param::Size> p;
        
        source_id_t m_source_id = HoaLibraryApi::invalid_source_id;
    };
    
    #include "UnityCallbacks.hpp"
}
