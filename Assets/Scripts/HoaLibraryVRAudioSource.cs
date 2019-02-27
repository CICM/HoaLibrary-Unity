//==============================================================================
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//==============================================================================

using UnityEngine;

/// HoaLibraryVR Audio source component that adds more spatial audio settings.
[AddComponentMenu("HoaLibraryVR/AudioSource")]
[RequireComponent(typeof(AudioSource))]
[ExecuteInEditMode]
public class HoaLibraryVRAudioSource : MonoBehaviour {

  /// Ambisonic optimization.
  public enum Optim {
    Basic = 0,  ///< No effect.
    MaxRe = 1,  ///< Low quality binaural rendering (first-order HRTF).
    InPhase = 2  ///< High quality binaural rendering (third-order HRTF).
  }

  /// Input gain in decibels.
  // [Tooltip("Applies a gain to the source for adjustment of relative loudness.")]
  // [Range(-120.0f, 50.0f)]
  // public float gain = 0.0f;

  /// Rendering quality of the audio source.
  [Tooltip("Sets the ambisonic optimization mode for this source (Basic | MaxRe | InPhase)")]
  public Optim optim = Optim.Basic;

  /// Unity audio source attached to the game object.
  public AudioSource audioSource { get; private set; }

  // Native audio spatializer effect data.
  private enum EffectData {
    DistanceAttenuation = 0, // Computed distance attenuation.
    Gain = 1,  // Gain.
    CustomFalloff = 2,
    Optim = 3,  // Source audio rendering quality.
  }

  void Awake() {
    audioSource = GetComponent<AudioSource>();
  }

#if UNITY_EDITOR
  void OnEnable() {
#if UNITY_2017_2_OR_NEWER
    // Validate the spatializer plugin selection.
    if (AudioSettings.GetSpatializerPluginName() != HoaLibraryVR.spatializerPluginName) {
      Debug.LogWarning(HoaLibraryVR.spatializerPluginName + " must be selected as the " +
                       "Spatializer Plugin in Edit > Project Settings > Audio.");
    }
#endif  // UNITY_2017_2_OR_NEWER
    // Validate the source output mixer route.
    if (HoaLibraryVR.MixerGroup == null || audioSource.outputAudioMixerGroup != HoaLibraryVR.MixerGroup) {
      Debug.LogWarning("Make sure AudioSource is routed to a mixer that "+ HoaLibraryVR.rendererPluginName + " is attached to.");
    }
  }
#endif  // UNITY_EDITOR

  void Update() {
    if (audioSource.spatialize) {
      // audioSource.SetSpatializerFloat((int) EffectData.Gain, gain);
      audioSource.SetSpatializerFloat((int) EffectData.Optim, (float) optim);
    }
  }
}
