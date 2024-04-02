#include "geometry.h"
#include <cstdlib>
#include <fstream>
#include <filesystem>
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


std::vector<Vec3f> vertices;
std::vector<int> triangles;

float cosf(float angle) { return (float)(cos(angle * std::numbers::pi/180)); }
float sinf(float angle) { return (float)(sin(angle * std::numbers::pi/180)); }

int main(int argc, char const *argv[])
{
    if (!readVerticesFile("headphones.txt")) 
    {
        return 1;
    }

    // Ex. 1
    // Camera is placed at [0.5, -9, 3.5], rotated 77d around x and 5d around z.
    float t1 = 77;
    float t2 = 5;
    Matrix44f cameraToWorld1 = 
    {
        cosf(t2), sinf(t2), 0, 0,
        cosf(t1)*-sinf(t2), cosf(t1)*cosf(t2), sinf(t1), 0,
        sinf(t1)*sinf(t2), -sinf(t1)*cosf(t2), cosf(t1), 0,
        0.5, -9, 3.5, 1
    };

    renderObject(50, 35, 24, 0.1, 100, cameraToWorld1, "./headphones1.svg");

    // Ex. 2
    Matrix44f cameraToWorld2 = getCameraToWorld(79, 14, 116, 7.3, 4, 3);
    renderObject(50, 35, 24, 0.1, 100, cameraToWorld2, "./headphones2.svg");

    // Ex.3 
    Matrix44f cameraToWorld3 = getCameraToWorld(67.2,0,-24,-3.3,-6,5);
    renderObject(17, 35, 24, 0.1, 100, cameraToWorld3, "./headphones3.svg");

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

// Taken from scratch-a-pixel source code
// https://github.com/scratchapixel/scratchapixel-code/blob/main/3d-viewing-pinhole-camera/pinhole.cpp

Matrix44f getCameraToWorld(float s1, float s2, float s3, float x, float y, float z)
{
    Matrix44f cameraToWorld = 
    {
        cosf(s2)*cosf(s3), cosf(s2)*sinf(s3), sinf(s2), 0,
        -sinf(s1)*sinf(s2)*cosf(s3) - cosf(s1)*sinf(s3), -sinf(s1)*sinf(s2)*sinf(s3) + cosf(s1)*cosf(s3), sinf(s1)*cosf(s2), 0,
        -cosf(s1)*sinf(s2)*cosf(s3) + sinf(s1)*sinf(s3), -cosf(s1)*sinf(s2)*sinf(s3) - sinf(s1)*cosf(s3), cosf(s1)*cosf(s2), 0,
        x, y, z, 1
    };

    return cameraToWorld;
}

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

void renderObject
(
    float fLength,              // Focal Length
    float fAW,                  // Film Aperture Width
    float fAH,                  // Film Aperture Height
    float nCP,                  // Near Clipping Plane
    float fCP,                  // Far Clipping Plane
    Matrix44f cameraToWorld,    // Camera to World matrix, i.e. how has the camera been transformed
    std::string filename        // Output file name
)
{
    // Settings taken from Blender render
    float focalLength = fLength; // 50mm
    float filmApertureWidth = fAW; // 35mm
    float filmApertureHeight = fAH; // 24mm
    float nearClippingPlane = nCP; // 0.1m
    float farClippingPlane = fCP; // 100m

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
       const Vec3f &v0World = vertices[triangles[i * 3]];
        const Vec3f &v1World = vertices[triangles[i * 3 + 1]];
        const Vec3f &v2World = vertices[triangles[i * 3 + 2]];
        Vec2i v0Raster, v1Raster, v2Raster;

        bool visible = true;
        visible &= computeCoordinates(v0World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v0Raster);
        visible &= computeCoordinates(v1World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v1Raster);
        visible &= computeCoordinates(v2World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v2Raster);
        
        int val = visible ? 0 : 255;
        ofs << "<line x1=\"" << v0Raster.x << "\" y1=\"" << v0Raster.y << "\" x2=\"" << v1Raster.x << "\" y2=\"" << v1Raster.y << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n";
        ofs << "<line x1=\"" << v1Raster.x << "\" y1=\"" << v1Raster.y << "\" x2=\"" << v2Raster.x << "\" y2=\"" << v2Raster.y << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n";
        ofs << "<line x1=\"" << v2Raster.x << "\" y1=\"" << v2Raster.y << "\" x2=\"" << v0Raster.x << "\" y2=\"" << v0Raster.y << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n"; 
    }
    ofs << "</svg>\n";
    ofs.close();
}