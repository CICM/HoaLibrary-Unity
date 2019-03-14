//==============================================================================
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//
// Thirdparty :
// - [HoaLibrary-Light](https://github.com/CICM/HoaLibrary-Light)
// - Unity [nativeaudioplugins](https://bitbucket.org/Unity-Technologies/nativeaudioplugins) SDK.
//==============================================================================

#pragma once

//! @brief platform defines
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#   define HOA_PLATFORM_WIN 1
#   if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#       define HOA_PLATFORM_WINRT 1
#   endif
#elif defined(__MACH__) || defined(__APPLE__)
#   define HOA_PLATFORM_OSX 1
#elif defined(__ANDROID__)
#   define HOA_PLATFORM_ANDROID 1
#elif defined(__linux__)
#   define HOA_PLATFORM_LINUX 1
#endif

//! @brief Attribute to make function be exported from a plugin
#if HOA_PLATFORM_WIN
#   define HOA_EXPORT __declspec(dllexport)
#else
#   define HOA_EXPORT
#endif

//! @brief print message into unity log file
//! @see ~/Library/Logs/Unity/Editor.log
#ifndef HOA_LOG
#   define HOA_LOG(message) ::fprintf(stdout, message);
#endif

#include <Hoa.hpp>

#include <assert.h>
#include <atomic>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>

namespace HoaLibraryVR
{
    using namespace hoa;
    using float_t = float;
    
    class HoaLibraryApi;
    
    //! @brief Returns the number of harmonics depending on an ambisonic order.
    static constexpr size_t get_num_harmonics_for_order(const size_t order)
    {
        return (order + 1) * (order + 1);
    }
    
    extern "C"
    {
        //! @brief Factory method to create a HoaLibrary API instance.
        //! @details Caller must take ownership of returned instance and destroy it via operator delete.
        //! @param vectorsize Number of frames per buffer.
        //! @param samplerate System sample rate (Hz).
        HOA_EXPORT HoaLibraryApi* CreateHoaLibraryApi(size_t vectorsize);
    }
    
    // ==================================================================================== //
    // Source
    // ==================================================================================== //
    
    struct CartesianCoordinate
    {
        float_t x = 0.f;
        float_t y = 0.f;
        float_t z = 0.f;
    };
    
    struct PolarCoordinate
    {
        float_t radius = 0.f;
        float_t azimuth = 0.f;
        float_t elevation = 0.f;
    };
    
    class Source
    {
    public:
        
        Source(size_t order, size_t vectorsize);
        ~Source();
        
        void setGain(float_t gain);
        
        void setPan(float_t pan);
        
        void setOptim(int optim);
        
        void setInterleavedBuffer(float_t const* inputs, size_t frames);
        
        void setPosition(float_t x, float_t y, float_t z);
        
        void process(float_t** outputs, size_t frames);
        
    private:
        
        float_t m_gain = 1.f;
        float_t m_pan = 0.f;
        
        CartesianCoordinate m_source_position {};
        
        using encoder_t = hoa::Encoder<hoa::Hoa3d, float_t>;
        using optim_t = hoa::Optim<hoa::Hoa3d, float_t>;
        using optim_mode_t = hoa::Optim<hoa::Hoa3d, float_t>::Mode;
        
        encoder_t m_encoder;
        optim_t m_optim;
        
        std::vector<float_t> m_mono_input_buffer {};
        std::vector<float_t> m_temp_harmonics {};
    };
    
    // ==================================================================================== //
    // HoaLibraryApi
    // ==================================================================================== //
    
    //! @brief HoaLibrary API
    class HoaLibraryApi
    {
    public:
        
        using source_id_t = int;
        
        HoaLibraryApi(size_t vectorsize);
        
        ~HoaLibraryApi();
        
        // Invalid source id that can be used to initialize handler variables during
        // class construction.
        static const source_id_t invalid_source_id = -1;
        
        //! @brief Sets the master gain of the main audio output.
        //! @param volume Master volume (linear) in amplitude in range [0, 1] for
        //! attenuation, range [1, inf) for gain boost.
        void setMasterGain(float_t gain);
        
        //! @brief Sets the response time (could be usefull to reduce the cpu).
        void setResponseTime(float_t response_time);
        
        //! @brief Renders and outputs an interleaved output buffer in float format.
        //! @param num_frames Size of output buffer in frames.
        //! @param num_channels Number of channels in output buffer.
        //! @param buffer_ptr Raw float pointer to audio buffer.
        //! @return True if a valid output was successfully rendered, false otherwise.
        bool fillInterleavedOutputBuffer(size_t num_frames, float_t* buffer_ptr);
        
        //! @brief Creates a sound object source instance.
        //! @param rendering_mode Rendering mode which governs quality and performance.
        //! @return Id of new source.
        source_id_t createSource();
        
        //! @brief Destroys source instance.
        //! @param source_id Id of source to be destroyed.
        void destroySource(source_id_t source_id);
        
        //! @brief Sets the next audio buffer in interleaved float format to a sound source.
        //! @param source_id Id of sound source.
        //! @param audio_buffer_ptr Pointer to interleaved float audio buffer.
        //! @param num_frames Number of frames per channel (assumed stereo) in interleaved audio buffer.
        void setInterleavedSourceBuffer(source_id_t source_id,
                                        float_t const* audio_buffer_ptr,
                                        size_t num_frames);
        
        //! @brief Sets the given source's position.
        //! @details Note that, the given position for an ambisonic source is only used
        //! to determine the corresponding room effects to be applied.
        //! @param source_id Id of source.
        //! @param x X coordinate of source position in world space.
        //! @param y Y coordinate of source position in world space.
        //! @param z Z coordinate of source position in world space.
        void setSourcePosition(source_id_t source_id, float_t x, float_t y, float_t z);
        
        //! @brief Sets the stereo pan
        void setSourcePan(source_id_t source_id, float_t pan);
        
        //! @brief Sets the given source's volume.
        //! @param source_id Id of source.
        //! @param volume Linear source volume in amplitude in range [0, 1] for
        //! attenuation, range [1, inf) for gain boost.
        void setSourceGain(source_id_t source_id, float_t volume);
        
        //! @brief Sets the source optimization.
        void setSourceOptim(source_id_t source_id, int optim);
        
    private:
        
        using decoder_t = DecoderBinaural<Hoa3d, float_t, hrir::Sadie_D2_3D>;
        using hrir_t = decoder_t::hrir_t;
        
        static constexpr size_t m_output_channels = 2;
        static constexpr size_t m_order = hrir_t::getOrderOfDecomposition();
        static constexpr size_t m_num_harmonics = get_num_harmonics_for_order(m_order);
        
        const size_t m_vectorsize;
        
        // Incremental source id counter.
        std::atomic<source_id_t> m_source_id_counter {0};
        std::unordered_map<source_id_t, std::unique_ptr<Source>> m_sources;
        
        float_t m_master_gain = 1.f;
        
        std::array<float_t*, m_num_harmonics> m_soundfield_matrix;
        
        decoder_t m_decoder;
        
        std::array<float_t*, m_output_channels> m_binaural_output_matrix;
    };
}

