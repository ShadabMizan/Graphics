#include "geometry.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

bool readVerticesFile(std::string filename);

bool computeCoordinates
(
    const Vec3f &pWorld,            
    const Matrix44f &worldToCamera, 
    const float &b,                 
    const float &l,
    const float &t,
    const float &r,
    const float &near,              
    const uint32_t &imageWidth,     
    const uint32_t &imageHeight,
    Vec2i &pRaster                  
);

void renderObject
(
    float fLength,              
    float fAW,                  
    float fAH,                  
    float nCP,                  
    float fCP,                  
    Matrix44f cameraToWorld,    
    std::string filename        
);

Matrix44f getCameraToWorld(float s1, float s2, float s3, float x, float y, float z);

// Two vectors, one to store all vertex coordinates, and the other to store which indices of vertices in the vertices vector are linked together to make a triangle
std::vector<Vec3f> vertices;
std::vector<int> triangles;

// Float casts for trig functions
float cosf(float angle) { return (float)(cos(angle * std::numbers::pi/180)); }
float sinf(float angle) { return (float)(sin(angle * std::numbers::pi/180)); }

int main(int argc, char const *argv[])
{
    if (!readVerticesFile("headphones.txt")) 
    {
        return 1;
    }

    // Ex. 1
    // Camera is placed at (0.5, -9, 3.5), rotated 77deg around X and 5deg around Z.
    renderObject(50, 35, 24, 0.1, 100, getCameraToWorld(77, 0, 5, 0.5, -9, 3.5), "./headphones1.svg");

    // Ex. 2
    // Camera looks from below the object
    renderObject(48, 35, 24, 0.1, 100, getCameraToWorld(113, 30, 39, 5.3, -10, -2.75), "./headphones2.svg");

    // Ex. 3
    // Camera is zoomed out (focal length is smaller)
    renderObject(17, 35, 24, 0.1, 100, getCameraToWorld(67.2, 0, -24, -3.3, -6, 5), "./headphones3.svg");

    // Ex. 4
    // Camera is zoomed in, with some vertices outside of the FOV
    renderObject(156, 35, 24, 0.1, 100, getCameraToWorld(51, 0, -135, -7.8, 7.5, 10.2), "./headphones4.svg");

    return 0;
}

// [comment]
// Reads from a text file generated from Blender that contains all the vertices of a 3D object and how they are connected to each other. 
// Stores the vertices in a vertices vector, and which points are connected to each other in a triangle vector

// Text file should have the form:
// vertex Flag:
// x1 x2 x3
// x1 x2 x3
// ...
// x1 x2 x3
// connection Flag:
// a b c ...

// [/comment]

bool readVerticesFile(std::string filename)
{
    std::ifstream inputFile(filename);

    if (inputFile.is_open())
    {   
        std::string line;
        bool readingVertices;
        bool readingTris;
        int vertexCount = 0;

        while (std::getline(inputFile, line))
        {
            if (line == "Array of vertices:")
            {
                readingVertices = true;
            } else if (line == "Array of connected vertices:")
            {
                readingTris = true;
            } 
            else if (readingVertices)
            {
                // Read next 8 lines, each containing 3 floats
                std::stringstream ss(line);
                float x, y, z;
                ss >> x >> y >> z;
                Vec3f vertex = {x,y,z};
                vertices.push_back(vertex);
                vertexCount++;
                if (vertexCount % 8 == 0) { readingVertices = false; }
            } else if (readingTris)
            {
                // Read next line
                std::stringstream ss(line);
                int index;
                while (ss >> index) 
                { 
                    // indices in text file are relative to the object's vertices, not the entire list.
                    index += 8*((vertexCount/8) - 1);
                    triangles.push_back(index);
                }
                readingTris = false;
            } else 
            {
                continue;
            }
        }
        return 1;
    } else {
        std::cerr << "Could not open file";
        return 0;
    }
}

// [comment]
// Get a cameraToWorld matrix, which is defined to be how the camera's transformation can be described relative to global coordinates.
// Used to calculate coordinates in the screen space before converting to raster space.
// First 3 parameters are rotations around x, y, and z axes respectively, and the next three describe the translation of the camera.
// Parameters should be taken from Blender Camera settings to replicate
// [/comment]
Matrix44f getCameraToWorld(float s1, float s2, float s3, float x, float y, float z)
{
    // Rotation around X
    Matrix44f rotx = 
    {
        1, 0, 0, 0,
        0, cosf(s1), sinf(s1), 0,
        0, -sinf(s1), cosf(s1), 0,
        0, 0, 0, 1
    };

    // Rotation around Y (Negative because x-axis points in opposite direction than convenetional when looking down the y-axis)
    Matrix44f roty = 
    {
        cosf(-s2), 0, sinf(-s2), 0,
        0, 1, 0, 0,
        -sinf(-s2), 0, cosf(-s2), 0,
        0, 0, 0, 1
    };

    // Rotation around Z
    Matrix44f rotz = 
    {
       cosf(s3), sinf(s3), 0, 0,
       -sinf(s3), cosf(s3), 0, 0, 
       0, 0, 1, 0,
       0, 0, 0, 1 
    };

    // Translation to (x,y,z)
    Matrix44f translation = 
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };

    // Compose 3 Rotations and a translation together
    Matrix44f temp;
    Matrix44f::multiply(rotx, roty, temp);

    Matrix44f temp2;
    Matrix44f::multiply(temp, rotz, temp2);

    Matrix44f cameraToWorld;
    Matrix44f::multiply(temp2, translation, cameraToWorld);

    return cameraToWorld;
}

