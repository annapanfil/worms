#include <ctime>
#include <stdexcept>
#include "things.hpp"

const glm::vec3 CAMERA_DEFAULT_POS = glm::vec3(2, 10, -15);
const int R = 2;  //odległość trafienia robaka pociskiem
bool show_textures = 1;

////////////////////////////////////////////////////////////////////

Everything::Everything(glm::vec3 _pos = glm::vec3(0, 0, 0), float _angle_x = 0, float _angle_y = 0) {
    this->angle_x = _angle_x;
    this->angle_y = _angle_y;
    this->pos = _pos;
}


Drawable::Drawable(const std::string& _model_filename, glm::vec3 _scale){
    this->scale = _scale;
    model = Model(_model_filename);
    if (show_textures) model.readTextures(filenames);
}


void Drawable::draw(GLFWwindow* window, glm::mat4 V) {
    model.draw(window, get_angle_x(), get_angle_y(), get_position(), V, scale);
}

////////////////////////////////////////////////////////
int Worm::count_worms = 1;


Worm::Worm(std::string name, Board* board, Camera* camera, const std::string& _model_filename) : Movable(), Drawable(_model_filename, glm::vec3(0.5f,0.5f,0.5f)), Everything(){
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
    try {
        float y = board->get_height(x, z);
        set_position(glm::vec3(x, y, z));

        camera->update_pos(get_position(), get_angle_x());
        std::cout<<get_position().x<<" "<<get_position().z<<std::endl;
    }
    catch (std::out_of_range) {}
}


void Worm::damage(int how_much) {
    life -= how_much;
}

////////////////////////////////////////////////////////////////////

Bullet::Bullet(const std::string& obj_filename) : Movable(), Drawable(obj_filename, glm::vec3(0.02f,0.02f,0.02f)), Everything() {
}

void Bullet::apply_gravity_and_wind(glm::vec3 _wind, float time) {
    glm::vec3 gravity = glm::vec3(0, -1.5, 0);
    set_position(get_position() + speed * glm::vec3(time, time, time));
    this->speed = speed + (gravity + _wind) * glm::vec3(time, time, time);
}

void Bullet::check_collision(Board* _board, std::vector<Worm*> _worms) {
    glm::vec3 _pos = get_position();
    try {
        if (_pos.y <= _board->get_height(_pos.x, _pos.z)) {
            this->speed = glm::vec3(0, 0, 0);
            //wyświetl eksplozję
        }
    }
    catch (std::out_of_range) {
        this->speed = speed * (-1.0f);
    }
    for (int i = 0; i < 2; i++) {
        float dist = glm::distance(_pos, _worms[i]->get_position());
        if (dist < R) {
            _worms[i]->damage(1 / dist * 15);
            //eksplozja
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

Board::Board(const std::string& obj_filename): Drawable(obj_filename, glm::vec3(50.0f,50.0f,50.0f)), Everything(glm::vec3(0, -26, 0)) {
    x = 30;
    z = 30;
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
