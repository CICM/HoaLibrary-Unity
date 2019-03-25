//==============================================================================
// HoaLibrary for Unity - version 1.0.0
// https://github.com/CICM/HoaLibrary-Unity
// Copyright (c) 2019, Eliott Paris, David Fiero, CICM, ArTeC.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
//==============================================================================

﻿using UnityEngine;
using System.Collections;
using UnityEngine.EventSystems;

/**
 * Control View rotation and navigation
 * mouse: View rotation
 * arrow keys: View navigation
 */
public class FlyCamera : MonoBehaviour
{
    public float mainSpeed = 0.80f;   // regular speed

    private float camSens = 0.25f;    // How sensitive it with mouse
    private Vector3 lastMouse = new Vector3(255, 255, 255); // kind of in the middle of the screen, rather than at the top (play)
    private float totalRun = 1.0f;

    void Update ()
    {
        // View rotation with mouse drag
        lastMouse = Input.mousePosition - lastMouse ;
        if(Input.GetMouseButton(0) && !EventSystem.current.IsPointerOverGameObject())
        {
            lastMouse = new Vector3(-lastMouse.y * camSens, lastMouse.x * camSens, 0);
            lastMouse = new Vector3(transform.eulerAngles.x + lastMouse.x , transform.eulerAngles.y + lastMouse.y, 0);

            if(lastMouse.x > 270)
            {
                lastMouse.x = Mathf.Max(lastMouse.x, 285.0f);
            }
            else if(lastMouse.x < 90)
            {
                lastMouse.x = Mathf.Min(lastMouse.x, 75.0f);
            }

            transform.eulerAngles = lastMouse;
        }
        lastMouse = Input.mousePosition;

        // View navigation with arrow keys
        Vector3 p = GetBaseInput();
        totalRun = Mathf.Clamp(totalRun * 0.5f, 1f, 1000f);
        p *= mainSpeed;

        p *= Time.deltaTime;

        Vector3 newPosition = transform.position;
        transform.Translate(p);
    }

    // returns the basic values, if it's 0 than it's not active.
    private Vector3 GetBaseInput()
    {
        Vector3 p_Velocity = new Vector3();
        if (Input.GetKey("up"))
        {
            p_Velocity += new Vector3(0, 0, 1);
        }
        if (Input.GetKey("down"))
        {
            p_Velocity += new Vector3(0, 0, -1);
        }
        if (Input.GetKey("left"))
        {
            p_Velocity += new Vector3(-1, 0, 0);
        }
        if (Input.GetKey("right"))
        {
            p_Velocity += new Vector3(1, 0, 0);
        }

        return p_Velocity;
    }
}
