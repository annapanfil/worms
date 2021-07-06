#version 330

uniform sampler2D font_tex;
out vec4 color;
in vec2 uv;

void main(){
  color = texture(font_tex, uv);
}
