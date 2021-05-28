#include "model.hpp"

tex = readTexture("bricks.png");


Model::Model(const std::string& obj_filename){
  load_model("Sir_Wormie.obj");

}
