// This is a class that will create a camera object from which the scene will be rendered in.
#pragma once

#include "geometry.h"

class Camera
{
public:
    Camera(float focalLength, float fAW, float fAH, float nCP, float fCP, Vec3f pos, Vec3f rot);    
    Matrix44f getCameraToWorld();

    Vec3f position;     // Position in world coordinates
    Vec3f rotation;     // Rotation ...
private:
    float _focalLength;         // Distance between the eye and the image plane

    float _filmApertureWidth;   // Used to determine the angle of view (AOV) and film gate aspect ratio
    float _filmApertureHeight;  // ...

    float _nearClippingPlane;   // Determines the minimum distance from the eye for objects to be rendered in the camera's view.
    float _farClippingPlane;    // Determines the maximum distance ...
};