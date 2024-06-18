#include "Camera.h"

Camera::Camera(float focalLength, float fAW, float fAH, float nCP, float fCP)
    : _focalLength(focalLength), _filmApertureWidth(fAW), _filmApertureHeight(fAH), _nearClippingPlane(nCP), _farClippingPlane(fCP) {}

