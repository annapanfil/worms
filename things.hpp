#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "model.hpp"

class Everything {
private:
	glm::vec3 pos;
  float angle_x; //current rotation angle of the object, x axis
  float angle_y; //current rotation angle of the object, y axis
public:
	Everything(glm::vec3 _pos, float _angle_x, float _angle_y);
	void set_position(glm::vec3 _pos) { pos = _pos; }
	glm::vec3 get_position() { return pos; }
  float get_angle_x() { return angle_x; }
  float get_angle_y() { return angle_y; }
  void set_angle_x(float _angle_x) { angle_x = _angle_x; }
  void set_angle_y(float _angle_y) { angle_y = _angle_y; }
};

class Drawable: virtual public Everything {
private:
	Model model;
	std::vector<const char*> filenames = { "textures/orange.png" };
  glm::vec3 scale;
public:
	Drawable(const std::string& _model_filename, glm::vec3 _scale);
	void draw(GLFWwindow* window, glm::mat4 V); // , glm::vec3 scale);
};


class Movable: virtual public Everything {
public:
	void rotate(float angle, float time);
	void turn_right(float angle);
	void move_forward(float amount);
};

class Worm;
//eksplozja

class Board : public Drawable {
private:
	float x; //rozmiar
	float z; //size
	//std::vector<const char*> filenames = { "textures/bricks.png" };

public:
	Board(const std::string& obj_filename);
	float get_x() { return x; }
	float get_z() { return z; }
	float get_height(float x, float z);
};



class Camera : public Movable {
private:
	bool walking_mode;

	// skopiowana od prowadzącego albo z internetu
	// potrzebujemy obrót i przybliżanie/oddalanie
public:
	glm::vec3 nose_vector;
	Camera();
	void change_mode(Worm* active_worm);
	void update_pos(glm::vec3 _pos, float delta_angle_x);      //żeby kamera poruszała się razem z robaczkiem lub zmieniła ustawienie po mode_change
	bool get_mode() { return walking_mode; }        //potrzebne żeby kamera poruszała się razem z robaczkiem
	void set_angle_y_restricted(float _angle_y);
};


class Worm : public Movable, public Drawable{
private:
	std::string name;
	int life; //if 0 then picture is grave
	Board* board;
	Camera* camera;
	//std::vector<const char*> filenames = { "textures/skin.png", "textures/fabric.png", "textures/fabric.png", "textures/metal.png" };

public:
	Worm(std::string name, Board* board, Camera* camera, const std::string& obj_filename);
	void update(float speed, float angle_speed, double _time);
	void damage(int how_much);
	static int count_worms;
};


class Bullet : public Movable, public Drawable {
private:
	glm::vec3 speed;
	//std::vector<const char*> filenames = { "textures/orange.png" };

	//void explosion();
public:
	Bullet(const std::string& obj_filename);
	void apply_gravity_and_wind(glm::vec3 wind, float time);  //na razie czas nie potrzebny
	glm::vec3 get_speed() { return speed; }
	void check_collision(Board* board, std::vector<Worm*> worms);
	void shoot(glm::vec3 pos, float angle_x, float angle_y);
};
