#include <ctime>
#include <stdexcept>


Thing::Thing(float _angle_x, float _angle_y, glm::vec3 _pos){
  angle_x = _angle_x;
  angle_y = _angle_y;
  pos = _pos;
}
Thing::Thing(float _angle_x){
  angle_x = _angle_x;
  angle_y = nullptr;
  srand(time(NULL));
  int z = std::rand(board.get_z());
  int x = std::rand(board->get_x);
  this -> pos = (x, board->get_height(x,z), z);

}

Worm::Worm(std::string name, Board* board): Thing(0){
  this -> name = name;
  this -> life = 100;
  this -> board = board;
}

Worm::update(float speed, float angle_speed, double _time){
  //przesunięcie w przestrzeni świata
  angle_x += angle_speed*_time;
  x = pos[0] + speed*cos(angle_x)*_time;
  z = pos[2] + speed*sin(angle_x)*_time;
  try{
    y = board->get_height(x,z);
    pos = glm::vec3(x, y, z);
    camera.update_pos(glm::vec3(x+1, y+4, z-5));  //camera zainicjalizowana w mainie
  }   
  catch(std::out_of_range){}        //nie wiemy czy nie potrzeba &zmiennej
}


Bullet::Bullet(glm::vec3 pos, float angle_x, float angle_y): Thing(angle_x, angle_y, pos){
  float speed_val = 10;
  this -> speed = vec3(speed_val*cos(angle_x)*cos(angle_y), speed_val*sin(angle_y), speed_val*sin(angle_x)*cos(angle_y));
}
Bullet::apply_gravity_and_wind(glm::vec3 _wind, float _time){
  glm::vec3 gravity = glm::vec3(0, -0.02, 0);
  this -> speed =  speed + gravity + _wind;
}

Bullet::check_collision(Board* _board, vector<Worm*> _worms){
  try{
    if(this->pos.y == _board -> get_height(this->pos.x, this->pos.z)){
      this->speed=glm::vec(0,0,0);  
      //wyświetl eksplozję
    }
  }
  catch(std::out_of_range){
    this->speed=speed*(-1);
  }
  float r = 2
  for(i=0, i<2, i++){
    glm::vec3 dist = glm::distance(this->pos, _worm[i]->pos );
    if(dist < r){
      _worm[i]->damage(1/dist*15);
      //eksplozja
      this->speed=glm::vec(0,0,0);  
    }
  }
}

Camera::Camera(glm::vec3 pos): Thing(0,0,pos){
  walking_mode = true;
}

Camera::change_mode(){    //trzeba dodać angles
  //switch to different mode
  if(this->walking_mode == true){
    //opcjonalnie zapisz poprzednie ustawienie kamery wzgl. worma |+deklaracja globalna pos_save/zwracanie
    //pos_save = pos - active_worm.get_position();
    this -> pos = active_worm.get_position();
  }   //zmieniamy na strzelanie
  else{
    //wróć do poprzedniego ustawienia
    //pos = active_worm.get_position() + pos_save;
    this -> pos = active_worm.get_position() + glm::vec3(1, 4, -5);
  }   //zmieniamy na chodzenie
  walking_mode = -walking_mode; 
}


Camera::update_pos(_pos){
    this -> pos = _pos;
}


Board::Board(){

}
Board::get_height(float x, float z){
  //vec1
  if(x > this->x || z > this->z || x < 0 || z < 0){
    throw std::out_of_range;
  }
  float y = (-(x-vec1.x)*(vec2.y-vec1.y)*(vec3.z-vec1.z)-(vec2.x-vec1.x)*(vec3.y-vec1.y)*(z-vec1.z)+(z-vec1.z)*(vec2.y-vec1.y)*(vec3.x-vec1.x)+(vec2.z-vec1.z)*(vec3.y-vec1.y)*(x-vec1.x))/((vec2.z-vec1.z)*(vec3.x-vec1.x)-(vec3.z-vec1.z)*(vec2.x-vec1.x))+vec1.y;
  return y;
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