// [comment]
// Function is adapted from https://github.com/scratchapixel/scratchapixel-code/blob/main/3d-viewing-pinhole-camera/pinhole.cpp
// Used to compute the raster coordinates of a point in the world, returning whether that point is visible or not. 
// However, it will also assign a Vec2<int> value to the corresponding world coordinate, which can then be drawn.
// [/comment]
bool computeCoordinates
(
    const Vec3f &pWorld,            // Point in the world to transform to raster space
    const Matrix44f &worldToCamera, // worldToCamera matrix
    const float &b,                 // bottom, left, top, and right boundaries of image plane
    const float &l,
    const float &t,
    const float &r,
    const float &near,              // distance between eye and canvas
    const uint32_t &imageWidth,     // Dimensions of final image.
    const uint32_t &imageHeight,
    Vec2i &pRaster                  // Point in raster space to affect
)
{
    Vec3f pCamera; // Initialized as (0,0,0)
    worldToCamera.multVecMatrix(pWorld, pCamera); // Transform a point from pWorld into coordinates relative to pCamera.
    
    // Screen coordinates
    Vec2f pScreen;
    pScreen.x = pCamera.x / -pCamera.z * near;
    pScreen.y = pCamera.y / -pCamera.z * near;
    
    // Normalized Device coordinates
    Vec2f pNDC;
    pNDC.x = (pScreen.x + r) / (2 * r);
    pNDC.y = (pScreen.y + t) / (2 * t);

    // Raster coordinates
    pRaster.x = (int)(pNDC.x * imageWidth);
    pRaster.y = (int)((1 - pNDC.y) * imageHeight);

    // Check if point lies in the screen
    bool visible = true;
    if (pScreen.x < l || pScreen.x > r || pScreen.y < b || pScreen.y > t)
        visible = false;

    return visible;
}

// [comment]
// Code has been adapted from https://github.com/scratchapixel/scratchapixel-code/blob/main/3d-viewing-pinhole-camera/pinhole.cpp
// Allows user to easily create a render of the object from camera settings they specify. 
// [/comment]
void renderObject
(
    float fLength,              // Focal Length, mm
    float fAW,                  // Film Aperture Width, mm
    float fAH,                  // Film Aperture Height, mm
    float nCP,                  // Near Clipping Plane, m
    float fCP,                  // Far Clipping Plane, m
    Matrix44f cameraToWorld,    // Camera to World matrix, i.e. how has the camera been transformed
    std::string filename        // Output file name
)
{
    // Settings can be taken from Blender Camera to replicate
    float focalLength = fLength; 
    float filmApertureWidth = fAW; 
    float filmApertureHeight = fAH;
    float nearClippingPlane = nCP;
    float farClippingPlane = fCP; 

    Matrix44f worldToCamera = cameraToWorld.inverse();

    // Calculation of Canvas dimensions, based on camera settings.
    float top = (filmApertureHeight/2)/focalLength * nearClippingPlane;
    float bottom = -top;
    float right = (filmApertureWidth/2)/focalLength * nearClippingPlane; 
    float left = -right;

    // Final Image Dimensions
    uint32_t imageWidth = 512, imageHeight = 512;

    std::ofstream ofs;
    ofs.open(filename);
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << imageWidth << "\" height=\"" << imageHeight << "\">" << std::endl;
    for (size_t i{0}; i < triangles.size()/3; ++i)
    {
        // Grab 3 vertices that make up a triangle
        const Vec3f &v0World = vertices[triangles[i * 3]];
        const Vec3f &v1World = vertices[triangles[i * 3 + 1]];
        const Vec3f &v2World = vertices[triangles[i * 3 + 2]];
        Vec2i v0Raster, v1Raster, v2Raster;

        bool visible = true;

        // Visible only if all vertices making up that triangle are in the canvas frame, and assign raster coordinates 
        visible &= computeCoordinates(v0World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v0Raster);
        visible &= computeCoordinates(v1World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v1Raster);
        visible &= computeCoordinates(v2World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v2Raster);
        
        int val = visible ? 0 : 255; // Black if visible, red if not visible

        // Draw lines using svg format
        ofs << "<line x1=\"" << v0Raster.x << "\" y1=\"" << v0Raster.y << "\" x2=\"" << v1Raster.x << "\" y2=\"" << v1Raster.y << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n";
        ofs << "<line x1=\"" << v1Raster.x << "\" y1=\"" << v1Raster.y << "\" x2=\"" << v2Raster.x << "\" y2=\"" << v2Raster.y << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n";
        ofs << "<line x1=\"" << v2Raster.x << "\" y1=\"" << v2Raster.y << "\" x2=\"" << v0Raster.x << "\" y2=\"" << v0Raster.y << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n"; 
    }
    ofs << "</svg>\n";
    ofs.close();
}