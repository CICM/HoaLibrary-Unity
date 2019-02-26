#include "HoaLibraryApi.h"
#include <memory> // unique_ptr...
#include <algorithm> // std::fill...

namespace HoaLibraryVR {
    
    namespace
    {
        // Stores the necessary components for the HoaLibrary system. Methods called
        // from the native implementation below must check the validity of this
        // instance.
        struct HoaLibrarySystem
        {
            HoaLibrarySystem(size_t sampleframes)
            : api(CreateHoaLibraryApi(sampleframes))
            {}
            
            // HoaLibrary API instance to communicate with the internal system.
            std::unique_ptr<HoaLibraryApi> api = nullptr;
        };
        
        // Singleton instance to communicate with the internal API.
        static std::shared_ptr<HoaLibrarySystem> hoalib = nullptr;
        
    }  // namespace
    
    void Initialize(size_t vectorsize)
    {
        assert(vectorsize != 0);
        hoalib = std::make_shared<HoaLibrarySystem>(vectorsize);
    }
    
    void Shutdown()
    {
        hoalib.reset();
    }
    
    void ProcessListener(size_t frames, float_t* output)
    {
        assert(output != nullptr);
        
        const size_t channels = 2;
        auto hoalib_copy = hoalib;
        
        if (hoalib_copy == nullptr
            || !hoalib_copy->api->fillInterleavedOutputBuffer(frames, output))
        {
            // No valid output was rendered, fill the output buffer with zeros.
            const size_t buffer_size_samples = channels * frames;
            std::fill(output, output + buffer_size_samples, 0.0f);
        }
    }
    
    void SetMasterGain(float gain)
    {
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->setMasterGain(gain);
        }
    }
    
    void SetListenerTransform(float_t px, float_t py, float_t pz,
                              float_t qx, float_t qy, float_t qz, float_t qw)
    {
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->setHeadPosition(px, py, pz);
            hoalib_copy->api->setHeadRotation(qx, qy, qz, qw);
        }
    }
    
    HoaLibraryApi::source_id_t CreateSource()
    {
        auto id = HoaLibraryApi::invalid_source_id;
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            id = hoalib_copy->api->createSource();
        }
        return id;
    }
    
    void DestroySource(HoaLibraryApi::source_id_t id)
    {
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->destroySource(id);
        }
    }
    
    void ProcessSource(HoaLibraryApi::source_id_t id, size_t num_frames, float_t* inputs)
    {
        assert(inputs != nullptr);
        
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->setInterleavedSourceBuffer(id, inputs, num_frames);
        }
    }
    
    void SetSourcePan(HoaLibraryApi::source_id_t id, float_t pan)
    {
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->setSourcePan(id, pan);
        }
    }
    
    void SetSourceGain(HoaLibraryApi::source_id_t id, float_t gain)
    {
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->setSourceGain(id, gain);
        }
    }
    
    void SetSourcePosition(HoaLibraryApi::source_id_t id, float_t px, float_t py, float_t pz)
    {
        auto hoalib_copy = hoalib;
        if (hoalib_copy != nullptr)
        {
            hoalib_copy->api->setSourcePosition(id, px, py, pz);
        }
    }
}
