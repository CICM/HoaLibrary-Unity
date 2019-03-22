//==============================================================================
// HoaLibrary for Unity - version 1.0.0
// https://github.com/CICM/HoaLibrary-Unity
// Copyright (c) 2019, Eliott Paris, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//==============================================================================

﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[AddComponentMenu("HoaLibraryUnity/AudioMapping")]
[ExecuteInEditMode]
public class AudioMapping : MonoBehaviour
{
  public bool amplitudeScaleEnabled = true;

  private int qSamples = 1024;    // array size
  private float refValue = 0.1F;  // RMS value for 0 dB
  private float rmsValue;         // sound level - RMS
  private float dbValue;          // sound level - dB
  private float volume = 2;       // set how much the scale will vary
  private float minScale = 2;     // minimum scale
  private float[] samples;        // audio samples

  // Start is called before the first frame update
  void Start()
  {
      samples = new float[qSamples];
  }

  // Update is called once per frame
  void Update()
  {
      if(amplitudeScaleEnabled)
      {
          GetVolume();
          float scale = volume * rmsValue + minScale;
          transform.localScale = new Vector3(scale, scale, scale);
      }
  }

  private void GetVolume()
  {
      GetComponent<AudioSource>().GetOutputData(samples, 0); // fill array with samples
      float sum = 0;
      foreach (float sample in samples)
      {
          sum += sample * sample; // sum squared samples
      }

      rmsValue = Mathf.Sqrt(sum/qSamples); // rms = square root of average
      dbValue = 20*Mathf.Log10(rmsValue/refValue); // calculate dB

      if (dbValue < -160)
      {
          dbValue = -160; // clamp it to -160dB min
      }
  }
}
