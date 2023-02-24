#include <SDL2/SDL.h>

#include "game.h"

int WIDTH;
int HEIGHT;

void draw(SDL_Renderer *renderer, SDL_Texture* backgroundTexture);

void drawCubes(SDL_Renderer *renderer, Cube cubes[], int cubesLength);

void drawCube(SDL_Renderer *renderer, Cube cube);

void drawSpeedText(SDL_Renderer *renderer);

void drawGameOverText(SDL_Renderer *renderer);
