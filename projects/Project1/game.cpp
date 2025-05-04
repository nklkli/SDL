#include "game.h"
#include "image.h"
#include "sound.h"

void  normalised(float* x, float* y) {
	/*Return a unit vector
	Get length of vector(x, y) - math.hypot uses Pythagoras' theorem to get length of hypotenuse
	of right-angle triangle with sides of length x and y */
	float hypotenuse = SDL_sqrtf((*x * *x) + (*y * *y));
	*x = *x / hypotenuse;
	*y = *y / hypotenuse;
}

static void state_init(Game* game) {
	game->state = Game::GAMEPLAY;
	game->humanPaddle.x = game->PLAYFIELD_WIDTH - 40.f;
	game->humanPaddle.y = (float)game->PLAYFIELD_HALF_HEIGHT;
	game->humanPaddle.dy = game->PLAYFIELD_HEIGHT / 9.f;
	game->humanPaddle.image = "bat00.png";
	game->humanPaddle.w = game->humanPaddle.h = 160;

	game->computerPaddle.x = 40;
	game->computerPaddle.y = (float)game->PLAYFIELD_HALF_HEIGHT;
	game->computerPaddle.dy = game->PLAYFIELD_HEIGHT / 9.f;
	game->computerPaddle.image = "bat10.png";
	game->computerPaddle.w = game->computerPaddle.h = 160;

	game->ball.x = (float)game->PLAYFIELD_HALF_WIDTH;
	game->ball.y = (float)game->PLAYFIELD_HALF_HEIGHT;
	game->ball.dx = -1;
	game->ball.dy = 0;
	game->ball.speed = 5;
	game->ball.image = "ball.png";
	game->ball.w = game->ball.h = 24;

}

static void state_gameplay(Game* game) {

	/////////////////////////////
	//		HUMAN PLAYER PADDLE
	/////////////////////////////
	Paddle* humanPaddle = &game->humanPaddle;

	if (humanPaddle->move_command == Paddle::MoveCommand::UP) {
		humanPaddle->y += -humanPaddle->dy;
	}
	else if (humanPaddle->move_command == Paddle::MoveCommand::DOWN) {
		humanPaddle->y += humanPaddle->dy;
	}

	humanPaddle->y = SDL_min(400, SDL_max(80, humanPaddle->y));

	if (humanPaddle->command_play) {
		sound_play("foo.mp3");
	}

	humanPaddle->move_command = Paddle::MoveCommand::NONE;
	humanPaddle->command_play = false;

	image_render(humanPaddle->image,
		humanPaddle->x - humanPaddle->w / 2,
		humanPaddle->y - humanPaddle->h / 2);


	//////////////////////////////////
	//		COMPUTER'S PLAYER PADDLE
	/////////////////////////////////
	Paddle* computerPaddle = &game->computerPaddle;
	image_render(computerPaddle->image,
		computerPaddle->x - computerPaddle->w / 2,
		computerPaddle->y - computerPaddle->h / 2);

	///////////////////////////
	//			BALL
	//////////////////////////
	Ball* ball = &game->ball;
	for (size_t i = 0; i < ball->speed; i++)
	{
		// Store the previous x position
		float original_x = ball->x;

		// Move the ball based on dx and dy
		ball->x += ball->dx ;
		ball->y += ball->dy ;
		

		int new_dir_x = 0;
		Paddle* paddle = NULL;
		// Ball could collide with paddle on the x-axis?
		if (SDL_abs((int)ball->x - game->PLAYFIELD_HALF_WIDTH) >= 344 &&
			SDL_abs((int)original_x - game->PLAYFIELD_HALF_WIDTH) < 344) {

			if (ball->x < game->PLAYFIELD_HALF_WIDTH) { // left paddle is computer player
				new_dir_x = 1;
				paddle = computerPaddle;
			}
			else { // right paddle is human player
				new_dir_x = -1;
				paddle = humanPaddle;
			}

			float difference_y = ball->y - paddle->y;

			//  Check to see if ball needs to bounce off a bat
			if (difference_y > -64 and difference_y < 64) {
				// Ball has collided with bat - calculate new direction vector
				ball->dx = -ball->dx;

				// Deflect slightly up or down depending on where ball hit bat
				ball->dy += difference_y / 128;

				// Limit the Y component of the vector so we don't get into a situation where the ball is bouncing
				// up and down too rapidly
				ball->dy = SDL_min(SDL_max(ball->dy, -1), 1);

				// Ensure our direction vector is a unit vector, i.e.represents a distance of the equivalent of
				// 1 pixel regardless of its angle
				normalised(&ball->dx, &ball->dy);

				// Create an impact effect
				/*game.impacts.append(
						Impact((self.x - new_dir_x * 10, self.y)))*/

				// Increase speed with each hit
				ball->speed += 1;

				// Add an offset to the AI player's target Y position, so it won't aim to hit the ball exactly
				// in the centre of the bat
				//game.ai_offset = random.randint(-10, 10)

				// Bat glows for 10 frames
				// bat.timer = 10

				/* // Play hit sounds, with more intense sound effects as the ball gets faster
					// play every time in addition to :
				game.play_sound("hit", 5)
					if self.speed <= 10 :
						game.play_sound("hit_slow", 1)
						elif self.speed <= 12 :
						game.play_sound("hit_medium", 1)
						elif self.speed <= 16 :
						game.play_sound("hit_fast", 1)
					else:
				game.play_sound("hit_veryfast", 1)*/
			}
		}
		
		// Ball could collide with top or bottom wall of the playfield?
		// The top and bottom of the arena are 220 pixels from the centre
		if (SDL_abs(ball->y - (float)game->PLAYFIELD_HALF_HEIGHT) > 220) {
			//Invert vertical direction and apply new dy to y so that the ball is no longer overlapping with the
			// edge of the arena
			ball->dy = -ball->dy;
			ball->y += ball->dy;

			// Create impact effect
			// TODO game.impacts.append(Impact(self.pos))

			// TODO Sound effect
			/*	game.play_sound("bounce", 5)
			game.play_sound("bounce_synth", 1)*/
		}

	}
	image_render(game->ball.image,
		game->ball.x - game->ball.w / 2,
		game->ball.y - game->ball.h / 2);


}


void game_on_update(Game* game) {
	image_render("table.png", 0, 0);

	switch (game->state)
	{
	case Game::State::INIT:
		state_init(game);
		break;
	case Game::State::MENU:
		break;
	case Game::State::GAMEPLAY:
		state_gameplay(game);
		break;

	}
}

void game_on_event(Game* game, SDL_Event* e) {

	switch (game->state)
	{
	case Game::State::INIT:
		break;
	case Game::State::MENU:
		break;
	case Game::State::GAMEPLAY:
		if (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_RETURN) {
			game->humanPaddle.command_play = true;
		}

		if (e->type == SDL_EVENT_MOUSE_WHEEL) {
			SDL_Log("Mouse wheel: %.0f", e->wheel.y);
			game->humanPaddle.move_command = e->wheel.y < 0 ? Paddle::MoveCommand::DOWN : Paddle::MoveCommand::UP;
		}
		break;
	}


}

