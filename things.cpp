#include <ctime>
#include <stdexcept>
#include "things.hpp"

const glm::vec3 CAMERA_DEFAULT_POS = glm::vec3(2, 10, -15);
const int R = 2;  //odległość materializowania się pocisku
bool show_textures = 1;

////////////////////////////////////////////////////////////////////

Everything::Everything(glm::vec3 _pos = glm::vec3(0, 0, 0), float _angle_x = 0, float _angle_y = 0) {
    this->angle_x = _angle_x;
    this->angle_y = _angle_y;
    this->pos = _pos;
}

//////////////////////////////////////////////////

Drawable::Drawable(const std::string& model_filename, std::vector<const char*> texture_filenames, glm::vec3 _scale, bool whole=true){
    this->scale = _scale;
    model = Model(model_filename, whole);
    if (show_textures) model.readTextures(texture_filenames);
}


void Drawable::draw(GLFWwindow* window, glm::mat4 V) {
    model.draw(window, get_angle_x(), get_angle_y(), get_position(), V, scale);
}

glm::mat4 Drawable::calc_M_matrix(){
  glm::mat4 M=glm::mat4(1.0f);
  M=glm::translate(M, get_position());
  M=glm::scale(M, scale);
  M=glm::rotate(M, get_angle_y(), glm::vec3(1.0f,0.0f,0.0f));
  M=glm::rotate(M, get_angle_x(), glm::vec3(0.0f,1.0f,0.0f));
  return M;
}

////////////////////////////////////////////////////////
int Worm::count_worms = 1;


Worm::Worm(std::string name, Board* board, Camera* camera, const std::string& _model_filename, std::vector<const char*> tex_filenames) :
  Movable(),
  Drawable(_model_filename, tex_filenames,
  glm::vec3(0.5f,0.5f,0.5f), false),
  Everything(){
    this->name = name;
    this->life = 100;
    this->board = board;
    this->camera = camera;
    srand(time(NULL));
    int x = std::rand()*count_worms % (int)(board->get_x());
    int z = std::rand()*count_worms % (int)(board->get_z());
    set_position(glm::vec3(x, board->get_height(x, z), z));
    count_worms++;
}


void Worm::update(float speed, float angle_speed, double _time) {
    //przesunięcie w przestrzeni świata
    set_angle_x(get_angle_x() + angle_speed * _time);

    float x = get_position()[0] + speed * sin(get_angle_x()) * _time;
    float z = get_position()[2] + speed * cos(get_angle_x()) * _time;
    if (abs(x) < board->get_x() && abs(z) < board->get_z()){
      try {
          float y = board->get_height(x, z);
          set_position(glm::vec3(x, y, z));

          camera->update_pos(get_position(), get_angle_x());
      }
      catch (std::out_of_range) {}
    }

    // std::cout<<get_position()[0]<<" "<<get_position()[1]<<" "<<get_position()[2]<<std::endl;
}


void Worm::damage(int how_much) {
    life -= how_much;
    if (life <= 0)
      throw GameOverException(name);
    std::cout<< name <<" "<< life<<std::endl;
}

////////////////////////////////////////////////////////////////////

Bullet::Bullet(const std::string& obj_filename, std::vector<const char*> tex_filenames) :
  Movable(),
  Drawable(obj_filename, tex_filenames, glm::vec3(0.02f,0.02f,0.02f)),
  Everything() {
}

void Bullet::apply_gravity_and_wind(glm::vec3 _wind, float time) {
    glm::vec3 gravity = glm::vec3(0, -1.5, 0);
    set_position(get_position() + speed * glm::vec3(time, time, time));
    this->speed = speed + (gravity + _wind) * glm::vec3(time, time, time);
}

