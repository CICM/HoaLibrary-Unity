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
    
    //! @brief Updates the listener's position and rotation.
    void SetListenerTransform(float_t px, float_t py, float_t pz,
                              float_t qx, float_t qy, float_t qz, float_t qw);
    
    //! @brief Creates an object audio source to be spatialized.
    source_id_t CreateSource();
    
    //! @brief Removes source
    void DestroySource(source_id_t id);
    
    //! @brief Passes the next input buffer of the source to the system.
    void ProcessSource(source_id_t id, size_t num_frames, float_t* input);
    
    //! @brief Sets the stereo pan
    void SetSourcePan(source_id_t id, float_t pan);
    
    //! @brief Updates the position of the source.
    void SetSourcePosition(HoaLibraryApi::source_id_t id, float_t px, float_t py, float_t pz);
}
