LIBS=-lGL -lglfw -lGLEW -lassimp
HEADERS=*.hpp *.h
FILES=*.cpp

main_file: $(FILES) $(HEADERS)
	g++ -o worms $(FILES)  $(LIBS) -I.
