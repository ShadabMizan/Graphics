// In the accompanying image (cube.png), the x-axis is horizontal and y-axis is vertical, and the z-axis is out of the screen.
// The black cube represents the original coordinates of the points with edges drawn, whereas the green cube is rotated around the x-axis by 15 degrees.
// The Camera lies at the origin

#include <iostream>
#include <vector>
#include <cmath>
#include <numbers>

typedef struct  
{
    float x;
    float y;
    float z;
} Point_t;

// Screen Dimensions
int screenWidth = 512, screenHeight = 512;

// Distance the canvas is from the apex of the viewing frustrum (where the camera is)
float canvasDist = 1;

void applyTransformR3(std::vector<std::vector<float>> matrix, Point_t& point);
void printCoords(std::vector<Point_t> matrix);


int main(int argc, char const *argv[])
{
    // // A 2x2x2 Cube
    std::vector<Point_t> corners
    {
         { 1, -1, -5},
         { 1, -1, -3},
         { 1,  1, -5},
         { 1,  1, -3},
         {-1, -1, -5},
         {-1, -1, -3},
         {-1,  1, -5},
         {-1,  1, -3}
    };

    // A 2x2x2 Cube much further away.
    // std::vector<Point_t> corners
    // {
    //      { 1, -1, -100},
    //      { 1, -1, -98},
    //      { 1,  1, -100},
    //      { 1,  1, -98},
    //      {-1, -1, -100},
    //      {-1, -1, -98},
    //      {-1,  1, -100},
    //      {-1,  1, -98}
    // };

    float theta = std::numbers::pi/12;

    // Matrix for rotation around x-axis by pi/12 radians
    std::vector<std::vector<float>> rot_x = 
    {   
        {1, 0, 0},
        {0, (float)cos(theta), (float)sin(theta)},
        {0, -(float)sin(theta), (float)cos(theta)}
    };
    
    // Original
    std::cout << "Original Coords" << std::endl;
    printCoords(corners);

    // Transformation
    for (Point_t& corner : corners)
    {
        applyTransformR3(rot_x, corner);
    }
    std::cout << "\nRotation by pi/12 around x-axis" << std::endl;
    printCoords(corners);

    return 0;
}

void applyTransformR3(std::vector<std::vector<float>> matrix, Point_t& point)
{
    std::vector<float> vector = {point.x, point.y, point.z};
    int rows = matrix.size();
    int cols = matrix[0].size();

    std::vector<float> result(rows);

    for (int i = 0; i < rows; ++i) 
    {
        for (int j = 0; j < cols; ++j) 
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    point = {result[0], result[1], result[2]};    
}

void printCoords(std::vector<Point_t> matrix)
{
    for (int i{0}; Point_t point : matrix)
    {
        // Outputs the x and y projections onto a canvas [-1, 1] (can be out of bounds!)
        float x_proj = (point.x / -point.z) * canvasDist;
        float y_proj = (point.y / -point.z) * canvasDist;

        // Normalize to [0, 1] and then to the screen's dimensions.
        x_proj = screenWidth * (x_proj + 1) / 2;
        y_proj = screenHeight * (y_proj + 1) / 2;

        printf("%d.   x: %f y: %f \n", ++i, x_proj, y_proj);
    }
}