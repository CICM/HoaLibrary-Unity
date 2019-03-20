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

#include "HoaLibraryApi.h"

namespace HoaLibraryVR
{
    HOA_EXPORT HoaLibraryApi* CreateHoaLibraryApi(size_t vectorsize)
    {
        return new HoaLibraryApi(vectorsize);
    }
    
    SphericalCoordinate cartopol(CartesianCoordinate car)
    {
        SphericalCoordinate pol;
        
        pol.radius = std::sqrtf(car.x * car.x + car.y * car.y + car.z * car.z);
        
        // azimuth 0 in hoa system is in front.
        pol.azimuth = (((car.x == 0.f && car.z == 0.f) ? 0.f : std::atan2f(car.z, car.x))
                       - hoa::math<float_t>::pi_over_two());
        
        if(! (car.y == 0.f || pol.radius == 0.f))
        {
            pol.elevation = std::asinf(car.y / pol.radius);
        }
        
        return pol;
    }
    
    void SmoothedCartesianCoordinate::setRamp(const size_t ramp)
    {
        m_x.setRamp(ramp);
        m_y.setRamp(ramp);
        m_z.setRamp(ramp);
    }
    
    void SmoothedCartesianCoordinate::setValues(CartesianCoordinate car)
    {
        m_x.setValue(car.x);
        m_y.setValue(car.y);
        m_z.setValue(car.z);
    }
    
    CartesianCoordinate SmoothedCartesianCoordinate::getValues() const
    {
        return {m_x.getValue(), m_y.getValue(), m_z.getValue()};
    }
    
    CartesianCoordinate SmoothedCartesianCoordinate::process()
    {
        return {m_x.process(), m_y.process(), m_z.process()};
    }
    
    // ==================================================================================== //
    // Source
    // ==================================================================================== //
    
    Source::Source(size_t order, size_t vectorsize)
    : m_encoder(order)
    , m_optim(order)
    , m_mono_input_buffer(vectorsize)
    , m_temp_harmonics(m_encoder.getNumberOfHarmonics())
    {
        m_smoothed_position.setRamp(1100); // in samps (± 25ms at 44.1kHz)
        m_optim.setMode(optim_mode_t::Basic);
        
        m_mono_input_buffer.setZero();
        m_temp_harmonics.setZero();
    }
    
    Source::~Source()
    {}
    
    void Source::setPan(float_t pan)
    {
        m_pan = pan;
    }
    
    void Source::setOptim(int optim_int)
    {
        auto optim = static_cast<optim_mode_t>(optim_int);
        if(optim != m_optim.getMode())
        {
            m_optim.setMode(optim);
        }
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
        
        auto const& stereo_input = stereo_matrix_t::Map(inputs, 2, frames);
        
        m_mono_input_buffer.noalias() = ((stereo_input.row(0) * left_gain)
                                         + (stereo_input.row(1) * right_gain)) * m_gain;
    }
    
    void Source::setPosition(float_t x, float_t y, float_t z)
    {
        m_smoothed_position.setValues({x, y, z});
    }
    
    void Source::process(harmonics_matrix_t& harmonics_matrix)
    {
        assert(harmonics_matrix.cols() == m_mono_input_buffer.size());
        
        const bool process_optim = m_optim.getMode() != optim_mode_t::Basic;
        
        auto* input = m_mono_input_buffer.data();
        for(auto harmonic_vector : harmonics_matrix.colwise())
        {
            auto polar_coords = cartopol(m_smoothed_position.process());
            
            if(m_encoder.getRadius() != polar_coords.radius)
            {
                // We let unity provide gain attenuation when the source is farther than 1 meter.
                // @todo Set it to "minimum distance" instead of the arbitrary 1 meter value.
                
                m_encoder.setRadius(std::min<float_t>(polar_coords.radius, 1.0f));
            }
            
            if(m_encoder.getAzimuth() != polar_coords.azimuth)
            {
                m_encoder.setAzimuth(polar_coords.azimuth);
            }
            
            if(m_encoder.getElevation() != polar_coords.elevation)
            {
                m_encoder.setElevation(polar_coords.elevation);
            }
            
            auto* harmonics = m_temp_harmonics.data();
            m_encoder.process(input++, harmonics);
            
            if(process_optim)
            {
                m_optim.process(harmonics, harmonics);
            }
            
            harmonic_vector += m_temp_harmonics;
        }
    }
    
    // ==================================================================================== //
    // API
    // ==================================================================================== //
    
    HoaLibraryApi::HoaLibraryApi(size_t vectorsize)
    : m_vectorsize(vectorsize)
    , m_source_id_counter(0)
    , m_master_gain(1.f)
    , m_decoder(k_order)
    {
        m_decoder.prepare(m_vectorsize);        
        m_soundfield_matrix.resize(k_num_harmonics, m_vectorsize);
    }
    
    HoaLibraryApi::~HoaLibraryApi()
    {}
    
    bool HoaLibraryApi::fillInterleavedOutputBuffer(size_t frames, float_t* outputs)
    {
        m_soundfield_matrix.setZero();
        
        for(auto& source : m_sources)
        {
            source.second->process(m_soundfield_matrix);
        }
        
        auto outs = stereo_matrix_t::Map(outputs, 2, frames);
        m_decoder.processBlock(m_soundfield_matrix, outs);
        outs *= m_master_gain;
        
        return true;
    }
    
    void HoaLibraryApi::setMasterGain(float_t gain)
    {
        m_master_gain = gain;
    }
    
    auto HoaLibraryApi::createSource() -> source_id_t
    {
        const auto source_id = m_source_id_counter.fetch_add(1);
        assert(m_sources.find(source_id) == m_sources.end());
        const auto order = k_order; // (silent symbol not found issue on osx)
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
    
    void HoaLibraryApi::setSourceOptim(source_id_t source_id, int optim)
    {
        auto source = m_sources.find(source_id);
        if(source != m_sources.end())
        {
            source->second->setOptim(optim);
        }
    }
}
