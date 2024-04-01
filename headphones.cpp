#include "geometry.h"
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <vector>

bool readVerticesFile(std::string filename);

std::vector<Vec3f> vertices;
std::vector<int> triangles;

int main(int argc, char const *argv[])
{
    if (!readVerticesFile("headphones.txt")) 
    {
        return 1;
    }
    
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

