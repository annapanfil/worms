#define GLM_FORCE_RADIANS

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "shaderprogram.h"
#include "lodepng.h"
#include <vector>

const float PI = 3.1415;

class Mesh{
private:
  GLuint texture;
  std::vector<glm::vec4> verts;
  std::vector<glm::vec4> norms;
  std::vector<glm::vec2> texCoords;
  std::vector<unsigned int> indices;
public:
  Mesh(const aiScene* scene, int nr);
  void readTexture(const char* filename);
  void draw(GLFWwindow* window, glm::mat4 V, glm::mat4 P, glm::mat4 M);
};

class Model{
private:
  std::vector<Mesh> meshes;
  void load(const std::string& filename);
public:
  Model(){};
  Model(const std::string& obj_filename);
  void draw(GLFWwindow* window,float angle_x,float angle_y, glm::vec3 position, glm::mat4 V, glm::vec3 scale); //shader musi już istnieć
  void readTextures(std::vector<const char*> filenames);
};


//TODO: supermodel

class SimpleModel{
private:
  float verts [100]= {
    0,0,-4,1,     1,0.2,-4,1,   2,0.6,-4,1,    3,0.4,-4,1,	4,0,-4,1,
	0,0,-3,1,	  1,0,-3,1,     2,0.2,-3,1,    3,0,-3,1,    4,0,-3,1,	//
	0,0,-2,1,     1,0.2,-2,1,   2,0,-2,1,      3,0,-2,1,	4,0,-2,1,
	0,0.2,-1,1,	  1,0.5,-1,1,   2,0.3,-1,1,    3,0.2,-1,1,  4,0,-1,1,
	0,0,0,1,	  1,0.3,0,1,    2,0,0,1,       3,0,0,1,     4,0,0,1		//5 wierszy
	//0             0             0              0            0
};
  float normals[384] = {
    0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, 0,1,0,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,

	};
  float texCoords[384] = {
  1,0, 1,1, 0,0, //0,1,5
  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  //4 'górne'  0,1,5
  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  //4 'dolne'  5,1,6
  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  //4 'dolne'  10,6,11
  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  //4 'górne'  5,6,10
  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  //4 'dolne'  10,11,15
  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,	// dolne inaczej 15,16,11
  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  //4 'górne' 15,16,20
  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,	// dolne inaczej 20,21,16
	};
  unsigned int indexes[384]= {
	  0,1,5,  1,2,6,  2,3,7,  3,4,8,
	  5,1,6,  6,2,7,  7,3,8,  8,4,9,	//
	  10,6,11,  11,7,12,  12,8,13,  13,9,14,
	  5,6,10,  6,7,11,  7,8,12,  8,9,13,
	  10,11,15, 11,12,16, 12,13,17, 13,14,18,
	  15,16,11, 16,17,12, 17,18,13, 18,19,14,
	  15,16,20, 16,17,21, 17,18,22, 18,19,23,
	  20,21,16, 21,22,17, 22,23,18, 23,24,19
	};
  GLuint texture;
  int indexCount = 96;
  int vertexCount = 25;

public:
  SimpleModel();
  void draw(GLFWwindow* window, glm::mat4 V);
  void readTexture(const char* filename); //UWAGA! skopiowana z mesh
};
