//==============================================================================
// HoaLibrary for Unity - version 1.0.0
// https://github.com/CICM/HoaLibrary-Unity
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//==============================================================================

using UnityEngine;
using UnityEngine.Audio;

// HoaLibraryUnity supports Unity 2017.1 or newer.
#if !UNITY_2017_1_OR_NEWER
  #error HoaLibrary SDK requires Unity 2017.1 or newer.
#endif  // !UNITY_2017_1_OR_NEWER

public static class HoaLibrary {

#if UNITY_EDITOR
  /// Default audio mixer group of the renderer.
  public static AudioMixerGroup MixerGroup {
    get {
      if (mixerGroup == null) {
        AudioMixer mixer = (Resources.Load("HoaLibraryAudioMixer") as AudioMixer);
        if (mixer != null) {
          mixerGroup = mixer.FindMatchingGroups("Master")[0];
        }
      }
      return mixerGroup;
    }
  }
  private static AudioMixerGroup mixerGroup = null;
#endif  // UNITY_EDITOR

  /// Spatializer plugin name.
  public const string spatializerPluginName = "HoaLibrary Spatializer";

  /// Renderer plugin name.
  public const string rendererPluginName = "HoaLibrary Renderer";

  /// Native plugin name.
  private const string pluginName = "AudioPluginHoaLibrary";
}
