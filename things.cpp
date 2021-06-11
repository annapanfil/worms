#include <ctime>
#include <stdexcept>
#include "things.hpp"

const glm::vec3 CAMERA_DEFAULT_POS = glm::vec3(2, 10, -15);

////////////////////////////////////////////////////////////////////

Thing::Thing(float _angle_x, float _angle_y, glm::vec3 _pos){
  angle_x = _angle_x;
  angle_y = _angle_y;
  pos = _pos;
}

Thing::Thing(float _angle_x, Board* board){
  angle_x = _angle_x;
  angle_y = 0;  //nieużywane
  srand(time(NULL));
  // int x = std::rand()%(int)(board->get_x());
  // int z = std::rand()%(int)(board->get_z());
  // this -> pos = glm::vec3(x, board->get_height(x,z), z);
  this -> pos = glm::vec3(0,0,0);
}

////////////////////////////////////////////////////////////////////

Worm::Worm(std::string name, Board* board, Camera* camera, const std::string& obj_filename): Thing(0, board){
  this -> name = name;
  this -> life = 100;
  this -> board = board;
  this -> camera = camera;
  this -> model = Model(obj_filename);
  // model.readTextures(filenames); //TODO: odblokować tekstury
}

void Worm::draw(GLFWwindow* window, glm::mat4 V){
  model.draw(window, get_angle_x(), 0, pos, V, glm::vec3(0.5f, 0.5f, 0.5f));
}

void Worm::update(float speed, float angle_speed, double _time){
  //przesunięcie w przestrzeni świata
  set_angle_x(get_angle_x()+angle_speed*_time);

  float x = pos[0] + speed*sin(get_angle_x())*_time;
  float z = pos[2] + speed*cos(get_angle_x())*_time;
  try{
    float y = board->get_height(x,z);
    set_position(glm::vec3(x, y, z));
    // std::cout<<get_angle_x()<<" "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;

    camera->update_pos(get_position(), get_angle_x());
  }
  catch(std::out_of_range){}
}


void Worm::damage(int how_much){
  life -= how_much;
}

////////////////////////////////////////////////////////////////////

Bullet::Bullet(const std::string& obj_filename): Thing(0, 0, glm::vec3(0,0,0)){
  this -> model = Model(obj_filename);
  // model.readTextures(filenames);
}

void Bullet::apply_gravity_and_wind(glm::vec3 _wind, float _time){
  glm::vec3 gravity = glm::vec3(0, 0, 0);
  this -> speed =  speed + gravity + _wind;
}

bool Bullet::check_collision(Board* _board, std::vector<Worm*> _worms){
  try{
    if(this->pos.y == _board -> get_height(this->pos.x, this->pos.z)){
      this->speed=glm::vec3(0,0,0);
      //wyświetl eksplozję
    }
  }
  catch(std::out_of_range){
    this->speed=speed*(-1.0f);
  }
  float r = 2;
  for(int i=0; i<2; i++){
    float dist = glm::distance(this->get_position(), _worms[i]->get_position());
    if(dist < r){
      _worms[i]->damage(1/dist*15);
      //eksplozja
      this->speed=glm::vec3(0,0,0);
    }
  }
}

void Bullet::shoot(glm::vec3 _pos, float _angle_x, float _angle_y){
  set_position(_pos);
  set_angle_x(_angle_x);
  set_angle_y(_angle_y);
  float speed_val = 1;
  _angle_y = -_angle_y;
  // _angle_x = -_angle_x;

  this -> speed = glm::vec3(speed_val*sin(_angle_x)*cos(_angle_y), speed_val*sin(_angle_y), speed_val*cos(_angle_x)*cos(_angle_y));
}

void Bullet::draw(GLFWwindow* window, glm::mat4 V){
  model.draw(window, get_angle_x(), 0, pos, V, glm::vec3(0.02f, 0.02f, 0.02f));
}

void Bullet::update(double time){
  set_position(pos + speed*glm::vec3(time,time,time));
}

////////////////////////////////////////////////////////////////////

Camera::Camera(): Thing(0,0,glm::vec3(0,0,0)){
  walking_mode = true;
  nose_vector = glm::vec3(0.0f,1.0f,0.0f);
}

void Camera::change_mode(Worm* active_worm){    //trzeba dodać angles
  //switch to different mode
  if(this->walking_mode == true){
    //opcjonalnie zapisz poprzednie ustawienie kamery wzgl. worma |+deklaracja globalna pos_save/zwracanie
    //pos_save = pos - active_worm->get_position();
    this -> pos = active_worm->get_position();
  }   //zmieniamy na strzelanie
  else{
    //wróć do poprzedniego ustawienia
    //pos = active_worm->get_position() + pos_save;
    this -> pos = active_worm->get_position() + glm::vec3(2, 10, -15); //TODO: domyślne ustawienie kamery
  }   //zmieniamy na chodzenie
  walking_mode = -walking_mode;
}

void Camera::update_pos(glm::vec3 worm_pos, float angle_x){
  float distance = 10;   //odległość między kamerą a robakiem
  this->pos = worm_pos + glm::vec3(-sin(angle_x)*distance, 7, -cos(angle_x)*distance);
  set_angle_x(angle_x);
}

void Camera::set_angle_y_restricted(float _angle_y){
  if (abs(_angle_y) <= 0.6){
    this -> set_angle_y(_angle_y);
  }
}

////////////////////////////////////////////////////////////////////

Board::Board(){
  x=3;
  z=3;
  model = SimpleModel();
  pos = glm::vec3(0,0,0); //nic nie robi
}

float Board::get_height(float x, float z){
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

void Board::draw(GLFWwindow* window, glm::mat4 V){
  model.draw(window, V);
}
