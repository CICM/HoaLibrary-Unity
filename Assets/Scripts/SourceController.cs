﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SourceController : MonoBehaviour
{
  [Tooltip("Azimuth of the object")]
  [Range(0.0f, 360.0f)]
  public float azimuth = 0;

  [Tooltip("Elevation of the object")]
  [Range(-90.0f, 90.0f)]
  public float elevation = 0;

  [Tooltip("Radius of the object")]
  [Range(0.0f, 20.0f)]
  public float radius = 0;

  const float TWO_PI = Mathf.PI * 2f;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    transform.position = SphericalToCartesian(azimuth,elevation,radius);
  }

  //On ajoute pi/2 a lazimuth pour corriger la position
  public static Vector3 SphericalToCartesian(float azimuth_, float elevation_, float radius_)
  {
      float a = radius_ * Mathf.Cos(elevation_ * TWO_PI / 360f);
      float x = a * Mathf.Cos((azimuth_ + 90f) * TWO_PI / 360f);
      float y = radius_ * Mathf.Sin(elevation_ * TWO_PI / 360f);
      float z = a * Mathf.Sin((azimuth_ + 90f) * TWO_PI / 360f);

      return new Vector3(x, y, z);
  }
}
