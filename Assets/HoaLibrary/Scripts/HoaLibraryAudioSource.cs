//==============================================================================
// HoaLibrary for Unity - version 1.0.0
// https://github.com/CICM/HoaLibrary-Unity
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//==============================================================================

using UnityEngine;

/// HoaLibrary Audio source component that adds more spatial audio settings.
[AddComponentMenu("HoaLibrary/AudioSource")]
[RequireComponent(typeof(AudioSource))]
[ExecuteInEditMode]
public class HoaLibraryAudioSource : MonoBehaviour {

  /// Ambisonic optimization.
  public enum Optim {
    Basic = 0,    ///< Has no effect.
    MaxRe = 1,    ///< Energy vector optimization.
    InPhase = 2   ///< Energy and velocity vector optimization
  }

  /// Input gain in decibels.
  [Tooltip("Additional gain for this source")]
  [Range(0.0f, 20.0f)]
  public float gain = 0.0f;

  /// Rendering quality of the audio source.
  [Tooltip("Sets the ambisonic optimization mode for this source (Basic | MaxRe | InPhase)")]
  public Optim optim = Optim.Basic;

  /// Unity audio source attached to the game object.
  public AudioSource audioSource { get; private set; }

  // Native audio spatializer effect data.
  private enum EffectData {
    DistanceAttenuation = 0,    // Distance attenuation.
    Gain = 1,                   // Gain.
    CustomFalloff = 2,          // Custom Falloff
    Optim = 3,                  // Optimization.
  }

  void Awake() {
    audioSource = GetComponent<AudioSource>();
  }

#if UNITY_EDITOR
  void OnEnable() {
#if UNITY_2017_2_OR_NEWER
    // Validate the spatializer plugin selection.
    if (AudioSettings.GetSpatializerPluginName() != HoaLibrary.spatializerPluginName) {
      Debug.LogWarning(HoaLibrary.spatializerPluginName + " must be selected as the " +
                       "Spatializer Plugin in Edit > Project Settings > Audio.");
    }
#endif  // UNITY_2017_2_OR_NEWER
    // Validate the source output mixer route.
    if (HoaLibrary.MixerGroup == null || audioSource.outputAudioMixerGroup != HoaLibrary.MixerGroup) {
      Debug.LogWarning("Make sure AudioSource is routed to a mixer that "+ HoaLibrary.rendererPluginName + " is attached to.");
    }
  }
#endif  // UNITY_EDITOR

  void Update() {
    if (audioSource.spatialize) {
      audioSource.SetSpatializerFloat((int) EffectData.Gain, gain);
      audioSource.SetSpatializerFloat((int) EffectData.Optim, (float) optim);
    }
  }
}
