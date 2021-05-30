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
  Model(const std::string& obj_filename);
  void draw(GLFWwindow* window,float angle_x,float angle_y); //shader musi już istnieć
  void readTextures(std::vector<const char*> filenames);

};
