#include "includes.hpp"

/*TODO:
- nakładki 2D:
    - wiatr:
        - obliczyć wektor względem wzroku robaczka
        - ewentualnie wskazwać strzałakami
    - celownik
*/


using std::cout;
using std::endl;

const int roundtime = 15;       //time fot walking
float speed = 0;                // current speed
float angle_speed = 0;          // current angle speed
float camera_angle_speed_x = 0; // current camera angle speed
float camera_angle_speed_y = 0;

const float ANGLE_SPEED = PI / 3; // max angle speed of the camera and worm
const float SPEED = 5;            // max speed of the worm

GLuint font_tex; // handle for a font texture

bool walking = true;  // current worm mode (walking or aiming)


// game window to be closed
class CloseWindowException : public std::exception {
public:
    CloseWindowException() {}
};

// OpenGL error handling
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

// arrows for moving or aiming, space for shooting or changing mode
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (walking == true) {
            if (key == GLFW_KEY_LEFT) { angle_speed = ANGLE_SPEED; }
            if (key == GLFW_KEY_RIGHT) { angle_speed = -ANGLE_SPEED; }
            if (key == GLFW_KEY_UP) { speed = SPEED; }
            if (key == GLFW_KEY_DOWN) { speed = -SPEED; }
        }
        else {
            if (key == GLFW_KEY_UP) { camera_angle_speed_y = -ANGLE_SPEED; }
            if (key == GLFW_KEY_DOWN) camera_angle_speed_y = ANGLE_SPEED;
            if (key == GLFW_KEY_RIGHT) camera_angle_speed_x = -ANGLE_SPEED;
            if (key == GLFW_KEY_LEFT) camera_angle_speed_x = ANGLE_SPEED;
        }
    }
    if (action == GLFW_RELEASE) {
        if (walking == true) {
            if (key == GLFW_KEY_LEFT) angle_speed = 0;
            if (key == GLFW_KEY_RIGHT) angle_speed = 0;
            if (key == GLFW_KEY_UP) speed = 0;
            if (key == GLFW_KEY_DOWN) speed = 0;
        }
        else {
            if (key == GLFW_KEY_LEFT) camera_angle_speed_x = 0;
            if (key == GLFW_KEY_RIGHT) camera_angle_speed_x = 0;
            if (key == GLFW_KEY_UP) { camera_angle_speed_y = 0; }
            if (key == GLFW_KEY_DOWN) camera_angle_speed_y = 0;
        }
        if (key == GLFW_KEY_SPACE) { walking = !walking; }
    }
}


// stop everything before next player movement
void stop_movement() {
    angle_speed = 0;
    speed = 0;
    camera_angle_speed_x = 0;
    camera_angle_speed_y = 0;
}

// read texture from png file
GLuint readTexture(const char* filename){
  GLuint tex;

  glActiveTexture(GL_TEXTURE0);

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

  std::cout<<"Texture "<<filename<<" read.\n";
  return tex;
}

void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0.2, 0.2, 0.9, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, keyCallback);
    initShaders();     // start shaders for worms, text and other objects
    font_tex = readTexture("textures/consolas.png"); //load font texture
}

void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
}


// calc camera direction while shooting
glm::vec3 calcDir(float angle_x, float angle_y) {
    glm::vec4 dir = glm::vec4(0, 0, 1, 0);
    glm::mat4 M = glm::rotate(glm::mat4(1.0f), angle_y, glm::vec3(0, 1, 0)); //vertical rotation
    M = glm::rotate(M, angle_x, glm::vec3(1, 0, 0));  //horizontal rotation
    dir = M * dir;
    return glm::vec3(dir);
}


