LIBS=-lGL -lglfw -lGLEW
HEADERS=*.hpp *.h
FILES=main.cpp shaderprogram.cpp

main_file: $(FILES) $(HEADERS)
	g++ -o worms $(FILES)  $(LIBS) -I.
