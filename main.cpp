

#include "includes.hpp"
//dodaj do includes.h
//#include <iostream>
//#include <fstream>
//#include <stdlib.h>
//#include <glut.h>


/*TODO:
- draw jako metoda w klasie thing, nie w worm i bullet D  
- obracanie robaka przy celowaniu D  DONE
- wychodzenie za stół A
- naprawa tekstur A
- nakładki 2D:
    - wiatr
    - celownik
    - życie  + (kiedy życie się skończy zakończ grę)
- przeszkody
- oświetlenie
- Bullet::check_collision must return value bool -> void
- chodzenie/skręcanie jeśli nie przestanie się chodzić w czasie ruchu  DONE stop_movement()
- draw_explosion() 
*/


using std::cout;
using std::endl;

float speed = 0;
float angle_speed = 0;
float camera_angle_speed_x = 0;
float camera_angle_speed_y = 0;

const float ANGLE_SPEED = PI / 4;
const float SPEED = 3;

bool walking = true;


class CloseWindowException : public std::exception {
public:
    CloseWindowException() {}
};
/*
void renderbitmap(float x, float y, void* font, char* string) {
    char* c;
    glrasterpos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutbitmapcharacter(font, *c);
    }
}

void introscreen() {
    glcolor3f(1.f, 1.f, 1.f);
    char buf[100] = { 0 };
    sprintf_s(buf, "worm 1 life:");
    renderbitmap(-80, 40, glut_bitmap_times_new_roman_24, buf);
    sprintf_s(buf, ":::::::::::::::");
    renderbitmap(-80, 35, glut_bitmap_times_new_roman_24, buf);
} */

void draw_coords(glm::mat4 V) {
    std::vector<float> vertices = { 10.0f, 0.0f,0.0f,   0.0f,0.0f,0.0f,  0.0f,10.0f,0.0f };


    glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
    glm::mat4 M = glm::mat4(1.0f);
    spConstant->use();  //activate shading program

    //Send parameters to graphics card
    glUniformMatrix4fv(spConstant->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spConstant->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(spConstant->u("M"), 1, false, glm::value_ptr(M));
    glUniform4f(spConstant->u("color"), 1, 0, 0, 0);

    glEnableVertexAttribArray(spConstant->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(spConstant->a("vertex"), 4, GL_FLOAT, false, 0, vertices.data()); //Specify source of the data for the attribute vertex

    glDrawArrays(GL_LINES, 0, vertices.size()); //Draw the object

    glDisableVertexAttribArray(spConstant->a("vertex")); //Disable sending data to the attribute vertex
}



void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

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

void stop_movement() {
    angle_speed = 0;
    speed = 0;
    camera_angle_speed_x = 0;
    camera_angle_speed_y = 0;
}

void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0.2, 0.2, 0.9, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, keyCallback);
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

void drawSceneWalking(GLFWwindow* window, Camera* camera, std::vector<Drawable*> objects, Worm* active_worm) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position
  // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position

    glm::vec3 observer = camera->get_position();
    glm::vec3 center = active_worm->get_position() + glm::vec3(0, 3, 0); 	//active_worm
    glm::vec3 nose_vector = glm::vec3(0.0f, 1.0f, 0.0f); //(pionowo prostopadły do osi patrzenia)

      //liczy macierz widoku uwzgędniając kąty
    glm::mat4 V = glm::lookAt(observer, center, nose_vector);  //Wylicz macierz widoku

    for (int i = 0; i < objects.size(); i++) {
        objects[i]->draw(window, V);
    }

    glfwSwapBuffers(window);
}

void drawSceneAiming(GLFWwindow* window, Camera* camera, std::vector<Drawable*> objects, Worm* active_worm) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position
  // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position

    glm::vec3 observer = camera->get_position() + glm::vec3(0, 2, 0);
    glm::vec3 center = camera->get_position() + glm::vec3(0, 2, 0) + calcDir(camera->get_angle_y(), camera->get_angle_x()); 	//active_worm

    camera->nose_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    //liczy macierz widoku uwzgędniając kąty
    glm::mat4 V = glm::lookAt(observer, center, camera->nose_vector); //Wylicz macierz widoku

    for (int i = 0; i < objects.size(); i++) {
        if ((Worm*)(objects[i]) != active_worm) {
            objects[i]->draw(window, V);
        }
    }

    glfwSwapBuffers(window);
}