// create flat square object to texture with text
void prepareTextSquares(std::string text, std::vector<glm::vec2>* vertices,
  std::vector<glm::vec2>* UVs, int x=30, int y=470, int size = 30)
  {
  // x, y – text position, size – text size

  int pixels = 16;                // pixels of letter square
  float pixels_f = (float)pixels; // in order not to convert back and forth
  float cell_fill = 0.5;          // how much space of the cell takes the font letter

  for (int i=0 ; i<text.length() ; i++){
    // vertices positions
    glm::vec2 vertex_up_left    = glm::vec2( x+i*size, y+size);
    glm::vec2 vertex_up_right   = glm::vec2( x+i*size+size, y+size);
    glm::vec2 vertex_down_right = glm::vec2( x+i*size+size, y);
    glm::vec2 vertex_down_left  = glm::vec2( x+i*size, y);

    vertices->push_back(vertex_up_left);
    vertices->push_back(vertex_down_left);
    vertices->push_back(vertex_up_right);

    vertices->push_back(vertex_down_right);
    vertices->push_back(vertex_up_right);
    vertices->push_back(vertex_down_left);

    // texture conrdinates positions
    char character = text[i];                   // we need ascii code
    float uv_x = (character%pixels)/pixels_f;   // column
    float uv_y = (character/pixels)/pixels_f;   // row

    glm::vec2 uv_up_left = glm::vec2(uv_x,uv_y);
    glm::vec2 uv_up_right = glm::vec2(uv_x + cell_fill/pixels_f, uv_y);
    glm::vec2 uv_down_right = glm::vec2(uv_x + cell_fill/pixels_f,uv_y + 1.0f/pixels_f);
    glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + 1.0f/pixels_f);

    UVs->push_back(uv_up_left);
    UVs->push_back(uv_down_left);
    UVs->push_back(uv_up_right);

    UVs->push_back(uv_down_right);
    UVs->push_back(uv_up_right);
    UVs->push_back(uv_down_left);
  }
}


// draw text in 2D
void drawText(std::string text, int x = 30, int y = 470, int size = 30){
  std::vector<glm::vec2> vertices;
  std::vector<glm::vec2> texCoords;
  prepareTextSquares(text, &vertices, &texCoords, x, y, size);

  sp_text->use(); // special shader

  // pass vertex and texture coordinates and the texture
  glEnableVertexAttribArray(sp_text->a("vertex"));
  glVertexAttribPointer(sp_text->a("vertex"),2,GL_FLOAT,false,0, vertices.data());

  glEnableVertexAttribArray(sp_text->a("texCoord"));
  glVertexAttribPointer(sp_text->a("texCoord"),2,GL_FLOAT,false,0, texCoords.data());

  glUniform1i(sp_text->u("font_tex"), 0);
  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_tex);

  glDrawArrays(GL_TRIANGLES, 0, vertices.size());

  glDisableVertexAttribArray(sp_text->a("vertex"));
	glDisableVertexAttribArray(sp_text->a("texCoord"));
	glDisableVertexAttribArray(sp_text->a("font_tex"));
}

// draw scene of walking worm
void drawSceneWalking(GLFWwindow* window, Camera* camera, std::vector<Drawable*> objects, Worm* active_worm, std::vector<Worm*> worms, glm::vec3 wind, float timer) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 observer = camera->get_position();
    glm::vec3 center = active_worm->get_position() + glm::vec3(0, 3, 0); 	// camera is following active worm
    glm::vec3 nose_vector = glm::vec3(0.0f, 1.0f, 0.0f); //(pionowo prostopadły do osi patrzenia)

    // Calculate view matrix
    glm::mat4 V = glm::lookAt(observer, center, nose_vector);

    // draw all objects
    for (int i = 0; i < objects.size(); i++) {
        objects[i]->draw(window, V);
    }

    // display text
    int win = floor(wind[0]);
    int win_ = floor(wind[1]);
    int win__ = floor(wind[2]);
    float backwards_timer = roundtime - timer;
    std::string text_view[4];
    text_view[0] = { " Time: " + std::to_string(backwards_timer) };
    text_view[1] = { " Worm1 (BLUE)life: " + std::to_string(worms[0]->get_life()) };
    text_view[2] = { " Worm2 (RED)life: " + std::to_string(worms[1]->get_life())};
    text_view[3] = { "LIFE:" };
    drawText(text_view[0], 10, 570, 25);
    drawText(text_view[1], 10, 525, 18);
    drawText(text_view[2], 10, 546, 18);
    glfwSwapBuffers(window);
}

