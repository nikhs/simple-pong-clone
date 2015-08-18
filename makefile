SOURCE = core.cpp
OBJECT = $(SOURCE:.cpp=.o)
OUTPUT = game.app
CC = g++
FLAGS = -Wall -std=c++11
LIB_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

all : obj
	$(CC) $(OBJECT) -o $(OUTPUT) $(LIB_FLAGS)

obj : 
	$(CC) -c $(SOURCE) $(FLAGS)

dist : permission
	tar -cvzf dist.tar.gz res $(OUTPUT)

permission : all
	chmod +x $(OUTPUT)
