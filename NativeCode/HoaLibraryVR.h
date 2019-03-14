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

#include "HoaLibraryApi.h"

namespace HoaLibraryVR
{
    using source_id_t = HoaLibraryApi::source_id_t;
    
    //! @brief Initializes the HoaLibrary system with Unity audio engine settings.
    void Initialize(size_t vectorsize);
    
    //! @brief Shuts down the HoaLibrary system.
    void Shutdown();
    
    //! @brief Processes the next output buffer and stores the processed buffer in |output|.
    //! This method must be called from the audio thread.
    void ProcessListener(size_t num_frames, float_t* output);
    
    //! @brief Updates the listener's master gain.
    void SetMasterGain(float_t gain);
    
    //! @brief Sets the response time (could be usefull to reduce the cpu).
    void SetResponseTime(float_t response_time);
    
    //! @brief Creates an object audio source to be spatialized.
    source_id_t CreateSource();
    
    //! @brief Removes source
    void DestroySource(source_id_t id);
    
    //! @brief Passes the next input buffer of the source to the system.
    void ProcessSource(source_id_t id, size_t num_frames, float_t* input);
    
    //! @brief Sets the stereo pan
    void SetSourcePan(source_id_t id, float_t pan);
    
    //! @brief Sets the stereo pan
    void SetSourceGain(source_id_t id, float_t gain);
    
    //! @brief Updates the position of the source.
    void SetSourcePosition(HoaLibraryApi::source_id_t id,
                           float_t px, float_t py, float_t pz);
    
    //! @brief Sets the source ambisonic optimization.
    void SetSourceOptim(HoaLibraryApi::source_id_t id, int optim);
}
