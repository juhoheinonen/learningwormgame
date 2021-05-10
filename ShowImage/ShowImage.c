#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gHelloWorld = NULL;

bool init() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow(
			"SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gWindow == NULL) {
			printf("Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
		}
		else {
			gScreenSurface = SDL_GetWindowSurface(gWindow);
			return true;
		}
	}
	return false;
}

bool loadMedia()
{
	bool success = true;

	gHelloWorld = SDL_LoadBMP("blackbird.bmp");
	if (gHelloWorld == NULL) {
		printf("Could not load image %s! SDL Error: %s\n", "blackbird.bmp", SDL_GetError());
	}

	return success;
}

void closeSdl() {
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	SDL_DestroyWindow(gWindow);

	SDL_Quit();
}

int main(int argc, char* argv[])
{
	if (!init()) {
		printf("Failed to initialize!\n");
	}
	else {
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {
			SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
			SDL_UpdateWindowSurface(gWindow);
			bool quit = false;
			SDL_Event e;
			while (!quit) {
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
			}
		}
	}

	closeSdl();

	return 0;
}