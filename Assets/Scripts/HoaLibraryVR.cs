//==============================================================================
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//==============================================================================

using UnityEngine;
using UnityEngine.Audio;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;


// HoaLibraryVR supports Unity 2017.1 or newer.
#if !UNITY_2017_1_OR_NEWER
  #error HoaLibraryVR SDK requires Unity 2017.1 or newer.
#endif  // !UNITY_2017_1_OR_NEWER

/// This is the main HoaLibraryVR class that communicates with the native code implementation of
/// the audio system.
public static class HoaLibraryVR {

#if UNITY_EDITOR
  /// Default audio mixer group of the renderer.
  public static AudioMixerGroup MixerGroup {
    get {
      if (mixerGroup == null) {
        AudioMixer mixer = (Resources.Load("HoaLibraryVRAudioMixer") as AudioMixer);
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
  public const string spatializerPluginName = "HoaLibrary VR Spatializer";

  /// Renderer plugin name.
  public const string rendererPluginName = "HoaLibrary VR Renderer";

  /// Native plugin name.
  private const string pluginName = "AudioPluginHoaLibraryVR";
}