// draw aiming scene, center defines the direction of the bullet
void drawSceneAiming(GLFWwindow* window, Camera* camera, std::vector<Drawable*> objects, Worm* active_worm, std::vector<Worm*> worms, glm::vec3 wind) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 observer = camera->get_position() + glm::vec3(0, 2, 0);
    glm::vec3 center = camera->get_position() + glm::vec3(0, 2, 0) + calcDir(camera->get_angle_y(), camera->get_angle_x()); 	//active_worm

    camera->nose_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 V = glm::lookAt(observer, center, camera->nose_vector); //calculate view matrix

    // draw all objects
    for (int i = 0; i < objects.size(); i++) {
        if ((Worm*)(objects[i]) != active_worm) {
            objects[i]->draw(window, V);
        }
    }

    // display 2D text
    int win = floor(wind[0]);
    int win_ = floor(wind[1]);
    int win__ = floor(wind[2]);
    std::string text_view[4];
    text_view[0] = "Wind: " + std::to_string(win) + "," + std::to_string(win_) + "," + std::to_string(win__);
    text_view[1] = { " Worm1 (BLUE) life:" + std::to_string(worms[0]->get_life()) };
    text_view[2] = { " Worm2 (RED) life:" + std::to_string(worms[1]->get_life()) };
    text_view[3] = { "LIFE:" };
    drawText(text_view[0], 10, 570, 25);
    drawText(text_view[1], 10, 525, 18);
    drawText(text_view[2], 10, 546, 18);

    glfwSwapBuffers(window);
}


// animation of the bullet
void drawSceneShooting(GLFWwindow* window, Camera* camera, std::vector<Drawable*> objects, Bullet* bullet, Worm* worm) {
    /* camera is following the bullet */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 worm_M = worm->calc_M_matrix();
    glm::vec3 cam_pos = glm::vec4(worm->get_position(),0) + worm_M * glm::vec4(30, 10, 35, 0); // camera position with respect to worm

    glm::vec3 observer = glm::vec3(cam_pos.x, cam_pos.y, cam_pos.z);
    glm::vec3 center = bullet->get_position(); 	//follow bullet
    camera->nose_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    //calculate view matrix, including angles
    glm::mat4 V = glm::lookAt(observer, center, camera->nose_vector);

    // draw objects
    for (int i = 0; i < objects.size(); i++) {
        objects[i]->draw(window, V);
    }
    bullet->draw(window, V);

    glfwSwapBuffers(window);

    sleep(1 / 24); //Linux
    // Sleep(1 / 24); //Windows
}


