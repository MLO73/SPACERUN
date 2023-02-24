.DEFAULT_GOAL := all_default

install_dependencies_mac:
	brew install sdl2
	brew install sdl2_ttf
	brew install pkg-config

satisfy_vscode_mac:
	mkdir include && cp /opt/homebrew/include/SDL2/* ./include/
	cp /opt/homebrew/opt/sdl_ttf/include/SDL/* ./include/

all_default: compile run

compile:
	clang -std=c11 -Wall -Wextra -Werror -Wpedantic -o spacerun ./src/main.c `pkg-config --cflags --libs SDL2_ttf` `sdl2-config --cflags --libs` -lSDL2 -lSDL2_ttf -lSDL2_image -lavformat -lavcodec -lswresample -lswscale -lavutil -lm -lSDL2_mixer

run:
	./spacerun
