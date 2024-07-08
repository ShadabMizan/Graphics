#include <fstream>
#include "geometry.h"
#include <vector>

class SceneObject
{
public:
    SceneObject(std::string name);
    void print();

private:
    std::string _name;
    std::vector<Vec3f> _vertices;
    std::ifstream _inFile;
};