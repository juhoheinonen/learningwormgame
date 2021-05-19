#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_ttf.h>
#include <SDL_render.h>

bool initializeSdl();
void drawWindow();
void worm_updateLocation(struct Worm*, struct Location);
int main();
void initializeGameGrid();
void updateGrid(struct Worm*);
void closeSdl();
void showEndImageAndSetGameOver();
void createApple();
bool gGameOver;
bool gAppleExists;
bool getsApple(struct Worm);
struct Worm* worm_getTail(struct Worm*);
void initializeColors();

enum PixelDimensions { SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600 };
enum GridDimensions { GRID_WIDTH = 160, GRID_HEIGHT = 100 };
enum Directions { LEFT, UP, RIGHT, DOWN };

const int GamePieceLength = 5;

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
	struct Location location;
	enum Direction direction;
	struct Worm* next;
	struct Worm* previous;	
};

struct Apple
{
	struct Location location;	
};

struct Color gBackgroundColor, gWormColor, gFruitColor;

struct Apple gApple;

int gScore;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gEndImage = NULL;

int gGameGrid[GRID_WIDTH][SCREEN_WIDTH];

int main(int argc, char* argv[])
{
	if (!initializeSdl()) {
		printf("Failed to initialize!\n");
	}

	gGameOver = false;

	initializeColors();

	bool quit = false;
	SDL_Event e;

	// initial position
	struct Worm* worm = NULL;

	worm = (struct Worm*)malloc(sizeof(struct Worm));

	srand(time(NULL));
	int maxX = GRID_WIDTH - 1;
	worm->location.x = rand() % (maxX + 1);
	int maxY = GRID_HEIGHT - 4;
	worm->location.y = rand() % (maxY + 1);

	if (worm->location.x > (GRID_WIDTH / 2)) {
		worm->direction = LEFT;
	}
	else {
		worm->direction = RIGHT;
	}
	
	worm->previous = NULL;
		

	struct Worm* tail = NULL;
	tail = (struct Worm*)malloc(sizeof(struct Worm));
	tail->direction = worm->direction;
	tail->location.x = worm->location.x + 1;
	tail->location.y = worm->location.y;
	tail->next = NULL;	
	tail->previous = worm;
	worm->next = tail;
	gAppleExists = false;

	// initialize grid
	initializeGameGrid();

	while (!quit) {
		SDL_Delay(10);
		if (!gGameOver) {
			if (!gAppleExists) {
				createApple();
				gAppleExists = true;
			}

			worm_updateLocation(worm, worm->location);			
			updateGrid(worm);
			drawWindow();
		}
		if (gGameOver) {
			showEndImageAndSetGameOver();
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
					if (worm->direction != DOWN) {
						worm->direction = UP;
					}
					break;

				case SDLK_DOWN:
					if (worm->direction != UP) {
						worm->direction = DOWN;
					}
					break;

				case SDLK_LEFT:
					if (worm->direction != RIGHT) {
						worm->direction = LEFT;
					}
					break;

				case SDLK_RIGHT:
					if (worm->direction != LEFT) {
						worm->direction = RIGHT;
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

void drawWindow() {
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			SDL_Rect tile;
			tile.x = i * GamePieceLength;
			tile.y = j * GamePieceLength;
			tile.w = GamePieceLength;
			tile.h = GamePieceLength;
			if (gGameGrid[i][j] == Empty) {
				SDL_FillRect(gScreenSurface, &tile, SDL_MapRGB(gScreenSurface->format, gBackgroundColor.red, gBackgroundColor.green, gBackgroundColor.blue));
			}
			else if (gGameGrid[i][j] == WormTile) {
				SDL_FillRect(gScreenSurface, &tile, SDL_MapRGB(gScreenSurface->format, gWormColor.red, gWormColor.green, gWormColor.blue));
			}
			else if (gGameGrid[i][j] == Fruit) {
				SDL_FillRect(gScreenSurface, &tile, SDL_MapRGB(gScreenSurface->format, gFruitColor.red, gFruitColor.green, gFruitColor.blue));
			}
		}
	}

	SDL_UpdateWindowSurface(gWindow);
}

void worm_updateLocation(struct Worm* worm, struct Location previousLocation)
{		
	if (worm->previous == NULL) {
		struct Location currentLocation = worm->location;

		if (worm->direction == UP) {
			worm->location.y--;
		}
		else if (worm->direction == DOWN) {
			worm->location.y++;
		}
		else if (worm->direction == LEFT) {
			worm->location.x--;
		}
		else if (worm->direction == RIGHT) {
			worm->location.x++;
		}
		
		if (getsApple(*worm)) {
			gScore += 10;
			gAppleExists = false;
			struct Worm* currentTail = worm_getTail(worm);

			struct Worm* newTail = NULL;
			newTail = (struct Worm*)malloc(sizeof(struct Worm));
			newTail->previous = currentTail;
			newTail->location = currentTail->location;			
			newTail->next = NULL;			
			newTail->direction = currentTail->direction;
			currentTail->next = newTail;			
		}
		struct Worm* next = worm->next;
		if (next != NULL) {
			worm_updateLocation(next, currentLocation);
		}
	}
	else {
		struct Location currentLocation = worm->location;
		worm->location = previousLocation;
		struct Worm* next = worm->next;
		if (next != NULL) {
			worm_updateLocation(next, currentLocation);
		}
	}
}

bool initializeSdl() {	
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

void updateGrid(struct Worm* worm) {
	// draw worm's head's current location
	if (worm->location.x < 0 || worm->location.x >= GRID_WIDTH) {
		// crash to outer border horizontally.	
		showEndImageAndSetGameOver();
	}
	else if (worm->location.y < 0 || worm->location.y >= GRID_HEIGHT) {
		// crash to outer border vertically.
		showEndImageAndSetGameOver();
	}
	else {
		gGameGrid[worm->location.x][worm->location.y] = WormTile;		
		struct Worm* tail = worm_getTail(worm);

		// clear worm's tail's tile
		gGameGrid[tail->location.x][tail->location.y] = Empty;		

		if (!gAppleExists) {
			for (int i = gApple.location.x; i < gApple.location.x + 3; i++) {
				for (int j = gApple.location.y; j < gApple.location.y + 3; j++) {
					gGameGrid[i][j] = Empty;
				}
			}
		}
	}
}

void closeSdl() {
	SDL_FreeSurface(gScreenSurface);
	SDL_FreeSurface(gEndImage);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

void showEndImageAndSetGameOver()
{
	gGameOver = true;

	gScreenSurface = SDL_GetWindowSurface(gWindow);

	gGameOver = true;
	gEndImage = SDL_LoadBMP("endImage.bmp");
	SDL_BlitSurface(gEndImage, NULL, gScreenSurface, NULL);
	SDL_UpdateWindowSurface(gWindow);
}

void createApple()
{
	srand(2 * time(NULL));
	int maxX = GRID_WIDTH - 4;
	gApple.location.x = rand() % (maxX + 1);
	int maxY = GRID_HEIGHT - 4;
	gApple.location.y = rand() % (maxY + 1);

	if (gApple.location.x <= 10) {
		gApple.location.x = 20;
	}
	if (gApple.location.x > GRID_WIDTH - 10) {
		gApple.location.x = GRID_WIDTH - 20;
	}
	if (gApple.location.y <= 10) {
		gApple.location.y = 20;
	}
	if (gApple.location.y > GRID_HEIGHT - 10) {
		gApple.location.y = GRID_HEIGHT - 20;
	}

	for (int i = gApple.location.x; i < gApple.location.x + 3; i++) {
		for (int j = gApple.location.y; j < gApple.location.y + 3; j++) {
			gGameGrid[i][j] = Fruit;
		}
	}
}

void initializeColors() {
	gBackgroundColor.red = 222;
	gBackgroundColor.green = 222;
	gBackgroundColor.blue = 222;

	gWormColor.red = 150;
	gWormColor.green = 100;
	gWormColor.blue = 10;

	gFruitColor.red = 255;
	gFruitColor.green = 10;
	gFruitColor.blue = 10;
}

bool getsApple(struct Worm worm) {	
	for (int i = max(0, gApple.location.x - 1); i < min(gApple.location.x + 3, GRID_WIDTH - 1); i++) {
		for (int j = max(0, gApple.location.y - 1); j < min(gApple.location.y + 3, GRID_HEIGHT - 1); j++) {
			if (i == worm.location.x && j == worm.location.y) {
				return true;
			}
		}
	}
	return false;
}

struct Worm* worm_getTail(struct Worm* worm) {		
	struct Worm* currentTail = worm->next;

	while (currentTail->next != NULL) {
		currentTail = currentTail->next;
	}

	return currentTail;	
}