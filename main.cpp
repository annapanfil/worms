#include "model.hpp"

float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians

void key_callback(GLFWwindow* window,	int key,	int scancode,	int action,	int mod){
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			speed_x = -PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_x = PI;
		}
		if (key == GLFW_KEY_UP) {
			speed_y = -PI;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_y = PI;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			speed_x = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			speed_y = 0;
		}
	}
}

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0,0,1,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback);
	initShaders();

}

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
  std::vector<const char*> filenames = {"bricks.png", "metal.png", "tiger.png"};
	Model worm("Sir_Wormie.obj");
  // worm.readTextures(filenames);

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

		worm.draw(window,angle_x,angle_y);
		glfwPollEvents();
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
