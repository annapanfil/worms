#define GLM_FORCE_RADIANS

#include "includes.hpp"

float speed = 0;
float angle_speed = 0;
float camera_angle_speed_x = 0;
float camera_angle_speed_y = 0;

const float ANGLE_SPEED = PI/4;
const float SPEED = 1;

bool walking = true;



 void draw_coords(glm::mat4 V){
	std::vector<float> vertices = {10.0f, 0.0f,0.0f,   0.0f,0.0f,0.0f,  0.0f,10.0f,0.0f};


	glm::mat4 P=glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 M=glm::mat4(1.0f);
	spConstant->use();  //activate shading program

	//Send parameters to graphics card
	glUniformMatrix4fv(spConstant->u("P"),1,false,glm::value_ptr(P));
	glUniformMatrix4fv(spConstant->u("V"),1,false,glm::value_ptr(V));
	glUniformMatrix4fv(spConstant->u("M"),1,false,glm::value_ptr(M));
	glUniform4f(spConstant->u("color"),1,0,0,0);

	glEnableVertexAttribArray(spConstant->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(spConstant->a("vertex"),4,GL_FLOAT,false,0, vertices.data()); //Specify source of the data for the attribute vertex

	glDrawArrays(GL_LINES, 0, vertices.size()); //Draw the object

	glDisableVertexAttribArray(spConstant->a("vertex")); //Disable sending data to the attribute vertex
}



void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
  if (action==GLFW_PRESS) {
				//worm
		if (key==GLFW_KEY_LEFT) {std::cout<<"lewo\n"; angle_speed = ANGLE_SPEED;}
	  if (key==GLFW_KEY_RIGHT) {std::cout<<"prawo\n"; angle_speed = -ANGLE_SPEED;}
	  if (key==GLFW_KEY_UP) {std::cout<<"góra\n"; speed = SPEED;}
	  if (key==GLFW_KEY_DOWN) {std::cout<<"dół\n"; speed = -SPEED;}
					//camera
		if (key==GLFW_KEY_W) camera_angle_speed_y = ANGLE_SPEED;
		if (key==GLFW_KEY_S) camera_angle_speed_y = -ANGLE_SPEED;
		if (key==GLFW_KEY_D) camera_angle_speed_x = ANGLE_SPEED;
		if (key==GLFW_KEY_A) camera_angle_speed_x = -ANGLE_SPEED;
  }
  if (action==GLFW_RELEASE) {
      if (key==GLFW_KEY_LEFT) angle_speed = 0;
      if (key==GLFW_KEY_RIGHT) angle_speed = 0;
      if (key==GLFW_KEY_UP) speed = 0;
      if (key==GLFW_KEY_DOWN) speed = 0;

			if (key==GLFW_KEY_SPACE) walking = !walking;
    	if (key==GLFW_KEY_A) camera_angle_speed_x = 0;
			if (key==GLFW_KEY_D) camera_angle_speed_x = 0;
			if (key==GLFW_KEY_W) camera_angle_speed_y = 0;
			if (key==GLFW_KEY_S) camera_angle_speed_y = 0;
    }
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0.2,0.2,0.9,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window,keyCallback);
	initShaders();
}

void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
}



glm::vec3 calcDir(float kat_x, float kat_y) {		//do kamery podczas strzelania
	glm::vec4 dir = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::rotate(glm::mat4(1.0f), kat_y, glm::vec3(0, 1, 0)); //obrót w pionie
	M = glm::rotate(M, kat_x, glm::vec3(1, 0, 0));  //obrót w poziomie
	dir = M * dir;
	return glm::vec3(dir);
}  //podajemy kąty

