#include <iostream>
#include "Camera.h"

int main(int argc, char const *argv[])
{
    Vec3f position(0.5, -9, 3.5);
    Vec3f rotation(77, 0, 5);
    Camera camera(50, 35, 24, 0.1, 500, position, rotation);

    Matrix44f cameraToWorld = camera.getCameraToWorld();
    std::cout << cameraToWorld;
    return 0;
}
