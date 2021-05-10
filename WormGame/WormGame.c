#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

bool init();
void drawWindow();
void updateGrid(struct Worm);
void drawWorm(struct Worm);
void closeSdl();

enum Dimensions { SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480 };
enum Directions { LEFT, UP, RIGHT, DOWN };

enum WormPieceSize { WORM_PIECE_WIDTH = 9, WORM_PIECE_HEIGHT = 9 };

struct Color
{
	Uint8 red;
	Uint8 green;
	Uint8 blue;
};

enum TileContent {
	Empty = 0,
	Fruit = 1,
	WormTile = 2
};

struct Location {
	int x;
	int y;
};

struct Worm
{
	bool isHead;
	struct Location location;
	enum Direction direction;
	struct Worm* next;
};

struct Color backgroundColor, wormColor;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gHelloWorld = NULL;

int gGameGrid[SCREEN_WIDTH][SCREEN_HEIGHT];

int main()
{    
	if (!init()) {
		printf("Failed to initialize!\n");
	}

	backgroundColor.red = 0;
	backgroundColor.green = 0;
	backgroundColor.blue = 0;

	wormColor.red = 10;
	wormColor.green = 255;
	wormColor.blue = 10;

	bool quit = false;
	SDL_Event e;

	drawWindow();

	// initial position
	srand(time(NULL));	
	int maxX = SCREEN_WIDTH - 1;
	int x = rand() % (maxX + 1);	
	int maxY = SCREEN_HEIGHT - 1;
	int y = rand() % (maxY + 1);	

	struct Worm worm;
	worm.isHead = true;
	worm.location.x = x;
	worm.location.y = y;
	worm.direction = LEFT;

	/*struct Worm* tail;
	tail->isHead = false;
	tail->location.x = worm.location.x + 3;
	tail->location.y = worm.location.y;
	worm.next = tail;*/

	drawWorm(worm);	

	// initialize grid
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			gGameGrid[i][j] = Empty;
		}
	}
	
	while (!quit) {
		SDL_Delay(20);		
		if (worm.direction == UP) {
			//worm.next->location.y = worm.location.y;
			worm.location.y -= 3;
			updateGrid(worm);
			drawWorm(worm);
		}
		else if (worm.direction == DOWN) {
			//worm.next->location.y = worm.location.y;
			worm.location.y += 3;
			updateGrid(worm);
			drawWorm(worm);
		}
		else if (worm.direction == LEFT) {
			//worm.next->location.x = worm.location.x;
			worm.location.x -= 3;
			updateGrid(worm);
			drawWorm(worm);
		}
		else if (worm.direction == RIGHT) {
			//worm.next->location.x = worm.location.x;
			worm.location.x += 3;
			updateGrid(worm);
			drawWorm(worm);
		}

		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					if (worm.direction != DOWN) {
						worm.direction = UP;
					}
					break;

				case SDLK_DOWN:
					if (worm.direction != UP) {
						worm.direction = DOWN;
					}
					break;

				case SDLK_LEFT:
					if (worm.direction != RIGHT) {
						worm.direction = LEFT;
					}					
					break;

				case SDLK_RIGHT:
					if (worm.direction != LEFT) {
						worm.direction = RIGHT;
					}
					break;

				default:
					
					break;
				}
			}
		}
	}

	closeSdl();

	return 0;
}

void drawWindow() {
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, backgroundColor.red, backgroundColor.green, backgroundColor.blue));

	SDL_UpdateWindowSurface(gWindow);	
}

bool init() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow(
			"Worm game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

void updateGrid(struct Worm worm) {
	// todo: length of worm should be taken into account.
	
	// clear tail of worm.
	if (worm.next != NULL) {
		// do something
	}
	// only 1 length worm, clear current location
	else {

	}
}

void drawWorm(struct Worm worm) {
	SDL_Rect wormHead;
	wormHead.x = worm.location.x;
	wormHead.y = worm.location.y;
	wormHead.w = WORM_PIECE_WIDTH;
	wormHead.h = WORM_PIECE_HEIGHT;

	gScreenSurface = SDL_GetWindowSurface(gWindow);

	SDL_FillRect(gScreenSurface, &wormHead, SDL_MapRGB(gScreenSurface->format, wormColor.red, wormColor.green, wormColor.blue));

	SDL_UpdateWindowSurface(gWindow);
}

void closeSdl() {
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	SDL_DestroyWindow(gWindow);

	SDL_Quit();
}