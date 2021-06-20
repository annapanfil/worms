#version 330

uniform sampler2D tex; //jednostka teksturująca

out vec4 pixelColor; //Output variable. Almost final pixel color

in vec4 i_color;
in vec4 n;
in vec4 l;
in vec4 v;
in vec2 i_texc;

void main(void) {
	float brighteness = 1.5;

	vec4 kd = texture(tex,i_texc) * brighteness; //kolor powierzchni

	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	vec4 mr = reflect(-ml, mn); //w przestrzeni oka, bo l i n są w tej przestrzeni

	vec4 ks = kd;	//kolor światła odbitego

	float nl = clamp(dot(mn, ml), 0,1); // cos kąta
	float alfa = 25;
	float rv = pow(clamp(dot(mr,mv), 0, 1), alfa);

//L = ka*la + kd*ld*nl + ks*ls*pow(rv, alfa)
	pixelColor=vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);




}
