// Please note that this will only work on Unity 5.2 or higher.

#include "HoaLibraryVR.h"

#include "AudioPluginInterface.h"
#include "AudioPluginUtil.h"

namespace HoaLibraryVR_Renderer
{
    using HoaLibraryVR::float_t;
    using effect_definition_t = UnityAudioEffectDefinition;
    using param_definition_t = UnityAudioParameterDefinition;
    using effect_state_t = UnityAudioEffectState;
    
    //==============================================================================
    // Processor
    //==============================================================================
    
    //! @brief Converts soundfield into binaural audio
    class HoaAudioProcessor
    {
    public:
        
        // parameters
        enum
        {
            P_MASTER_GAIN,
            P_NUM
        };
        
        HoaAudioProcessor() = default;
        ~HoaAudioProcessor() = default;
        
        static int registerEffect(effect_definition_t& definition)
        {
            int numparams = P_NUM;
            definition.paramdefs = new param_definition_t[numparams];
            
            RegisterParameter(definition, "Master Gain", "dB",
                              -120.f, 50.f, 0.0f, 1.0f, 1.0f,
                              P_MASTER_GAIN, "Master Gain");
            
            return numparams;
        }
        
        //! @brief Called when the plugin is created
        void create(effect_state_t* state)
        {
            assert(state);
            state->effectdata = this;
            InitParametersFromDefinitions(registerEffect, p.data());
            const size_t vectorsize = static_cast<size_t>(state->dspbuffersize);
            HoaLibraryVR::Initialize(vectorsize);
        }
        
        //! @brief Release ressources.
        void release()
        {
            HoaLibraryVR::Shutdown();
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
        
        void process(effect_state_t* state,
                     float_t* inputs, float_t* outputs, unsigned int length,
                     int numins, int numouts)
        {
            const bool is_muted = state->flags & (UnityAudioEffectStateFlags_IsMuted);
            const bool is_paused = state->flags & (UnityAudioEffectStateFlags_IsPaused);
            const bool is_playing = state->flags & (UnityAudioEffectStateFlags_IsPlaying);
            
            // Check that I/O formats are right
            if ((numins != 2 || numouts != 2) || (is_muted || is_paused || !is_playing))
            {
                // fill with zeros
                std::fill(outputs, outputs + length * numouts, 0.f);
                
                // or copy inputs to outputs ?
                // const size_t buffer_size_per_channel_bytes = length * sizeof(float_t);
                // const size_t buffer_size_bytes = buffer_size_per_channel_bytes * numouts;
                // memcpy(outputs, inputs, buffer_size_bytes);
                
                return;
            }
            
            const auto gain = std::powf(10.f, p[P_MASTER_GAIN] * 0.05f);
            
            HoaLibraryVR::SetMasterGain(gain);
            HoaLibraryVR::ProcessListener(length, outputs);
        }
        
    private:
        
        std::array<float_t, P_NUM> p;
    };
    
    #include "UnityCallbacks.hpp"
    
}
