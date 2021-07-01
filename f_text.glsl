#version 330

uniform sampler2D font_tex;
out vec4 color;
in vec2 UV;

void main(){
    // color = texture( font_tex, UV );
		color = vec4(1,1,1,1);
}
