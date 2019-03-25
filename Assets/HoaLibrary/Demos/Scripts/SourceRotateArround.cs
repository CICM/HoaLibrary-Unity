using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SourceRotateArround : MonoBehaviour
{
    public float RotateHSpeed = 0.0F;
    public float RotateVSpeed = 0.0F;
    public GameObject mainCamera;
    public Slider Slider1;
    public Slider Slider2;

    // Start is called before the first frame update
    void Start()
    {}

    // Update is called once per frame
    void Update()
    {
        transform.RotateAround(mainCamera.transform.position, Vector3.up, 100 * RotateHSpeed * Time.deltaTime);
        transform.RotateAround(mainCamera.transform.position, Vector3.forward, 100 * RotateVSpeed * Time.deltaTime);
    }

    public void SetHSpeed()
    {
        RotateHSpeed = Slider1.value;
    }

    public void SetVSpeed()
    {
        RotateVSpeed = Slider2.value;
    }
}
