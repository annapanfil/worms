#include <ctime>

Worm::Worm(std::string name, Board* board){
  srand(time(NULL));
  int z = std::rand(board.get_z());
  int x = std::rand(board->get_x)
  this -> pos = (x, board->get_height(x,z), z)
  this -> name = name;
  this -> life = 100;
  this -> board = board;
  this -> angle_x = 0;
  this -> angle_y = nullptr;
}

Worm::update(float speed, float angle_speed, double _time){
  //przesunięcie w przestrzeni świata
  angle_x += angle_speed*_time;
  x = pos[0] + speed*cos(angle_x)*_time;
  z = pos[2] + speed*sin(angle_x)*_time;
  y = board->get_height(x,z);
  if (y != -1000000000){
    pos = glm::vec3(x, y, z);
  }
  camera.update_pos(glm::vec3(x+5, y+5, z+5););  //camera zainicjalizowana w mainie
  
}


Bullet::Bullet(glm::vec3 pos, float angle_x, float angle_y){
  this -> pos = pos;
  float speed_val = 10;
  this -> speed = vec3(speed_val*cos(angle_x)*cos(angle_y), speed_val*sin(angle_y), speed_val*sin(angle_x)*cos(angle_y));
  this -> angle_y = nullptr;
  this -> angle_x = nullptr;
}
////////////dodane
Camera::Camera(){
  walking_mode=true;
  this -> angle_x = 0;
  this -> angle_y = nullptr;
}

Camera::change_mode(){
  //switch to different mode
}

Camera::get_angle_x(){
  return this -> angle_x;
}

Camera::get_angle_y(){
  return this -> angle_y;
}
Camera::get_mode(){
  return this -> walking_mode;
}

Camera::update_pos(_pos){
    this -> pos = _pos;
}
///////////koniec

Board::Board(){

}








  void turn_right(float angle){
    //obrót całego układu współrzędnych robaka
  }
  void move_forward(float amount){
    //przeliczenie tego przesunięcia na przestrzeń świata
  }
};

class Worm: public Thing{

};

class Bazooka: public Thing{

};

class Bullet: public Thing{

};
