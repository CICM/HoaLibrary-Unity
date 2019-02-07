// Please note that this will only work on Unity 5.2 or higher.

#include "AudioPluginUtil.h"

namespace HoaLibraryVR
{
    enum
    {
        P_AUDIOSRCATTN,
        P_FIXEDVOLUME,
        P_CUSTOMFALLOFF,
        P_NUM
    };
    
    struct EffectData
    {
        float p[P_NUM];
    };
    
    //! @brief Check host compatibility.
    //! @details because hostapiversion is only supported from SDK version 1.03 (i.e. Unity 5.2) and onwards.
    //! Since we are only checking for version 0x010300 here,
    //! we can't use newer fields in the UnityAudioSpatializerData struct, such as minDistance and maxDistance.
    inline bool IsHostCompatible(UnityAudioEffectState* state)
    {
        return (state->structsize >= sizeof(UnityAudioEffectState)
                && state->hostapiversion >= 0x010300);
    }
    
    int InternalRegisterEffectDefinition(UnityAudioEffectDefinition& definition)
    {
        int numparams = P_NUM;
        definition.paramdefs = new UnityAudioParameterDefinition[numparams];
        RegisterParameter(definition, "AudioSrc Attn", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, P_AUDIOSRCATTN,
                          "AudioSource distance attenuation");
        
        RegisterParameter(definition, "Fixed Volume", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_FIXEDVOLUME,
                          "Fixed volume amount");
        
        RegisterParameter(definition, "Custom Falloff", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, P_CUSTOMFALLOFF,
                          "Custom volume falloff amount (logarithmic)");
        
        // required flag to be recognized as a spatialiser plugin
        definition.flags |= UnityAudioEffectDefinitionFlags_IsSpatializer;
        
        return numparams;
    }
    
    static UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK DistanceAttenuationCallback(UnityAudioEffectState* state, float distanceIn, float attenuationIn, float* attenuationOut)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        *attenuationOut =
        data->p[P_AUDIOSRCATTN] * attenuationIn +
        data->p[P_FIXEDVOLUME] +
        data->p[P_CUSTOMFALLOFF] * (1.0f / FastMax(1.0f, distanceIn));
        return UNITY_AUDIODSP_OK;
    }
    
    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state)
    {
        EffectData* effectdata = new EffectData;
        memset(effectdata, 0, sizeof(EffectData));
        state->effectdata = effectdata;
        if (IsHostCompatible(state))
            state->spatializerdata->distanceattenuationcallback = DistanceAttenuationCallback;
        InitParametersFromDefinitions(InternalRegisterEffectDefinition, effectdata->p);
        return UNITY_AUDIODSP_OK;
    }
    
    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        delete data;
        return UNITY_AUDIODSP_OK;
    }
    
    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= P_NUM)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        data->p[index] = value;
        return UNITY_AUDIODSP_OK;
    }
    
    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char *valuestr)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= P_NUM)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        if (value != NULL)
            *value = data->p[index];
        if (valuestr != NULL)
            valuestr[0] = 0;
        return UNITY_AUDIODSP_OK;
    }
    
    int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState* state, const char* name, float* buffer, int numsamples)
    {
        return UNITY_AUDIODSP_OK;
    }
    
    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState* state, float* inbuffer, float* outbuffer, unsigned int length, int numins, int numouts)
    {
        // Check that I/O formats are right and that the host API supports this feature
        if ((numins != 2 || numouts != 2)
            || (!IsHostCompatible(state) || state->spatializerdata == NULL))
        {
            memcpy(outbuffer, inbuffer, length * numouts * sizeof(float));
            return UNITY_AUDIODSP_OK;
        }
        
        EffectData* data = state->GetEffectData<EffectData>();
        
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
                float pan = 1.0f - ((c == 0) ? FastMax(0.0f, stereopan) : FastMax(0.0f, -stereopan));
            }
        }
        
        return UNITY_AUDIODSP_OK;
    }
}

