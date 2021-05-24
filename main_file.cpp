/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

float speed_x = 0;//[radiany/s]
float speed_y = 0;//[radiany/s]

GLuint tex; //Uchwyt – deklaracja globalna

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			speed_y = -PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = PI;
		}
		if (key == GLFW_KEY_UP) {
			speed_x = -PI;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = PI;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = PI;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			speed_y = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			speed_x = 0;
		}
	}
}


GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.2, 0.2, 0.7, 1);				//Ustaw kolor czyszczenia bufora kolorów				(kolor tła)			
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glfwSetKeyCallback(window, key_callback);

	//Wczytanie i import obrazka – w initOpenGLProgram
	tex = readTexture("bricks.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	//Usunięcie tekstury z pamięci karty graficznej – w freeOpenGLProgram
	glDeleteTextures(1, &tex);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości
	
	glm::mat4 M = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową
	M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi Y
	M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi X
	glm::mat4 V = glm::lookAt(glm::vec3(10.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, -1.0f)); //Wylicz macierz widoku (skąd patrzymy)
							//lookAt(vec3 observer,				vec3 center,						vec3 noseVector) (nosevec - oś po której się poruszamy i wg której kierujemy wzrok)

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania


	float verts[] = {
    0,0,-4,1,     1,0.2,-4,1,   2,0.6,-4,1,    3,0.4,-4,1,	4,0,-4,1,
	0,0,-3,1,	  1,0,-3,1,     2,0.2,-3,1,    3,0,-3,1,    4,0,-3,1,	//
	0,0,-2,1,     1,0.2,-2,1,   2,0,-2,1,      3,0,-2,1,	4,0,-2,1,
	0,0.2,-1,1,	  1,0.5,-1,1,   2,0.3,-1,1,    3,0.2,-1,1,  4,0,-1,1,
	0,0,0,1,	  1,0.3,0,1,    2,0,0,1,       3,0,0,1,     4,0,0,1		//5 wierszy
	//0             0             0              0            0
	};

	float normals[] = {
    0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, 0,1,0,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,
	0,1,0,0,    0.2,0.2,-0.2,0,  0,1,0,0, -0.2,-0.2,-0.2,0, 0,1,0,0,

	};																	//jak liczyć te normalne??? jak zrobić z nich sensowny cień

	float colors[] = {
	  1,0,0,1,  0,1,0,1,  0,0,1,1,  1,1,0,1,  0,1,0,1, 1,0,0,1,  1,0,0,1,  1,0,0,1,  1,0,0,1, 1,0,0,1,
	  1,0,0,1,  0,1,0,1,  0,0,1,1,  1,1,0,1,  0,1,0,1, 1,0,0,1,  1,0,0,1,  1,0,0,1,  1,0,0,1, 1,0,0,1,
	  1,0,0,1,  0,1,0,1,  0,0,1,1,  1,1,0,1,  0,1,0,1,
	};
	unsigned int indexes[] = {
	  0,1,5,  1,2,6,  2,3,7,  3,4,8,
	  5,1,6,  6,2,7,  7,3,8,  8,4,9,	//
	  10,6,11,  11,7,12,  12,8,13,  13,9,14,
	  5,6,10,  6,7,11,  7,8,12,  8,9,13,  
	  10,11,15, 11,12,16, 12,13,17, 13,14,18, 
	  15,16,11, 16,17,12, 17,18,13, 18,19,14,
	  15,16,20, 16,17,21, 17,18,22, 18,19,23,
	  20,21,16, 21,22,17, 22,23,18, 23,24,19
	};
	int indexCount = 96;		//ilość trójkątów w indexes *3
	int vertexCount = 25;		//(tyle ile największa liczba w intexes[]+1)


	float texCoords[] = {
  1,0, 1,1, 0,0, //0,1,5  
  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  //4 'górne'  0,1,5
  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  //4 'dolne'  5,1,6
  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  //4 'dolne'  10,6,11
  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  //4 'górne'  5,6,10
  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  0,0, 1,1, 1,0,  //4 'dolne'  10,11,15
  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,	// dolne inaczej 15,16,11
  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  1,0, 1,1, 0,0,  //4 'górne' 15,16,20
  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,  0,0, 1,0, 1,1,	// dolne inaczej 20,21,16
	};

	

	//Zamiast poniższych linijek należy wstawić kod dotyczący rysowania własnych obiektów (glDrawArrays/glDrawElements i wszystko dookoła)
	/*//-----------
	spColored->use();
	glUniformMatrix4fv(spColored->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spColored->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spColored->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(spColored->a("vertex"));
	glVertexAttribPointer(spColored->a("vertex"), 4, GL_FLOAT, false, 0, verts);

	glEnableVertexAttribArray(spColored->a("color"));
	glVertexAttribPointer(spColored->a("color"), 4, GL_FLOAT, false, 0, colors);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexes);

	glDisableVertexAttribArray(spColored->a("vertex"));
	glDisableVertexAttribArray(spColored->a("color"));
	//-----------*/
	/*
	spTextured->use();
	glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(spTextured->a("vertex"));
	glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, verts);

	glEnableVertexAttribArray(spTextured->a("texCoord"));
	glVertexAttribPointer(spTextured->a("texCoord"), 2, GL_FLOAT, false, 0, texCoords);

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(spTextured->u("tex"), 0);

	//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexes);

	glDisableVertexAttribArray(spTextured->a("vertex"));
	glDisableVertexAttribArray(spTextured->a("texCoord"));	*/

	
	spLambertTextured->use();
	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(spLambertTextured->a("vertex"));
	glVertexAttribPointer(spLambertTextured->a("vertex"), 4, GL_FLOAT, false, 0, verts);
	glEnableVertexAttribArray(spLambertTextured->a("normal"));
	glVertexAttribPointer(spLambertTextured->a("normal"), 4, GL_FLOAT, false, 0, normals);
	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
	glVertexAttribPointer(spLambertTextured->a("texCoord"), 2, GL_FLOAT, false, 0, texCoords);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, tex); 
	glUniform1i(spLambertTextured->u("tex"), 0);

	//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexes);

	glDisableVertexAttribArray(spLambertTextured->a("vertex"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));
	glDisableVertexAttribArray(spLambertTextured->a("texCoord"));


	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1200, 1200, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora czas pomiędzy zmianą klatek (zamiana buforów)

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	float angle_y = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_x += speed_x * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window,angle_x,angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
