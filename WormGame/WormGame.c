#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

bool init();
void drawWindow();
void worm_updateLocation(struct Worm*);
int main();
void initializeGameGrid();
struct Worm* worm_getTail(struct Worm);
void updateGrid(struct Worm);
//void drawWorm(struct Worm);
void closeSdl();
int worm_getLength(struct Worm);
void showEndTextAndEnd();

enum PixelDimensions { SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480 };
enum GridDimensions { GRID_WIDTH = 160, GRID_HEIGHT = 120 };
enum Directions { LEFT, UP, RIGHT, DOWN };

const int GamePieceLength = 4;

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
	struct Worm* previous;
};

struct Color backgroundColor, wormColor, fruitColor;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gHelloWorld = NULL;

int gGameGrid[GRID_WIDTH][SCREEN_WIDTH];

int main()
{
	if (!init()) {
		printf("Failed to initialize!\n");
	}

	backgroundColor.red = 0;
	backgroundColor.green = 0;
	backgroundColor.blue = 0;

	wormColor.red = 150;
	wormColor.green = 100;
	wormColor.blue = 10;

	fruitColor.red = 255;
	fruitColor.green = 10;
	fruitColor.blue = 10;

	bool quit = false;
	SDL_Event e;

	// initial position
	struct Worm worm;
	worm.isHead = true;
	worm.direction = LEFT;
	worm.previous = NULL;
	srand(time(NULL));
	int maxX = GRID_WIDTH - 1;
	worm.location.x = rand() % (maxX + 1);
	int maxY = GRID_HEIGHT - 1;
	worm.location.y = rand() % (maxY + 1);
	struct Worm tail;
	tail.isHead = false;
	tail.direction = worm.direction;
	tail.location.x = worm.location.x + 3;
	tail.location.y = worm.location.y;
	tail.next = NULL;
	tail.previous = &worm;
	worm.next = &tail;	

	// initialize grid
	initializeGameGrid();

	drawWindow();

	while (!quit) {
		SDL_Delay(20);
		worm_updateLocation(&worm);
		updateGrid(worm);
		drawWindow();

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

void initializeGameGrid()
{
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			gGameGrid[i][j] = Empty;
		}
	}
}

struct Worm* worm_getTail(struct Worm worm)
{
	while (worm.next != NULL) {
		worm = *worm.next;
	}
	return &worm;
}

void drawWindow() {
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	//SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, backgroundColor.red, backgroundColor.green, backgroundColor.blue));	

	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			SDL_Rect tile;
			tile.x = i * GamePieceLength;
			tile.y = j * GamePieceLength;
			tile.w = GamePieceLength;
			tile.h = GamePieceLength;
			if (gGameGrid[i][j] == Empty) {
				SDL_FillRect(gScreenSurface, &tile, SDL_MapRGB(gScreenSurface->format, backgroundColor.red, backgroundColor.green, backgroundColor.blue));
			}
			else if (gGameGrid[i][j] == WormTile) {
				SDL_FillRect(gScreenSurface, &tile, SDL_MapRGB(gScreenSurface->format, wormColor.red, wormColor.green, wormColor.blue));
			}
			else if (gGameGrid[i][j] == Fruit) {
				SDL_FillRect(gScreenSurface, &tile, SDL_MapRGB(gScreenSurface->format, wormColor.red, wormColor.green, wormColor.blue));
			}
		}
	}

	SDL_UpdateWindowSurface(gWindow);
}

void worm_updateLocation(struct Worm* worm)
{
	// Update to next part this part's previous location.
	if (worm->next != NULL) {
		worm_updateLocation(worm->next);
	}

	if (worm->previous == NULL) {
		if (worm->direction == UP) {
			worm->location.y -= 3;
		}
		else if (worm->direction == DOWN) {
			worm->location.y += 3;
		}
		else if (worm->direction == LEFT) {
			worm->location.x -= 3;
		}
		else if (worm->direction == RIGHT) {
			worm->location.x += 3;
		}
	}
	else {
		worm->location = worm->previous->location;
	}	
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
	// draw worm's head's current location
	if (worm.location.x < 0 || worm.location.x >= GRID_WIDTH) {
		// crash to outer border horizontally.
		showEndTextAndEnd();
	}
	else if (worm.location.y < 0 || worm.location.y >= GRID_WIDTH) {
		// crash to outer border vertically.
		showEndTextAndEnd();
	}
	else {
		gGameGrid[worm.location.x][worm.location.y] = WormTile;

		struct Worm* tail = worm_getTail(worm);

		// clear worm's tile
		gGameGrid[tail->location.x][tail->location.y] = Empty;
	}
}

void closeSdl() {
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	SDL_DestroyWindow(gWindow);

	SDL_Quit();
}

int worm_getLength(struct Worm worm)
{
	int length = 1;

	while (worm.next != NULL) {
		worm = *worm.next;
	}

	return length;
}

void showEndTextAndEnd()
{
	
}
