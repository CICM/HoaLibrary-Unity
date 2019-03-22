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

#include "HoaLibraryUnity.h"

#include "AudioPluginInterface.h"
#include "AudioPluginUtil.h"

namespace HoaLibrary_Renderer
{
    using HoaLibraryUnity::float_t;
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
        enum Param
        {
            MasterGain,
            Size
        };

        HoaAudioProcessor() = default;
        ~HoaAudioProcessor() = default;

        static int registerEffect(effect_definition_t& definition)
        {
            int numparams = Param::Size;
            definition.paramdefs = new param_definition_t[numparams];

            RegisterParameter(definition, "Master Gain", "dB",
                              -120.f, 50.f, 0.0f, 1.0f, 1.0f,
                              Param::MasterGain, "Master Gain");

            return numparams;
        }

        //! @brief Called when the plugin is created
        void create(effect_state_t* state)
        {
            assert(state);
            state->effectdata = this;
            InitParametersFromDefinitions(registerEffect, p.data());
            const size_t vectorsize = static_cast<size_t>(state->dspbuffersize);
            HoaLibraryUnity::Initialize(vectorsize);
        }

        //! @brief Release ressources.
        void release()
        {
            HoaLibraryUnity::Shutdown();
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

        void process(effect_state_t* state,
                     float_t* inputs, float_t* outputs, unsigned int length,
                     int numins, int numouts)
        {
            const bool is_muted = state->flags & (UnityAudioEffectStateFlags_IsMuted);
            const bool is_paused = state->flags & (UnityAudioEffectStateFlags_IsPaused);
            const bool is_playing = state->flags & (UnityAudioEffectStateFlags_IsPlaying);

            const int stereo = 2;

            // Check that I/O formats are right
            if ((numins != stereo || numouts != stereo)
                || (is_muted || is_paused || !is_playing))
            {
                // fill with zeros
                std::fill(outputs, outputs + length * numouts, 0.f);

                // or copy inputs to outputs ?
                // const size_t buffer_size_per_channel_bytes = length * sizeof(float_t);
                // const size_t buffer_size_bytes = buffer_size_per_channel_bytes * numouts;
                // memcpy(outputs, inputs, buffer_size_bytes);

                return;
            }

            const auto gain = std::powf(10.f, p[Param::MasterGain] * 0.05f);

            HoaLibraryUnity::SetMasterGain(gain);
            HoaLibraryUnity::ProcessListener(length, outputs);
        }

    private:

        std::array<float_t, Param::Size> p;
    };

    #include "UnityCallbacks.hpp"

}
