#pragma once
#include <SDL3/SDL_events.h>


struct Paddle {
	enum MoveCommand { NONE, DOWN, UP };

	MoveCommand move_command = MoveCommand::NONE;
	bool command_play = false;

	const char* image;
	int w, h;
	float x, y;
	//The number of pixels the paddle moves vertically with each notch of the mouse scroll wheel.
	float dy;
};

struct Ball {
	int w, h, speed;
	float x, y, dx, dy;
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

	// We’ll update the game using a fixed time step.
	// The shorter this step is(smaller number), the more processing time it takes. 
	// The longer it is(bigger number), the choppier the gameplay is. 
	// This number unit is seconds.
	inline static const float UPDATE_STEP = 1/1000.f; 



};


void game_on_update(Game&);
void game_on_draw(const Game&, const float lag_ratio);
void game_on_event(Game&, const SDL_Event&);
