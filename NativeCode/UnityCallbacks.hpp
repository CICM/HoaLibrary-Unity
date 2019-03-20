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

// #pragma once // disabled to allow multiple includes in cpp files

using processor_t = HoaAudioProcessor;

static auto* getProcessor(effect_state_t& state)
{
    return state.GetEffectData<processor_t>();
}

//==============================================================================
// Callbacks
//==============================================================================

int InternalRegisterEffectDefinition(effect_definition_t& definition)
{
    return processor_t::registerEffect(definition);
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
CreateCallback(effect_state_t* state)
{
    auto* processor = new processor_t();
    processor->create(state);
    return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
ReleaseCallback(effect_state_t* state)
{
    auto* processor = getProcessor(*state);
    processor->release();
    delete processor;
    return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
SetFloatParameterCallback(effect_state_t* state, int index, float value)
{
    auto* processor = getProcessor(*state);
    return (processor->setFloatParameter(state, index, value)
            ? UNITY_AUDIODSP_OK : UNITY_AUDIODSP_ERR_UNSUPPORTED);
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
GetFloatParameterCallback(effect_state_t* state, int index, float* value, char *valuestr)
{
    auto* processor = getProcessor(*state);
    return (processor->getFloatParameter(state, index, value, valuestr)
            ? UNITY_AUDIODSP_OK : UNITY_AUDIODSP_ERR_UNSUPPORTED);
}

int UNITY_AUDIODSP_CALLBACK
GetFloatBufferCallback(effect_state_t* state,
                       const char* name, float* buffer, int numsamples)
{
    return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
ProcessCallback(effect_state_t* state,
                float* inbuffer, float* outbuffer, unsigned int length,
                int numins, int numouts)
{
    auto* processor = getProcessor(*state);
    processor->process(state, inbuffer, outbuffer, length, numins, numouts);
    return UNITY_AUDIODSP_OK;
}

