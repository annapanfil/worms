#version 330

uniform sampler2D texMapColor; //jednostka teksturująca
uniform sampler2D texMapReflect;
uniform sampler2D texMapNormal;

out vec4 pixelColor; //Output variable. Almost final pixel color

in vec4 l;
in vec4 v;
in vec2 i_texc;


vec4 calc_light(vec4 light_position, vec4 normal, vec4 vertex, vec4 surface_color, vec4 reflect_color){
	vec4 reflect = reflect(-light_position, normal); //w przestrzeni oka, bo l i n są w tej przestrzeni

	float nl = clamp(dot(normal, light_position), 0,1); // cos of the angle between normal and light source vectors
	float alfa = 25; //how much the light is scattered
	float rv = pow(clamp(dot(reflect,vertex), 0, 1), alfa);

	//L = ka*la + kd*ld*nl + ks*ls*pow(rv, alfa)
	return vec4(surface_color.rgb * nl, surface_color.a) + vec4(reflect_color.rgb*rv, 0);
}


void main(void) {
	// float brighteness = 1.5;
	vec4 mn = normalize(vec4(texture(texMapNormal, i_texc).rgb*2-1, 0)); //pobranie wektorów normalnych tekstury i znormalizowanie ich do przedziału [-1,1]
	vec4 mv = normalize(v);

	vec4 kd = texture(texMapColor,i_texc); //kolor powierzchni
	vec4 ks = texture(texMapReflect,i_texc);	//kolor światła odbitego


	pixelColor=calc_light(l, mn, mv, kd, ks);




}