void Bullet::check_collision(Board* board, std::vector<Worm*> worms) {
    glm::vec3 pos = get_position();
    try {
        // collisions with the board
        if (pos.y <= board->get_height(pos.x, pos.z)) {
            this->speed = glm::vec3(0, 0, 0); // stop
            // TODO: wyświetl eksplozję

            //check if the worms were hurt
            for (int i = 0; i < 2; i++) {
                float dist = glm::distance(pos, worms[i]->get_position());
                if (dist < 5) // how far can it be hurt
                    worms[i]->damage(1 / dist * 50);
            }
        }
    }
    catch (std::out_of_range) {
        this->speed = speed * (-1.0f);
    }
    // hit the worm
    for (int i = 0; i < 2; i++) {
        float dist = glm::distance(pos, worms[i]->get_position());
        if (dist < R) {
            worms[i]->damage(1 / dist * 50);
            //TODO: eksplozja
            this->speed = glm::vec3(0, 0, 0);
        }
    }
}

void Bullet::shoot(glm::vec3 _pos, float _angle_x, float _angle_y) {
    _angle_y = -_angle_y;

    set_position(_pos + glm::vec3(0, 1, 0) + glm::vec3((R + 0.1) * sin(_angle_x) * cos(_angle_y), (R + 0.1) * sin(_angle_y), (R + 0.1) * cos(_angle_x) * cos(_angle_y)));
    set_angle_x(_angle_x);
    set_angle_y(_angle_y);
    float speed_val = 4;

    this->speed = glm::vec3(speed_val * sin(_angle_x) * cos(_angle_y), speed_val * sin(_angle_y), speed_val * cos(_angle_x) * cos(_angle_y));
}


////////////////////////////////////////////////////////////////////

Camera::Camera() : Movable(), Everything() {
    walking_mode = true;
    nose_vector = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::change_mode(Worm* active_worm) {
  //switch to different mode
    if (this->walking_mode == true) {
        //opcjonalnie zapisz poprzednie ustawienie kamery wzgl. worma |+deklaracja globalna pos_save/zwracanie
        //pos_save = pos - active_worm->get_position();
        set_position(active_worm->get_position());
    }   //zmieniamy na strzelanie
    else {
        //wróć do poprzedniego ustawienia
        //pos = active_worm->get_position() + pos_save;
        set_position(active_worm->get_position() + glm::vec3(2, 10, -15)); //TODO: domyślne ustawienie kamery
    }   //zmieniamy na chodzenie
    walking_mode = -walking_mode;
}

void Camera::update_pos(glm::vec3 worm_pos, float angle_x) {
    float distance = 10;   //odległość między kamerą a robakiem
    set_position(worm_pos + glm::vec3(-sin(angle_x) * distance, 7, -cos(angle_x) * distance));
    set_angle_x(angle_x);
}

void Camera::set_angle_y_restricted(float _angle_y) {
    if (abs(_angle_y) <= 0.6) {
        this->set_angle_y(_angle_y);
    }
}

////////////////////////////////////////////////////////////////////

Board::Board(const std::string& obj_filename, std::vector<const char*> tex_filenames):
  Drawable(obj_filename, tex_filenames, glm::vec3(50.0f,50.0f,50.0f)),
  Everything(glm::vec3(0, -26, 0)) {
    x = 29;
    z = 48;
}

float Board::get_height(float x, float z) {
    // glm::vec3 v1 = glm::vec3(0,0,0);  //TODO: pobrać najbliższe 3
    // glm::vec3 v2 = glm::vec3(0,0,0);
    // glm::vec3 v3 = glm::vec3(0,0,0);
    //
    // if(x > this->x || z > this->z || x < 0 || z < 0){
    //   throw std::out_of_range("Outside of the board");
    // }
    // float y = (-(x-v1.x)*(v2.y-v1.y)*(v3.z-v1.z)-(v2.x-v1.x)*(v3.y-v1.y)*(z-v1.z)+(z-v1.z)*(v2.y-v1.y)*(v3.x-v1.x)+(v2.z-v1.z)*(v3.y-v1.y)*(x-v1.x))/((v2.z-v1.z)*(v3.x-v1.x)-(v3.z-v1.z)*(v2.x-v1.x))+v1.y;
    // return y;
    return 0;
}
