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

#include <assert.h>
#include <atomic>
#include <vector>
#include <array>
#include <unordered_map>

namespace HoaLibraryVR
{
    using float_t = float;
    
    class HoaLibraryApi;
    
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
    
    class Source
    {
    public:
        
        Source(size_t order, size_t vectorsize);
        ~Source();
        
        void setGain(float_t gain);
        
        void setPan(float_t pan);
        
        void setInterleavedBuffer(float_t const* inputs, size_t frames);
        
        void setPosition(float_t x, float_t y, float_t z);
        
        void process(float_t* outputs, size_t frames);
        
        std::vector<float_t>& getBuffer();
        
    private:
        
        float_t m_gain = 1.f;
        float_t m_pan = 0.f;
        std::vector<float_t> m_mono_input_buffer {};
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
        
        //! @brief Renders and outputs an interleaved output buffer in float format.
        //! @param num_frames Size of output buffer in frames.
        //! @param num_channels Number of channels in output buffer.
        //! @param buffer_ptr Raw float pointer to audio buffer.
        //! @return True if a valid output was successfully rendered, false otherwise.
        bool fillInterleavedOutputBuffer(size_t num_frames, float_t* buffer_ptr);
        
        //! @brief Sets listener's head position.
        //! @param x X coordinate of head position in world space.
        //! @param y Y coordinate of head position in world space.
        //! @param z Z coordinate of head position in world space.
        void setHeadPosition(float_t x, float_t y, float_t z);
        
        //! @brief Sets listener's head rotation.
        //! @param x X component of quaternion.
        //! @param y Y component of quaternion.
        //! @param z Z component of quaternion.
        //! @param w W component of quaternion.
        void setHeadRotation(float_t x, float_t y, float_t z, float_t w);
        
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
        // @param source_id Id of source.
        // @param x X coordinate of source position in world space.
        // @param y Y coordinate of source position in world space.
        // @param z Z coordinate of source position in world space.
        void setSourcePosition(source_id_t source_id, float_t x, float_t y, float_t z);
        
        //! @brief Sets the stereo pan
        void setSourcePan(source_id_t source_id, float_t pan);
        
        //! @brief Sets the given source's volume.
        //! @param source_id Id of source.
        //! @param volume Linear source volume in amplitude in range [0, 1] for
        //! attenuation, range [1, inf) for gain boost.
        void setSourceGain(source_id_t source_id, float_t volume);
        
    private:
        
        static constexpr size_t m_output_channels = 2;
        const size_t m_order = 5;
        const size_t m_vectorsize;
        
        // Incremental source id counter.
        std::atomic<source_id_t> m_source_id_counter {0};
        std::unordered_map<source_id_t, std::unique_ptr<Source>> m_sources;
        
        float_t m_master_gain = 1.f;
    };
}

