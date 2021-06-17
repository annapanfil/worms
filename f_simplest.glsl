#version 330


out vec4 pixelColor; //Output variable. Almost final pixel color
in vec4 i_color;
in n;
in l;
in v;
in vec2 i_tc;

float toonify(float a, s){
	return round(s*a)/s
}

void main(void) {
	vec4 color=texture(tex,i_tc);

	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	vec4 mr = reflect(-ml, mn); //w przestrzeni oka, bo l i n są w tej przestrzeni
	float nl = clamp(dot(mn, ml), 0,1); // cos kąta
	float alfa = 25;
	float rv = pow(clamp(dot(mr,mv), 0, 1), alfa)

	//cieniowanie kreskówkowe
	nl = toonify(nl, 4);
	rv = toonify(rv, 4);


	pixelColor=vec4(color.rgb * nl, i_color.a) + vec4(rv, rv, rv, 0);




}
