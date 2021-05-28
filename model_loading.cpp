#define GLM_FORCE_RADIANS

//TODO: przerobić na klasę, usunąć niepotrzebne

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
//#include "mesh.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians

// the global Assimp scene object
GLuint tex;
std::vector<glm::vec4> verts;
std::vector<glm::vec4> norms;
std::vector<glm::vec2> texCoords;
std::vector<unsigned int> indices;

const float PI = 3.1415;


GLuint readTexture(const char* filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0); //choose a texture handler to edit

  //Read the file into computers memory
  std::vector<unsigned char> image;   //Allocate a vector for storing the image
  unsigned width, height;   //Variables which will contain the image size
  //Read the image
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import the image into graphics cards memory
  glGenTextures(1,&tex); //Initialize one handle
  glBindTexture(GL_TEXTURE_2D, tex); //Activate handle (bind it to the active texturing unit)
  //Import the image into the GC memory associated with the handle
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

void key_callback(GLFWwindow* window,	int key,	int scancode,	int action,	int mod){
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			speed_y = -PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = PI;
		}
		if (key == GLFW_KEY_UP) {
			speed_x = -PI;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = PI;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			speed_y = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			speed_x = 0;
		}
	}
}


void processMesh(const aiScene* scene, int i){
	aiMesh* mesh = scene->mMeshes[i];

	///przetwarzanie wierzchołków
	for (int i=0; i<mesh->mNumVertices; i++){
		aiVector3D vertex = mesh->mVertices[i];		//aiVector3D podobny do glm::vec3
		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];	//wertory znormalizowane
		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0)); //kierunek a nie pozycja, więc w=0

		//może być do 8 zestawów współrzędnych teksturowania.
		unsigned int uv_num = mesh->GetNumUVChannels();
		// std::cout<<"Liczba zestawów współrzędnych teksturowania: "<<uv_num<<std::endl;
		unsigned int uv_dim = mesh->mNumUVComponents[0]; //Ilość składowych wsp. teksturowania dla 0. zestawu
		// std::cout<<"Liczba współrzędnych: "<<uv_dim<<std::endl;

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		texCoords.push_back(glm::vec2(texCoord.x, texCoord.y)); //jeżeli tekstura ma tylko 2 wymiary
	}

	//przetwarzanie wielokątów składowych
	for (int i=0; i<mesh->mNumFaces; i++){
		aiFace& face = mesh->mFaces[i];	// face to 1 wielokąt siatki

		for (int j=0; j<face.mNumIndices; j++){ //zawsze 3, bo przerobiliśmy na trójkąty
			indices.push_back(face.mIndices[j]);
		}
	}

}


bool load_model(const std::string& filename){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
    aiProcess_Triangulate            |	//model będzie składał się tylko z trójkątów
		aiProcess_FlipUVs								 |	//odwrócenie współrzędnych - (0,0) będzie w lewym dolnym rogu
		aiProcess_GenSmoothNormals				 |  //Generuj uśrednione wektory normalne, jeśli ich nie ma
    aiProcess_JoinIdenticalVertices
		//   aiProcess_CalcTangentSpace       | //wylicz przestrzeń styczną, które tworzą przestrzeń ściany; przydatne dla bump shadingu
    // aiProcess_SortByPType
	);
	std::cout<<importer.GetErrorString()<<std::endl;  // If the import failed, report it

  // Now we can access the file's contents.
	if (scene->HasMeshes()){
		for (int i=0; i<scene->mNumMeshes;i++){
			processMesh(scene, i);
		}
	}

  // We're done. Everything will be cleaned up by the importer destructor
  return true;
}


void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(glm::vec3(0.0f,0.0f,-8.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
  glm::mat4 P=glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //compute projection matrix

	glm::mat4 M=glm::mat4(1.0f);
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f));

  spLambertTextured->use();  //activate shading program

	//Send parameters to graphics card
  glUniformMatrix4fv(spLambertTextured->u("P"),1,false,glm::value_ptr(P));
  glUniformMatrix4fv(spLambertTextured->u("V"),1,false,glm::value_ptr(V));
  glUniformMatrix4fv(spLambertTextured->u("M"),1,false,glm::value_ptr(M));

	glEnableVertexAttribArray(spLambertTextured->a("vertex")); //Enable sending data to the attribute vertex
  glVertexAttribPointer(spLambertTextured->a("vertex"),4,GL_FLOAT,false,0, verts.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
  glVertexAttribPointer(spLambertTextured->a("texCoord"),4,GL_FLOAT,false,0, texCoords.data());

	glEnableVertexAttribArray(spLambertTextured->a("normal"));
  glVertexAttribPointer(spLambertTextured->a("normal"),4,GL_FLOAT,false,0, norms.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(spLambertTextured->u("tex"), 0);


  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data()); //Draw the object

  glDisableVertexAttribArray(spLambertTextured->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(spLambertTextured->a("texCoord"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));

  glfwSwapBuffers(window); //Copy back buffer to front buffer
}

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0,0,1,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback);
	tex = readTexture("bricks.png");
	initShaders();

}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
}

GLFWwindow* create_window(){
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) { //Initialize GLFW library
    fprintf(stderr, "Can't initialize GLFW.\n");
    exit(EXIT_FAILURE);
  }

  window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);

  if (!window) //If no window is opened then close the program
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); //During vsync wait for the first refresh

  GLenum err;
  if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
    fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }

  initOpenGLProgram(window);

  return window;
}

int main(void){
	load_model("Sir_Wormie.obj");

	std::cout<<verts.size()<<" "<<norms.size()<<" "<<texCoords.size()<<" " <<indices.size()<<std::endl;

  GLFWwindow* window = create_window();

	float angle_x = 0;
	float angle_y = 0;

	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window))
	{
		angle_x+=speed_x*glfwGetTime();
		angle_y+=speed_y*glfwGetTime();
		glfwSetTime(0);

		drawScene(window,angle_x,angle_y);
		glfwPollEvents();
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
