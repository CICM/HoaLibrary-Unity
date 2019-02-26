// Please note that this will only work on Unity 5.2 or higher.

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
        enum
        {
            P_AUDIOSRC_ATTN,
            P_FIXEDVOLUME,
            P_CUSTOMFALLOFF,
            P_NUM
        };
        
        HoaAudioProcessor() = default;
        ~HoaAudioProcessor() = default;
        
        static int registerEffect(effect_definition_t& definition)
        {
            int numparams = P_NUM;
            definition.paramdefs = new param_definition_t[numparams];
            
            RegisterParameter(definition, "AudioSrc Attn", "",
                              0.0f, 1.0f, 1.0f, 1.0f, 1.0f, P_AUDIOSRC_ATTN,
                              "AudioSource distance attenuation");
            
            RegisterParameter(definition, "Fixed Volume", "",
                              0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_FIXEDVOLUME,
                              "Fixed volume amount");
            
            RegisterParameter(definition, "Custom Falloff", "",
                              0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_CUSTOMFALLOFF,
                              "Custom volume falloff amount (logarithmic)");
            
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
            if (index >= P_NUM)
                return false;
            
            p[index] = value;
            return true;
        }
        
        bool getFloatParameter(effect_state_t* state, int index, float_t* value, char *valuestr)
        {
            if (index >= P_NUM)
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
            return (p[P_AUDIOSRC_ATTN] * attenuationIn + p[P_FIXEDVOLUME] +
                    p[P_CUSTOMFALLOFF] * (1.0f / FastMax(1.0f, distanceIn)));
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
            
            auto const* sm = spatinfos.sourcematrix;
            auto const* lm = spatinfos.listenermatrix;
            
            // Currently we ignore source orientation and only use the position
            const float_t pos_x = sm[12];
            const float_t pos_y = sm[13];
            const float_t pos_z = sm[14];
            
            const float_t dir_x = lm[0] * pos_x + lm[4] * pos_y + lm[ 8] * pos_z + lm[12];
            const float_t dir_y = lm[1] * pos_x + lm[5] * pos_y + lm[ 9] * pos_z + lm[13];
            const float_t dir_z = lm[2] * pos_x + lm[6] * pos_y + lm[10] * pos_z + lm[14];
            
            /*
             auto const* lm = spatinfos.listenermatrix;
             const float_t listener_pos_x = -(lm[0] * lm[12] + lm[ 1] * lm[13] + lm[ 2] * lm[14]);
             const float_t listener_pos_y = -(lm[4] * lm[12] + lm[ 5] * lm[13] + lm[ 6] * lm[14]);
             const float_t listener_pos_z = -(lm[8] * lm[12] + lm[ 9] * lm[13] + lm[10] * lm[14]);
            */
            
            // HoaLibraryVR::SetListenerTransform(listener_pos_x, listener_pos_y, listener_pos_z, 0.f, 0.f, 0.f, 0.f);
            
            HoaLibraryVR::SetSourcePan(m_source_id, pan);
            HoaLibraryVR::SetSourcePosition(m_source_id, dir_x, dir_y, dir_z);
            HoaLibraryVR::ProcessSource(m_source_id, length, inputs);
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
        
        std::array<float_t, P_NUM> p;
        
        source_id_t m_source_id = HoaLibraryApi::invalid_source_id;
    };
    
    #include "UnityCallbacks.hpp"
}
