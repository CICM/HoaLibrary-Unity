// Please note that this will only work on Unity 5.2 or higher.

#include "AudioPluginUtil.h"
#include <array>

namespace HoaLibraryVR_Spatializer
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
            RegisterParameter(definition, "AudioSrc Attn", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, P_AUDIOSRC_ATTN,
                              "AudioSource distance attenuation");
            
            RegisterParameter(definition, "Fixed Volume", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_FIXEDVOLUME,
                              "Fixed volume amount");
            
            RegisterParameter(definition, "Custom Falloff", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_CUSTOMFALLOFF,
                              "Custom volume falloff amount (logarithmic)");
            
            // required flag to be recognized as a spatialiser plugin
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
        
        //! @brief Check host compatibility.
        //! @details because hostapiversion is only supported from SDK version 1.03 (i.e. Unity 5.2) and onwards.
        //! Since we are only checking for version 0x010300 here,
        //! we can't use newer fields in the UnityAudioSpatializerData struct, such as minDistance and maxDistance.
        bool isHostCompatible(effect_state_t* state) const
        {
            return (state->structsize >= sizeof(effect_state_t)
                    && state->hostapiversion >= 0x010300);
        }
        
        float getAttenuation(effect_state_t* state, float distanceIn, float attenuationIn)
        {
            return (p[P_AUDIOSRC_ATTN] * attenuationIn + p[P_FIXEDVOLUME] +
                    p[P_CUSTOMFALLOFF] * (1.0f / FastMax(1.0f, distanceIn)));
        }
        
        void process(effect_state_t* state,
                     float* inbuffer, float* outbuffer, unsigned int length,
                     int numins, int numouts)
        {
            // Check that I/O formats are right and that the host API supports this feature
            if ((numins != 2 || numouts != 2)
                || (!isHostCompatible(state) || !state->spatializerdata))
            {
                memcpy(outbuffer, inbuffer, length * numouts * sizeof(float));
                return;
            }
            
            /*
            static const float kRad2Deg = 180.0f / kPI;
            
            float* m = state->spatializerdata->listenermatrix;
            float* s = state->spatializerdata->sourcematrix;
            
            // Currently we ignore source orientation and only use the position
            float px = s[12];
            float py = s[13];
            float pz = s[14];
            
            float dir_x = m[0] * px + m[4] * py + m[8] * pz + m[12];
            float dir_y = m[1] * px + m[5] * py + m[9] * pz + m[13];
            float dir_z = m[2] * px + m[6] * py + m[10] * pz + m[14];
            
            float azimuth = (fabsf(dir_z) < 0.001f) ? 0.0f : atan2f(dir_x, dir_z);
            if (azimuth < 0.0f)
            {
                azimuth += 2.0f * kPI;
            }
            
            azimuth = FastClip(azimuth * kRad2Deg, 0.0f, 360.0f);
            
            const float elevation = atan2f(dir_y, sqrtf(dir_x * dir_x + dir_z * dir_z) + 0.001f) * kRad2Deg;
            float spatialblend = state->spatializerdata->spatialblend;
            float reverbmix = state->spatializerdata->reverbzonemix;
            float stereopan = state->spatializerdata->stereopan;
            float spread = cosf(state->spatializerdata->spread * kPI / 360.0f);
            float spreadmatrix[2] = { 2.0f - spread, spread };
            
            for (int sampleOffset = 0; sampleOffset < length; ++sampleOffset)
            {
                for (int c = 0; c < 2; c++)
                {
                    // stereopan is in the [-1; 1] range, this acts the way fmod does it for stereo
                    //float pan = FastMax(0.0f, 1.0f - ((c == 0) ? stereopan : -stereopan));
                }
            }
            */
        }
        
    private:
        
        static UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
        DistanceAttenuationCallback(effect_state_t* state,
                                    float distanceIn, float attenuationIn, float* attenuationOut)
        {
            auto* processor = state->GetEffectData<HoaAudioProcessor>();
            *attenuationOut = processor->getAttenuation(state, distanceIn, attenuationIn);
            return UNITY_AUDIODSP_OK;
        }
        
    private:
        
        std::array<float, P_NUM> p;
    };
    
    #include "UnityCallbacks.hpp"
}

