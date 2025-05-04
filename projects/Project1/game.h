#pragma once
#include <SDL3/SDL_events.h>


struct Paddle {
	enum MoveCommand { NONE, DOWN, UP };

	MoveCommand move_command = MoveCommand::NONE;
	bool command_play = false;

	const char* image;
	int w,h;
	float x, y;
	//The number of pixels the paddle moves vertically with each notch of the mouse scroll wheel.
	float dy;
};

struct Ball {
	int w, h, speed;
	float x,y, dx, dy;
	const char* image;
};

struct Game {
	enum State { INIT, MENU, GAMEPLAY };
	State state = INIT;
	const int PLAYFIELD_WIDTH = 800;
	const int PLAYFIELD_HEIGHT = 480;
	const int PLAYFIELD_HALF_WIDTH = PLAYFIELD_WIDTH / 2;
	const int PLAYFIELD_HALF_HEIGHT = PLAYFIELD_HEIGHT / 2;
	Paddle humanPaddle;
	Paddle computerPaddle;
	Ball ball;
	// Seconds elapsed between current and last frame.
	float elapsed = 0;
	// Fixed delta time (in seconds) for physics simulation
	const float dt = 1 / 120.f;
	// For physics simulation
	float accumulator = 0;
};


void game_on_update(Game*);
void game_on_event(Game*, SDL_Event*);
