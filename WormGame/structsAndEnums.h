#ifndef STRUCTS_AND_ENUMS_H
#define STRUCTS_AND_ENUMS_H
#endif

#include <SDL_stdinc.h>

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
	bool exists;
	struct Location location;
};

enum PixelDimensions { SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600 };
enum GridDimensions { GRID_WIDTH = 160, GRID_HEIGHT = 100 };
enum Directions { LEFT, UP, RIGHT, DOWN };
typedef enum { EMPTY, FRUIT, COLLISION } MOVE_RESULT;
