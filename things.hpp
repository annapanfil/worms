#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

//eksplozja

class Board{
private:
  glm::vec3 pos;
  float x;
  float z;
  float Vertices[];
  //grafika

public:
  float get_x(){return x;}
  float get_z(){return z;}
  float get_height(float x, float z); //jak poza planszą to coś tam
};


class Thing{
private:
  float angle_x; //current rotation angle of the object, x axis
  float angle_y; //current rotation angle of the object, y axis
  //graficzna reprezentacja
  //ewent. dorzucić macierz M
  //void move(float x, float y, float z);   //czy to jest potrzebne? mamay update
protected:
  glm::vec3 pos; //w przestrzeni świata //w bazooce to jest przesunięcie wzgl. robaka
public:
  Thing(float _angle_x, float angle_y, glm::vec3 pos);
  Thing(float _angle_x, Board* board);
  glm::vec3 get_position();
  void rotate(float angle, float time);
  void turn_right(float angle);
  void move_forward(float amount);
  float get_angle_x(){return angle_x;}
  float get_angle_y(){return angle_y;}
  void set_angle_x(float _angle_x){angle_x = _angle_x;}
  void set_angle_y(float _angle_y){angle_y = _angle_y;}
};


class Camera: public Thing{
private:
  bool walking_mode;
  // skopiowana od prowadzącego albo z internetu
  // potrzebujemy obrót i przybliżanie/oddalanie
public:
  Camera(glm::vec3 pos);
  void change_mode();
  void update_pos(glm::vec3 _pos);      //żeby kamera poruszała się razem z robaczkiem lub zmieniła ustawienie po mode_change
  bool get_mode(){return walking_mode;}        //potrzebne żeby kamera poruszała się razem z robaczkiem
};


class Worm: public Thing{
private:
  std::string name;
  int life; //if 0 then picture is grave
  Board* board;
  Camera* camera;

public:
  Worm(std::string name, Board* board, Camera* camera);
  void update(float speed, float angle_speed, double _time);
  void damage(int how_much);
};


class Bullet: public Thing{
private:
  glm::vec3 speed;
public:
  Bullet(glm::vec3 pos, float angle_x, float angle_y);
  void apply_gravity_and_wind(glm::vec3 wind, float time);  //na razie czas nie potrzebny
  bool check_collision(Board* board, std::vector<Worm*> worms);
  glm::vec3 get_speed() {return speed;}
  //void explosion();
};
