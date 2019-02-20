// Please note that this will only work on Unity 5.2 or higher.

#include "AudioPluginUtil.h"
#include <array>

namespace HoaLibraryVR_Renderer
{
    using effect_definition_t = UnityAudioEffectDefinition;
    using param_definition_t = UnityAudioParameterDefinition;
    using effect_state_t = UnityAudioEffectState;
    
    //==============================================================================
    // Processor
    //==============================================================================
    
    class HoaAudioProcessor
    {
    public:
        
        // parameters
        enum
        {
            P_GAIN,
            P_NUM
        };
        
        HoaAudioProcessor() = default;
        ~HoaAudioProcessor() = default;
        
        static int registerEffect(effect_definition_t& definition)
        {
            int numparams = P_NUM;
            definition.paramdefs = new param_definition_t[numparams];
            
            RegisterParameter(definition, "Fixed Volume", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_GAIN,
                              "Fixed volume amount");
            
            return numparams;
        }
        
        //! @brief Called when the plugin is created
        void create(effect_state_t* state)
        {
            state->effectdata = this;
            
            InitParametersFromDefinitions(registerEffect, p.data());
        }
        
        //! @brief Release ressources.
        void release() {}
        
        bool setFloatParameter(effect_state_t* state, int index, float value)
        {
            if (index >= P_NUM)
                return false;
            
            p[index] = value;
            return true;
        }
        
        bool getFloatParameter(effect_state_t* state, int index, float* value, char *valuestr)
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
                     float* inbuffer, float* outbuffer, unsigned int length,
                     int numins, int numouts)
        {
            // Check that I/O formats are right
            if (numins != 2 || numouts != 2)
            {
                memcpy(outbuffer, inbuffer, length * numouts * sizeof(float));
                return;
            }
            
            // do something here...
        }
        
    private:
        
        std::array<float, P_NUM> p;
    };
    
    #include "UnityCallbacks.hpp"
    
}
