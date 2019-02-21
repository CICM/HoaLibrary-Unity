#include "HoaLibraryApi.h"

namespace HoaLibraryVR
{
    HOA_EXPORT HoaLibraryApi* CreateHoaLibraryApi(size_t vectorsize)
    {
        return new HoaLibraryApi(vectorsize);
    }
    
    // ==================================================================================== //
    // Source
    // ==================================================================================== //
    
    Source::Source(size_t order, size_t vectorsize)
    : m_mono_input_buffer(vectorsize, 0.f)
    {}
    
    Source::~Source()
    {}
    
    void Source::setPan(float_t pan)
    {
        m_pan = pan;
    }
    
    void Source::setGain(float_t gain)
    {
        m_gain = std::max<float_t>(0.f, gain);
    }
    
    void Source::setInterleavedBuffer(float_t const* inputs, size_t frames)
    {
        assert(frames == m_mono_input_buffer.size() && "");
        
        const float_t left_gain = (1.f - m_pan) * 0.5;
        const float_t right_gain = (1.f + m_pan) * 0.5;
        
        float_t const* input = inputs;
        float_t* output = m_mono_input_buffer.data();
        
        for(int i = 0; i < frames; ++i)
        {
            // mono mix
            *output++ = (((*input) * left_gain)
                         + ((*(input+1)) * right_gain));
            input += 2;
        }
    }
    
    void Source::setPosition(float_t x, float_t y, float_t z)
    {
        
    }
    
    void Source::process(float_t* outputs, size_t frames)
    {
        //std::copy(outputs, outputs + frames, outputs);
    }
    
    std::vector<float_t>& Source::getBuffer()
    {
        return m_mono_input_buffer;
    }
    
    // ==================================================================================== //
    // API
    // ==================================================================================== //
    
    HoaLibraryApi::HoaLibraryApi(size_t vectorsize)
    : m_vectorsize(vectorsize)
    , m_source_id_counter(0)
    , m_master_gain(1.f)
    {}
    
    HoaLibraryApi::~HoaLibraryApi()
    {}
    
    bool HoaLibraryApi::fillInterleavedOutputBuffer(size_t frames, float_t* outputs)
    {
        // clear output buffer
        std::fill(outputs, outputs + frames * 2, 0.f);
        
        for(auto& source : m_sources)
        {
            auto& mono_input_buffer = source.second->getBuffer();
            
            const size_t numouts = 2;
            for (int i = 0; i < frames; ++i)
            {
                const auto value = mono_input_buffer[i];
                
                for(int channel = 0; channel < numouts; ++channel)
                {
                    outputs[i * numouts + channel] += value;
                }
            }
        }
        
        std::transform(outputs, outputs + frames * 2, outputs,
                       [gain = m_master_gain](auto& c){return c * gain;});
        
        return true;
    }
    
    void HoaLibraryApi::setMasterGain(float_t gain)
    {
        m_master_gain = gain;
    }
    
    void HoaLibraryApi::setHeadPosition(float_t x, float_t y, float_t z)
    {
        
    }
    
    void HoaLibraryApi::setHeadRotation(float_t x, float_t y, float_t z, float_t w)
    {
        
    }
    
    auto HoaLibraryApi::createSource() -> source_id_t
    {
        const auto source_id = m_source_id_counter.fetch_add(1);
        assert(m_sources.find(source_id) == m_sources.end());
        m_sources[source_id] = std::make_unique<Source>(m_order, m_vectorsize);
        return source_id;
    }
    
    void HoaLibraryApi::destroySource(source_id_t source_id)
    {
        assert(m_sources.find(source_id) != m_sources.end());
        m_sources.erase(source_id);
    }
    
    void HoaLibraryApi::setInterleavedSourceBuffer(source_id_t source_id,
                                                   float_t const* audio_buffer_ptr, size_t num_frames)
    {
        auto source = m_sources.find(source_id);
        if(source != m_sources.end())
        {
            source->second->setInterleavedBuffer(audio_buffer_ptr, num_frames);
        }
    }
    
    void HoaLibraryApi::setSourcePosition(source_id_t source_id,
                                          float_t x, float_t y, float_t z)
    {
        auto source = m_sources.find(source_id);
        if(source != m_sources.end())
        {
            source->second->setPosition(x, y, z);
        }
    }
    
    void HoaLibraryApi::setSourcePan(source_id_t source_id, float_t pan)
    {
        auto source = m_sources.find(source_id);
        if(source != m_sources.end())
        {
            source->second->setPan(pan);
        }
    }
    
    void HoaLibraryApi::setSourceGain(source_id_t source_id, float_t volume)
    {
        auto source = m_sources.find(source_id);
        if(source != m_sources.end())
        {
            source->second->setGain(volume);
        }
    }
}