void drawSceneShooting(GLFWwindow* window, Camera* camera, std::vector<Drawable*> objects, Bullet* bullet) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position
  // glUniform4f(sp->u("light_position"), 0,0,0,1); // light position

    glm::vec3 observer = glm::vec3(-15, 3, -5);    //camera position
    glm::vec3 center = bullet->get_position(); 	//follow bullet

    camera->nose_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    //liczy macierz widoku uwzgędniając kąty
    glm::mat4 V = glm::lookAt(observer, center, camera->nose_vector); //Wylicz macierz widoku

    for (int i = 0; i < objects.size(); i++) {
        objects[i]->draw(window, V);
    }
    bullet->draw(window, V);

    glfwSwapBuffers(window);

    Sleep(1 / 24);
}


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
    glClearColor(0.2, 0.2, 0.9, 1);
    //drawSceneShooting(window, &camera, objects, &bullet);

}

void drawSceneExplosion() {
}


GLFWwindow* create_window() {
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
    if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
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

    Board board = Board("objects/table.obj");
    Camera camera;

    Worm worm1 = Worm("Napoleon", &board, &camera, "objects/Sir_Wormie.obj");
    Worm worm2 = Worm("Che Guevara", &board, &camera, "objects/Sir_Wormie.obj");
    Bullet bullet = Bullet("objects/Orange.fbx");


    camera.update_pos(worm1.get_position(), 0);
    std::vector<Drawable*> objects = { &board, &worm1, &worm2 };
    std::vector<Worm*> worms = { &worm1, &worm2 };

    // glm::vec3 wind = glm::vec3((std::rand()%21)/10-10, (std::rand()%7)/10-3, (std::rand()%21)/10-10);
    glm::vec3 wind = glm::vec3(0, 0, 0);
    float angle_x, angle_y;

    glfwSetTime(0); //Zero the timer
    //Main application loop
    try {
        while (!glfwWindowShouldClose(window))
        {
            for (int i = 0; i < 2; i++) {
                walking = true;
                Worm* active_worm = worms[i];
                clock_t start = clock();
                //ruch gracza
                while (((float)(clock() - start) / CLOCKS_PER_SEC <= 3) && walking == true) {
                    active_worm->update(speed, angle_speed, glfwGetTime());

                    glfwSetTime(0);

                    drawSceneWalking(window, &camera, objects, active_worm);
                    glfwPollEvents();
                    if (glfwWindowShouldClose(window)) {
                        throw CloseWindowException();
                    }
                }

                //namierzanie
                walking = false;
                stop_movement();
                camera.change_mode(active_worm);
                while (walking == false) {
                    active_worm->update(0, camera_angle_speed_x, glfwGetTime());
                    //^obracanie robaczka podczas celowania
                    camera.set_angle_x(camera.get_angle_x() + camera_angle_speed_x * glfwGetTime());
                    camera.set_angle_y_restricted(camera.get_angle_y() + camera_angle_speed_y * glfwGetTime());

                    glfwSetTime(0);

                    drawSceneAiming(window, &camera, objects, active_worm);
                    glfwPollEvents();
                    if (glfwWindowShouldClose(window)) {
                        throw CloseWindowException();
                    }
                }
                //strzał

                bullet.shoot(active_worm->get_position(), camera.get_angle_x(), camera.get_angle_y());
                stop_movement();
                camera.change_mode(active_worm);
                glfwSetTime(0);
                while (bullet.get_speed() != glm::vec3(0, 0, 0)) {
                    bullet.apply_gravity_and_wind(wind, glfwGetTime());
                    bullet.check_collision(&board, worms);

                    glfwSetTime(0);

                    camera.set_angle_x(camera_angle_speed_x * glfwGetTime());
                    camera.set_angle_y(camera_angle_speed_y * glfwGetTime());

                    drawSceneShooting(window, &camera, objects, &bullet);

                    glfwPollEvents();
                    if (glfwWindowShouldClose(window)) {
                        throw CloseWindowException();
                    }
                }

                drawSceneExplosion();
                draw_explosion(window);

            }
        }
    }
    catch (CloseWindowException) {
        cout << "Goodbye!\n";
    }
    freeOpenGLProgram(window);

    glfwDestroyWindow(window); //Delete OpenGL context and the window.
    glfwTerminate(); //Free GLFW resources
    exit(EXIT_SUCCESS);
}
