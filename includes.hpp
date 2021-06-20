#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>    //linux
//#include <windows.h> //windows
#include "shaderprogram.h"
#include "things.hpp"

// objects filenames
std::string worm_obj = "objects/Sir_Wormie.obj";
std::string bullet_obj = "objects/Orange.fbx";
std::string table_obj = "objects/table.obj";

// textures filenames
std::vector<const char*> bullet_textures = {"textures/orange.png", "textures/orange.png", "textures/orange_normal.png"};
std::vector<const char*> worm_textures = {"textures/skin.png", "textures/fabric.png", "textures/fabric.png", "textures/metal.png"};
std::vector<const char*> table_textures = {"textures/table.png", "textures/table_reflect.png", "textures/table_normal.png"};
