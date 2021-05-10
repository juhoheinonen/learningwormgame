#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
struct Color backgroundColor, wormColor;

typedef struct Color
{
	Uint8 red;
	Uint8 green;
	Uint8 blue;
};

int main(int argc, char* argv[])
{	
	backgroundColor.red = 0;
	backgroundColor.green = 0;
	backgroundColor.blue = 0;

	wormColor.red = 10;
	wormColor.green = 255;
	wormColor.blue = 10;

	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else {
		//Create window
		window = SDL_CreateWindow(
			"SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == NULL) {
			printf("Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
		}
		else {
			screenSurface = SDL_GetWindowSurface(window);

			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, backgroundColor.red, backgroundColor.green, backgroundColor.blue));

			SDL_UpdateWindowSurface(window);

			SDL_Delay(2000);			
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
