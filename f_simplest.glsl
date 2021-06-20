#version 330

uniform sampler2D texMapColor; //jednostka teksturująca
uniform sampler2D texMapReflect;
uniform sampler2D texMapNormal;

out vec4 pixelColor; //Output variable. Almost final pixel color

// in vec4 i_color;
// in vec4 n;
in vec4 l;
in vec4 v;
in vec2 i_texc;

float toonify(float a, float s){
	return round(s*a)/s;
}

void main(void) {
	vec4 kd = texture(texMapColor,i_texc); //kolor powierzchni

	vec4 ml = normalize(l);
	// vec4 mn = normalize(n);
	vec4 mn = normalize(vec4(texture(texMapNormal, i_texc).rgb*2-1, 0)); //pobranie wektorów normalnych tekstury i znormalizowanie ich do przedziału [-1,1]
	vec4 mv = normalize(v);
	vec4 mr = reflect(-ml, mn); //w przestrzeni oka, bo l i n są w tej przestrzeni

	// vec4 ks = texture(texMapReflect,i_texc);	//kolor światła odbitego
	vec4 ks = vec4(1,1,1,1);

	float nl = clamp(dot(mn, ml), 0,1); // cos kąta
	float alfa = 25;
	float rv = pow(clamp(dot(mr,mv), 0, 1), alfa);

	//cieniowanie kreskówkowe
	// nl = toonify(nl, 4);
	// rv = toonify(rv, 4);

//L = ka*la + kd*ld*nl + ks*ls*pow(rv, alfa)
	pixelColor=vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);




}
