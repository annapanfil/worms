#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "model.hpp"

// game is over - 0 health
class GameOverException : public std::exception {
std::string dead_worm_name;
public:
    GameOverException(std::string name) {
      dead_worm_name = name;
    }
    std::string what(){
      return dead_worm_name;
    }
};


// every object in game
class Everything {
private:
	glm::vec3 pos; // current position
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


// thing which one can draw
class Drawable: virtual public Everything {
private:
	Model model;     // read from obj file
  glm::vec3 scale; // scale of the original to display
public:
	Drawable(const std::string& _model_filename, std::vector<const char*> texture_filenames, glm::vec3 _scale, bool whole);
	void draw(GLFWwindow* window, glm::mat4 V);
	glm::mat4 calc_M_matrix();  // model matrix
};


// thing which one can move
class Movable: virtual public Everything {
public:
	void rotate(float angle, float time);
	void turn_right(float angle);
	void move_forward(float amount);
	Movable();
};


class Worm;

class Board : public Drawable {
private:
	float x; //size
	float z; //size

public:
	Board(const std::string& obj_filename, std::vector<const char*> tex_filenames);
	float get_x() { return x; }
	float get_z() { return z; }
	float get_height(float x, float z); //heigt in exact point
};


class Camera : public Movable {
private:
	bool walking_mode; // follow the worm or move (in the aiming mode)

public:
	glm::vec3 nose_vector;
	Camera();
	void change_mode(Worm* active_worm);
	void update_pos(glm::vec3 _pos, float delta_angle_x);  // move with worm (walking_mode) or change position after change_mode
	bool get_mode() { return walking_mode; }    //potrzebne żeby kamera poruszała się razem z robaczkiem
	void set_angle_y_restricted(float _angle_y); // block the camera field of view while aiming
};


class Worm : public Movable, public Drawable{
private:
	std::string name;
	int life;
	Board* board;
	Camera* camera;

public:
	Worm(std::string name, Board* board, Camera* camera, const std::string& obj_filename, std::vector<const char*> tex_filenames);
	void update(float speed, float angle_speed, double _time); // update position using speed
	void damage(int how_much); // damage when hit with bullet
	static int count_worms; //number of worms on the board
	int get_life();
};


class Bullet : public Movable, public Drawable {
private:
	glm::vec3 speed;

public:
	Bullet(const std::string& obj_filename, std::vector<const char*> tex_filenames);
	void apply_gravity_and_wind(glm::vec3 wind, float time); // move with respect to physics
	glm::vec3 get_speed() { return speed; }
	void check_collision(Board* board, std::vector<Worm*> worms);
	void shoot(glm::vec3 pos, float angle_x, float angle_y); // place in start position
};
