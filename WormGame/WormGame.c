#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_ttf.h>
#include "structsAndEnums.h"

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
bool getsApple(struct Worm);
struct Worm* worm_initialize();
struct Worm* worm_getTail(struct Worm*);
MOVE_RESULT worm_checkMoveResult(struct Worm*);
void initializeColors();
void worm_grow(struct Worm* worm, int piecesToGrow);

const int GamePieceLength = 5;

const int gPiecesToGrow = 3;

struct Color gBackgroundColor, gWormColor, gFruitColor;
struct Apple gApple;
int gScore;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gEndImage = NULL;
TTF_Font* gFont = NULL;

int gGameGrid[GRID_WIDTH][SCREEN_WIDTH];

int main(int argc, char* argv[])
{
	if (!initializeSdl()) {
		printf("Failed to initialize!\n");
	}

	gGameOver = false;
	gApple.exists = false;

	initializeColors();

	bool quit = false;
	SDL_Event e;

	struct Worm* worm = worm_initialize();

	// initialize grid
	initializeGameGrid();

	while (!quit) {
		SDL_Delay(6);
		if (!gGameOver) {
			if (!gApple.exists) {
				createApple();
			}
			worm_updateLocation(worm, worm->location);						
		}

		if (!gGameOver) {
			updateGrid(worm);
			drawWindow();			
		}
		else {
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

	SDL_Color textColor = { 255, 255, 255 };

	char buffer[20];

	SDL_Rect bottomPart;
	bottomPart.x = 0;
	bottomPart.y = 520;
	bottomPart.w = SCREEN_WIDTH;
	bottomPart.h = 80;

	SDL_FillRect(gScreenSurface, &bottomPart, SDL_MapRGB(gScreenSurface->format, 170, 170, 170));

	sprintf_s(buffer, 20, "Score: %d", gScore);

	SDL_Surface* message = TTF_RenderText_Solid(gFont, buffer, textColor);

	SDL_Rect offset;
	offset.x = 50;
	offset.y = 550;

	SDL_BlitSurface(message, NULL, gScreenSurface, &offset);

	SDL_FreeSurface(message);

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

		MOVE_RESULT moveResult = worm_checkMoveResult(worm);

		switch (moveResult) {
		case FRUIT:
			gScore += 10;
			gApple.exists = false;

			worm_grow(worm, gPiecesToGrow);			
			break;
		case COLLISION:
			showEndImageAndSetGameOver();
			break;
		case EMPTY:
		default:
			break;
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

			if (TTF_Init() == -1) {
				return false;
			}

			gFont = TTF_OpenFont("Px437_IBM_VGA_8x16-2x.ttf", 28);
			if (gFont == NULL) {
				return false;
			}

			return true;
		}
	}
	return false;
}

void updateGrid(struct Worm* worm) {
	// draw worm's head's current location		
	gGameGrid[worm->location.x][worm->location.y] = WormTile;
	struct Worm* tail = worm_getTail(worm);

	// clear worm's tail's tile
	gGameGrid[tail->location.x][tail->location.y] = Empty;

	if (!gApple.exists) {
		for (int i = gApple.location.x; i < gApple.location.x + 3; i++) {
			for (int j = gApple.location.y; j < gApple.location.y + 3; j++) {
				gGameGrid[i][j] = Empty;
			}
		}
	}
}

void closeSdl() {

	//Close the font that was used
	TTF_CloseFont(gFont);

	//Quit SDL_ttf
	TTF_Quit();

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
	/*gEndImage = SDL_LoadBMP("endImage.bmp");
	SDL_BlitSurface(gEndImage, NULL, gScreenSurface, NULL);*/

	char buffer[20];

	SDL_Rect bottomPart;
	bottomPart.x = 0;
	bottomPart.y = 520;
	bottomPart.w = SCREEN_WIDTH;
	bottomPart.h = 80;

	SDL_FillRect(gScreenSurface, &bottomPart, SDL_MapRGB(gScreenSurface->format, 170, 170, 170));

	sprintf_s(buffer, 20, "Score: %d", gScore);

	SDL_Color textColor = { 200, 50, 50 };

	SDL_Surface* message = TTF_RenderText_Solid(gFont, buffer, textColor);

	SDL_Rect offset;
	offset.x = 20;
	offset.y = 550;

	SDL_BlitSurface(message, NULL, gScreenSurface, &offset);

	TTF_Font* font = TTF_OpenFont("Px437_IBM_VGA_8x16-2x.ttf", 18);
	message = TTF_RenderText_Solid(font, "Game over. Thanks for playing!", textColor);
	offset.y = 300;

	SDL_BlitSurface(message, NULL, gScreenSurface, &offset);

	SDL_FreeSurface(message);

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

	gApple.exists = true;
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

void worm_grow(struct Worm* worm, int piecesToGrow)
{
	if (piecesToGrow < 1) {
		return;
	}

	struct Worm* currentTail = worm_getTail(worm);

	struct Worm* newTail = NULL;
	newTail = (struct Worm*)malloc(sizeof(struct Worm));
	newTail->previous = currentTail;
	newTail->location = currentTail->location;
	newTail->next = NULL;
	newTail->direction = currentTail->direction;
	currentTail->next = newTail;

	piecesToGrow--;

	worm_grow(worm, piecesToGrow);
}

struct Worm* worm_initialize() {
	struct Worm* worm = (struct Worm*)malloc(sizeof(struct Worm));

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

	return worm;
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

MOVE_RESULT worm_checkMoveResult(struct Worm* worm)
{
	// Check if worm hits a border
	if (worm->location.x < 0 || worm->location.x >= GRID_WIDTH) {
		// crash to outer border horizontally.	
		return COLLISION;
	}
	else if (worm->location.y < 0 || worm->location.y >= GRID_HEIGHT) {
		// crash to outer border vertically.
		return COLLISION;
	}

	// check if worm hits its own tail
	struct Worm* wormPiece;
	// go through all parts but the last one, because it moves
	for (wormPiece = worm->next; wormPiece != NULL && wormPiece->next != NULL; wormPiece = wormPiece->next) {
		if (worm->location.x == wormPiece->location.x && worm->location.y == wormPiece->location.y) {
			return COLLISION;
		}
	}

	// check if worm gets the apple
	for (int i = max(0, gApple.location.x - 1); i < min(gApple.location.x + 3, GRID_WIDTH - 1); i++) {
		for (int j = max(0, gApple.location.y - 1); j < min(gApple.location.y + 3, GRID_HEIGHT - 1); j++) {
			if (i == worm->location.x && j == worm->location.y) {
				return FRUIT;
			}
		}
	}

	// default, worm goes to an empty tile.
	return EMPTY;
}
