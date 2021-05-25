#pragma once

virtual class Thing{
private:
  glm::vec3 pos; //w przestrzeni świata //w bazooce to jest przesunięcie wzgl. robaka
  float angle_x; //current rotation angle of the object, x axis
  float angle_y; //current rotation angle of the object, y axis
  //graficzna reprezentacja
  //ewent. dorzucić macierz M
  void move(float x, float y, float z);   //czy to jest potrzebne? mamay update

public:
  glm::vec3 get_position();
  void rotate(float angle, float time)
  void turn_right(float angle);
  void move_forward(float amount);
};


class Worm: public Thing{
private:
  std::string name;
  int life; //if 0 then picture is grave
  Board* board;

public:
  Worm(std::string name, Board* board);
  void update(float speed, float angle_speed, double _time);
  void damage(int how_much);
};


class Bullet: public Thing{
private:
  glm::vec3 speed;
public:
  Bullet(glm::vec3 pos, float angle_x, float angle_y);
  void apply_gravity_and_wind(glm::vec3 wind, float time);
  bool check_collision(Board board, vector<Worm*> worms);
};


class Camera: public Thing{
private:
  bool walking_mode;
  // skopiowana od prowadzącego albo z internetu
  // potrzebujemy obrót i przybliżanie/oddalanie
public:
  Camera();
  void change_mode();
  void update_pos(glm::vec3 _pos);      //żeby kamera poruszała się razem z robaczkiem lub zmieniła ustawienie po mode_change
  bool get_mode();        //potrzebne żeby kamera poruszała się razem z robaczkiem
  float get_angle_x();
  float get_angle_y();
};


class Board{
private:
  glm::vec3 pos;
  float x;
  float z;
  float Vertices[];
  //grafika

public:
  float get_x();
  float get_z();
  float get_height(float x, float z); //jak poza planszą to coś tam
};
