#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
// uniform vec4 light_position = vec4(20,30,-35,0); //in world space
uniform vec4 light_position = vec4(0,50,0,0);


//Attributes
in vec4 vertex; //Vertex coordinates in model space
//in vec4 color;
in vec4 normal; //in model space
in vec2 texCoord; //texturing coordinates
in vec4 tangent; // in model space


//Zmienne interpolowane
// out vec4 i_color; // interpolowany kolor
// out vec4 n;     // wektor normalny powierzchi w przestrzeni oka
out vec4 l;    // znormalizowany wektor do źródła światła w przestrzeni oka
out vec4 v;
out vec2 i_texc;  // współrzędne teksturowania


void main(void) {
    // l = normalize(V*light_position - V*M*vertex);
    // n = normalize(V*M*normal);
    // float nl = clamp(dot(n, l), 0,1); // cos kąta
    // kąt nie może być rozwarty (byłby pod powierzchnią)

    /* liczymy kolory*/
    /*lambert*/
    // interpolacted_color = kd*ld*nl
    // vec4 ld = vec4(1,1,1,1); //kolor światła - białe
    // kd - kolor powierzchni

    // interpolated_color = vec4(color.rgb * nl, color.a); //ze wzoru, nie zmieniamy alfy

    /*phong*/
    //L = ka*la + kd*ld*nl + ks*ls*pow(rv, alfa)
    //ks - kolor materiału dla światła odbitego
    //ld - kolor światła odbitego
    //rv – cos kąta między światłem odbitym a wektorem od powierzchni do obserwatora
    //alfa – określa jak bardzo światło jest rozpraszane (0-300+)

    // vec4 r = reflect(-l, n); //w przestrzeni oka, bo l i n są w tej przestrzeni
    // v = normalize(vec4(0,0,0,1) - V*M*vertex); //od powierzchni do obserwatora
    // float alfa = 25;
    // float rv = pow(clamp(dot(r,v), 0, 1), alfa)
    //
    // interpolated_color = vec4(color.rgb * nl, color.a) + vec4(rv, rv, rv, 0);
    // i_color = color;

    //normal mapping
    // wszystko wyrażamy do przestrzeni styczniej
    //invTBM -> z modelu do stycznej

    vec4 norm_n = normalize(normal);
    vec4 norm_bitangent = normalize(vec4(cross(norm_n.xyz, tangent.xyz),0));
    vec4 norm_tangent = normalize(tangent);
    mat4 TBN = mat4(norm_tangent, norm_bitangent, norm_n, vec4(0,0,0,1));
    mat4 invTBN = inverse(TBN);

    l = normalize(invTBN*inverse(M)*light_position - invTBN*vertex);
    v = normalize(invTBN*inverse(V*M)*vec4(0,0,0,1) - invTBN*vertex); //od powierzchni do obserwatora

    i_texc=texCoord;

    gl_Position=P*V*M*vertex;
}