// brighten the screen
void draw_explosion(GLFWwindow* window) {
    glm::vec4 cl = glm::vec4(0.5, 0.3, 0.3, 1);
    glClearColor(cl[0], cl[1], cl[2], cl[3]);
    for (int i = 0; i < 5; i++) {
        cl += glm::vec4(0.1, 0.1, 0.1, 0);
        glClearColor(cl[0], cl[1], cl[2], cl[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
    for (int i = 0; i < 2; i++) {
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
    for (int i = 0; i < 5; i++) {
        cl -= glm::vec4(0.1, 0.1, 0.1, 0);
        glClearColor(cl[0], cl[1], cl[2], cl[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
    glClearColor(0.2, 0.2, 0.9, 1);
}


// the and and the result
void drawSceneEndOfGame(GLFWwindow* window, std::vector<Worm*> worms) {
    glm::vec4 cl = glm::vec4(0.5, 0.3, 0.3, 1);
    glClearColor(cl[0], cl[1], cl[2], cl[3]);

    int won;

    for (int i = 0; i < 2; i++) {
        if (worms[1 - i]->get_life() <= 0) {
            won = i + 1;
        }
    }

    std::string text_to_view = "Game over";
    std::string text_view = "Worm number " + std::to_string(won) + " won!";
    for (int i = 0; i < 100; i++) {
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawText(text_to_view, 150, 300, 60);
        drawText(text_view, 150, 270, 30);
        glfwSwapBuffers(window);
    }


    glClearColor(0.2, 0.2, 0.9, 1);

}


// init libraries and create window
GLFWwindow* create_window() {
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) { //Initialize GLFW library
        fprintf(stderr, "Can't initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(1000, 1000, "Worms", NULL, NULL);

    if (!window) //If no window is opened then close the program
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //During vsync wait for the first refresh

    GLenum err;
    if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
        fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);

    return window;
}



int main(int argc, char** argv)
{
    srand(time(NULL));

    //create objects
    GLFWwindow* window = create_window();

    Board board = Board(table_obj, table_textures);
    Camera camera;
    Worm worm1 = Worm("Napoleon", &board, &camera, worm_obj, worm_blue_textures);
    Worm worm2 = Worm("Che Guevara", &board, &camera, worm_obj, worm_red_textures);
    Bullet bullet = Bullet(bullet_obj, bullet_textures);
    Drawable obstacle_orange = Drawable(bullet_obj, bullet_textures, glm::vec3(0.2f,0.2f,0.2f), true);

    obstacle_orange.set_position(glm::vec3(0,2.4f,0));
    camera.update_pos(worm1.get_position(), 0);

    // group objects
    std::vector<Drawable*> objects = {&board, &worm1, &worm2, &obstacle_orange};
    std::vector<Worm*> worms = { &worm1, &worm2 };

    float angle_x, angle_y;

    glfwSetTime(0); //Zero the timer
    //Main loop
    try {
        while (!glfwWindowShouldClose(window))
        {
            glm::vec3 wind = glm::vec3((std::rand()%40)/10-2, (std::rand()%20)/10-1, (std::rand()%40)/10-2); //random direction of the wind

            for (int i = 0; i < 2; i++) {
                walking = true;
                Worm* active_worm = worms[i];
                clock_t start = clock();
                float timer;

                // player move
                while (((float)(clock() - start) / CLOCKS_PER_SEC <= roundtime) && walking == true) {
                    active_worm->update(speed, angle_speed, glfwGetTime());

                    glfwSetTime(0);

                    timer = ((float)(clock() - start) / CLOCKS_PER_SEC);
                    drawSceneWalking(window, &camera, objects, active_worm, worms, wind, timer);
                    glfwPollEvents();
                    if (glfwWindowShouldClose(window)) {
                        throw CloseWindowException();
                    }
                }

                // aiming
                walking = false;
                stop_movement();
                camera.change_mode(active_worm);
                while (walking == false) {
                    active_worm->update(0, camera_angle_speed_x, glfwGetTime());
                    //^rotate worm while aiming

                    //move camera
                    camera.set_angle_x(camera.get_angle_x() + camera_angle_speed_x * glfwGetTime());
                    camera.set_angle_y_restricted(camera.get_angle_y() + camera_angle_speed_y * glfwGetTime());

                    glfwSetTime(0);

                    drawSceneAiming(window, &camera, objects, active_worm, worms, wind);
                    glfwPollEvents();
                    if (glfwWindowShouldClose(window)) {
                        throw CloseWindowException();
                    }
                }


                //shoot
                bullet.shoot(active_worm->get_position(), camera.get_angle_x(), camera.get_angle_y());
                stop_movement();
                camera.change_mode(active_worm);
                glfwSetTime(0);
                while (bullet.get_speed() != glm::vec3(0, 0, 0)) {
                    bullet.apply_gravity_and_wind(wind, glfwGetTime()); // move bullet with respect to physics
                    bullet.check_collision(&board, worms); // check collisions with worms and ground

                    glfwSetTime(0);

                    camera.set_angle_x(camera_angle_speed_x * glfwGetTime());
                    camera.set_angle_y(camera_angle_speed_y * glfwGetTime());

                    drawSceneShooting(window, &camera, objects, &bullet, active_worm);

                    glfwPollEvents();
                    if (glfwWindowShouldClose(window)) {
                        throw CloseWindowException();
                    }
                }

                draw_explosion(window);
            }
        }
    }
    catch (CloseWindowException) {
      cout << "Goodbye!\n";
    }
    catch (GameOverException e) {
      cout << endl<< e.what() << " is dead. Game over.\n";
      drawSceneEndOfGame(window, worms);

      cout << "Goodbye!\n";
      sleep(60); //Linux
      // Sleep(60); //Windows
    }
    freeOpenGLProgram(window);

    glfwDestroyWindow(window); //Delete OpenGL context and the window.
    glfwTerminate(); //Free GLFW resources
    exit(EXIT_SUCCESS);
}