void drawSceneWalking(GLFWwindow* window, Camera* camera, std::vector<Everything*> objects, Worm* active_worm) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glUniform4f(sp->u("light_position"), 0,0,0,1); // light position
  // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position

  glm::vec3 observer = camera->get_position(); //+calcDir(camera->get_angle_x(), camera->get_angle_y())-glm::vec3(0,0,-10);

  glm::vec3 center = active_worm->get_position()+glm::vec3(0,3,0);

  glm::vec3 nose_vector = glm::vec3(0.0f, 1.0f, 0.0f); //(pionowo prostopadły do osi patrzenia)

	//liczy macierz widoku uwzgędniając kąty									//active_worm
	glm::mat4 V = glm::lookAt(observer, center, nose_vector);
    //Wylicz macierz widoku
	// glm::mat4 V = glm::lookAt(camera->get_position()+calcDir(camera->get_angle_x(), camera->get_angle_y())-glm::vec3(0,0,-10), active_worm->get_position()+glm::vec3(1,6,0), glm::vec3(0.0f, 1.0f, 0.0f));


	for (int i=0; i<objects.size();i++){
		objects[i]->draw(window, V);
	}

	draw_coords(V);

  glfwSwapBuffers(window);
}

void drawSceneShooting(GLFWwindow* window, Camera* camera){
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform4f(sp->u("light_position"), 0,0,0,1); // light position
  glUniform4f(sp->u("light_position"), 0,0,0,1); // light position

	//liczy macierz widoku uwzgędniając kąty
	glm::mat4 V = glm::lookAt(camera.get_position(), camera.get_position()+calcDir(camera->get_angle_x(), camera->get_angle_y()), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
	//						  observer   , center							 	, noseVector (pionowo prostopadły do osi patrzenia)
  glm::mat4 P=glm::perspective(50.0f*PI/180.0f, 1.0f, 1.0f, 50.0f);

  sp->use();//activate shading program
  glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
  glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
*/
}


void drawSceneExplosion(){
}


GLFWwindow* create_window(){
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) { //Initialize GLFW library
    fprintf(stderr, "Can't initialize GLFW.\n");
    exit(EXIT_FAILURE);
  }

  window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);

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


int main(void)
{
	srand(time(NULL));
  GLFWwindow* window = create_window();

	Board board = Board(); //TODO: utworzyć planszę
	Camera camera;

	Worm worm1 = Worm("Napoleon", &board, &camera, "Sir_Wormie.obj");
	// Worm worm2 = Worm("Che Guevara", &board, &camera, "Sir_Wormie.obj");

	camera.update_pos(worm1.get_position(), 0);
	std::vector<Everything*> objects = {&board, &worm1}; //, &worm2
	std::vector<Worm*> worms = {&worm1}; //, &worm2

	glm::vec3 wind = glm::vec3((std::rand()%21)/10-10, (std::rand()%7)/10-3, (std::rand()%21)/10-10);
	float angle_x, angle_y;

	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window))
	{
		// std::cout<<"0 ";
		for(int i=0; i<1; i++){			//TODO: zmienić na 2
			Worm* active_worm = worms[i];
			clock_t start = clock();
			//ruch gracza
			while(((float)(clock() - start)/CLOCKS_PER_SEC <= 3) && walking == true){
				// std::cout<<(float)(clock() - start)/CLOCKS_PER_SEC<<std::endl;
				active_worm->update(speed, angle_speed, glfwGetTime());
				camera.set_angle_x(camera_angle_speed_x * glfwGetTime());
				camera.set_angle_y(camera_angle_speed_y * glfwGetTime());

				glfwSetTime(0);
				// std::cout<<"po robaczku\n";
				// std::cout<<"przed rysowaniem\n";
				drawSceneWalking(window, &camera, objects, active_worm);

				// char x;
				// std::cin>>x;

				glfwPollEvents();
			}

			/*//strzał
			while(walking=false){
				glfwSetTime(0);

				camera.set_angle_x(camera_angle_speed_x * glfwGetTime());
				camera.set_angle_y(camera_angle_speed_y * glfwGetTime());

				drawSceneShooting(window, &camera);
				glfwPollEvents();
			}

			Bullet bullet = Bullet(active_worm->get_position(), camera.get_angle_x(), camera.get_angle_y());

			camera.change_mode(active_worm);
			while(bullet.get_speed() != glm::vec3(0,0,0)){
				bullet.apply_gravity_and_wind(wind, glfwGetTime());
				bullet.check_collision(board, worms);

				glfwSetTime(0);

				camera.set_angle_x(camera_angle_speed_x * glfwGetTime());
				camera.set_angle_y(camera_angle_speed_y * glfwGetTime());

				drawSceneShooting(window, &camera);
				glfwPollEvents();
			}

			drawSceneExplosion();*/

		}
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
