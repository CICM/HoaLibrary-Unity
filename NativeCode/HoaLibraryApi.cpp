#include "HoaLibraryApi.h"

namespace HoaLibraryVR
{
    HOA_EXPORT HoaLibraryApi* CreateHoaLibraryApi(size_t vectorsize)
    {
        return new HoaLibraryApi(vectorsize);
    }
    
    PolarCoordinate cartopol(CartesianCoordinate car)
    {
        PolarCoordinate pol;
        
        pol.radius = std::sqrtf(car.x * car.x + car.y * car.y + car.z * car.z);
        
        
        pol.azimuth = (hoa::math<float_t>::pi_over_two()
                       + ((car.x == 0.f && car.z == 0.f) ? 0.f : std::atan2f(car.z, car.x)));
        
        if(! (car.y == 0.f || pol.radius == 0.f))
        {
            pol.elevation = hoa::math<float_t>::two_pi() - std::asinf(car.y / pol.radius);
        }
        
        return pol;
    }
    
    // ==================================================================================== //
    // Source
    // ==================================================================================== //
    
    Source::Source(size_t order, size_t vectorsize)
    : m_encoder(order)
    , m_mono_input_buffer(vectorsize, 0.f)
    , m_temp_harmonics(m_encoder.getNumberOfHarmonics(), 0.f)
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
        m_source_position = {x, y, z};
    }
    
    void Source::process(float_t** outputs, size_t frames)
    {
        assert(frames == m_mono_input_buffer.size());
        
        auto polar_coords = cartopol(m_source_position);
        
        // we do not use radius for now
        //m_encoder.setRadius(polar_coords.radius);
        m_encoder.setAzimuth(polar_coords.azimuth);
        m_encoder.setElevation(polar_coords.elevation);
        
        auto* input = m_mono_input_buffer.data();
        for(int i = 0; i < frames; ++i)
        {
            m_encoder.process(input, m_temp_harmonics.data());
            for(int harmo = 0; harmo < m_temp_harmonics.size(); ++harmo)
            {
                outputs[harmo][i] += m_temp_harmonics[harmo];
            }
            
            input++;
        }
    }
    
    // ==================================================================================== //
    // API
    // ==================================================================================== //
    
    HoaLibraryApi::HoaLibraryApi(size_t vectorsize)
    : m_vectorsize(vectorsize)
    , m_source_id_counter(0)
    , m_master_gain(1.f)
    , m_decoder(m_order)
    {
        m_decoder.prepare(m_vectorsize);
        
        for(int channel = 0; channel < m_output_channels; ++channel)
        {
            m_binaural_output_matrix[channel] = new float[m_vectorsize];
        }
        
        for(int harmonic = 0; harmonic < m_num_harmonics; ++harmonic)
        {
            m_soundfield_matrix[harmonic] = new float[m_vectorsize];
        }
    }
    
    HoaLibraryApi::~HoaLibraryApi()
    {
        for(int i = 0; i < m_output_channels; ++i)
        {
            delete [] m_binaural_output_matrix[i];
        }
        
        for(int i = 0; i < m_num_harmonics; ++i)
        {
            delete [] m_soundfield_matrix[i];
        }
    }
    
    bool HoaLibraryApi::fillInterleavedOutputBuffer(size_t frames, float_t* outputs)
    {
        // clear output buffer
        std::fill(outputs, outputs + frames * m_output_channels, 0.f);
        
        // clear encoded source matrix
        for(float_t* harmonic_vec : m_soundfield_matrix)
        {
            std::fill(harmonic_vec, harmonic_vec + frames, 0.f);
        }
        
        for(auto& source : m_sources)
        {
            source.second->process(m_soundfield_matrix.data(), frames);
        }
        
        auto soundfield_matrix = const_cast<float_t const**>(m_soundfield_matrix.data());
        
        float_t** binaural_output_matrix = m_binaural_output_matrix.data();
        
        m_decoder.processBlock(soundfield_matrix, binaural_output_matrix);
        
        // convert matrix to interleaved vector and apply gain
        const size_t numouts = 2;
        for (int i = 0; i < frames; ++i)
        {
            for(int channel = 0; channel < numouts; ++channel)
            {
                outputs[i * numouts + channel] = binaural_output_matrix[channel][i] * m_master_gain;
            }
        }
        
        return true;
    }
    
    void HoaLibraryApi::setMasterGain(float_t gain)
    {
        m_master_gain = gain;
    }
    
    void HoaLibraryApi::setHeadPosition(float_t x, float_t y, float_t z)
    {
        m_listener_position = {x, y, z};
    }
    
    void HoaLibraryApi::setHeadRotation(float_t x, float_t y, float_t z, float_t w)
    {
        
    }
    
    auto HoaLibraryApi::createSource() -> source_id_t
    {
        const auto source_id = m_source_id_counter.fetch_add(1);
        assert(m_sources.find(source_id) == m_sources.end());
        const auto order = m_order; // (silent symbol not found issue on osx)
        m_sources[source_id] = std::make_unique<Source>(order, m_vectorsize);
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
            source->second->setPosition(x - m_listener_position.x,
                                        y - m_listener_position.y,
                                        z - m_listener_position.z);
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
