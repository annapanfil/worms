#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "model.hpp"


class Everything{
public:
  virtual void draw(GLFWwindow* window, glm::mat4 V) = 0;
};

class Worm;
//eksplozja

class Board: public Everything{
private:
  glm::vec3 pos;  //na razie nieużywana
  float x;
  float z;
  SimpleModel model;

public:
  Board();
  float get_x(){return x;}
  float get_z(){return z;}
  float get_height(float x, float z); //TODO
  void draw(GLFWwindow* window, glm::mat4 V);
};


class Thing: public Everything{
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
  glm::vec3 get_position() {return pos;}
  void set_position(glm::vec3 _pos) {pos = _pos;}
  void rotate(float angle, float time);
  void turn_right(float angle);
  void move_forward(float amount);
  float get_angle_x(){return angle_x;}
  float get_angle_y(){return angle_y;}
  void set_angle_x(float _angle_x){angle_x = _angle_x;}
  void set_angle_y(float _angle_y){angle_y = _angle_y;}
  virtual void draw(GLFWwindow* window, glm::mat4 V) {std::cout<<"UWAGA rysujesz nierysowalny obiekt\n";}
};


class Camera: public Thing{
private:
  bool walking_mode;

  // skopiowana od prowadzącego albo z internetu
  // potrzebujemy obrót i przybliżanie/oddalanie
public:
  glm::vec3 nose_vector;
  Camera();
  void change_mode(Worm* active_worm);
  void update_pos(glm::vec3 _pos, float delta_angle_x);      //żeby kamera poruszała się razem z robaczkiem lub zmieniła ustawienie po mode_change
  bool get_mode(){return walking_mode;}        //potrzebne żeby kamera poruszała się razem z robaczkiem
  void set_angle_y_restricted(float _angle_y);
};


class Worm: public Thing{
private:
  std::string name;
  int life; //if 0 then picture is grave
  Board* board;
  Camera* camera;
  std::vector<const char*> filenames = {"textures/skin.png", "textures/fabric.png", "textures/fabric.png", "textures/metal.png"};
  Model model;

public:
  Worm(std::string name, Board* board, Camera* camera, const std::string& obj_filename);
  void update(float speed, float angle_speed, double _time);
  void damage(int how_much);
  void draw(GLFWwindow* window, glm::mat4 V);
};


class Bullet: public Thing{
private:
  glm::vec3 speed;
  Model model;
  std::vector<const char*> filenames = {"textures/orange.png"};
public:
  Bullet(const std::string& obj_filename);
  void apply_gravity_and_wind(glm::vec3 wind, float time);  //na razie czas nie potrzebny
  bool check_collision(Board* board, std::vector<Worm*> worms);
  glm::vec3 get_speed() {return speed;}
  void shoot(glm::vec3 pos, float angle_x, float angle_y);
  void update(double time);
  void draw(GLFWwindow* window, glm::mat4 V);

  //void explosion();
};
