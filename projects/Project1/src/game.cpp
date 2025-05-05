#include "game.h"
#include "image.h"
#include "sound.h"

void  normalised(float& x, float& y) {
	/*Return a unit vector
	Get length of vector(x, y) - math.hypot uses Pythagoras' theorem to get length of hypotenuse
	of right-angle triangle with sides of length x and y */
	float hypotenuse = SDL_sqrtf((x * x) + (y * y));
	x = x / hypotenuse;
	y = y / hypotenuse;
}

static void state_init(Game& game) {

	game.state = Game::GAMEPLAY;
	game.humanPaddle.move.pos.x = game.PLAYFIELD_WIDTH - 40.f;
	game.humanPaddle.move.pos.y = (float)game.PLAYFIELD_HALF_HEIGHT;
	game.humanPaddle.move.vel.dy = 1;
	game.humanPaddle.move.vel.dy = 0;
	game.humanPaddle.move.vel.speed= game.PLAYFIELD_HEIGHT / 9.f;
	game.humanPaddle.image = "bat00.png";
	game.humanPaddle.w = game.humanPaddle.h = 160;

	game.computerPaddle.move.pos.x = 40;
	game.computerPaddle.move.pos.y = (float)game.PLAYFIELD_HALF_HEIGHT;
	game.computerPaddle.move.vel.dy = game.PLAYFIELD_HEIGHT / 9.f;
	game.computerPaddle.image = "bat10.png";
	game.computerPaddle.w = game.computerPaddle.h = 160;

	game.ball.move.pos.x = (float)game.PLAYFIELD_HALF_WIDTH;
	game.ball.move.pos.y = (float)game.PLAYFIELD_HALF_HEIGHT;
	game.ball.move.vel.dx = -1;
	game.ball.move.vel.dy = 0;
	game.ball.move.vel.speed = (game.PLAYFIELD_WIDTH * 0.5f);
	game.ball.image = "ball.png";
	game.ball.w = game.ball.h = 24;

}

Moveable move(Moveable m, float lag=1) {
	m.pos.x += m.vel.dx * m.vel.speed * Game::UPDATE_STEP * lag;
	m.pos.y += m.vel.dy * m.vel.speed * Game::UPDATE_STEP*lag;
	return m;
}

static void state_gameplay(Game& game) {


	/////////////////////////////
	//		HUMAN PLAYER PADDLE
	/////////////////////////////
	Paddle& humanPaddle = game.humanPaddle;
	Paddle* computerPaddle = &game.computerPaddle;
	Ball& ball = game.ball;

	if (humanPaddle.move_command == Paddle::MoveCommand::UP) {
		humanPaddle.move.pos.y -= humanPaddle.move.vel.speed;
		humanPaddle.move_command = Paddle::MoveCommand::NONE;
	}
	else if (humanPaddle.move_command == Paddle::MoveCommand::DOWN) {
		humanPaddle.move.pos.y += humanPaddle.move.vel.speed;
		humanPaddle.move_command = Paddle::MoveCommand::NONE;
	}
		
	humanPaddle.move.pos.y = SDL_min(400, SDL_max(80, humanPaddle.move.pos.y));

	if (humanPaddle.command_play) {
		sound_play("foo.opus");
		humanPaddle.command_play = false;
	}
	



	///////////////////////////
	//			BALL
	//////////////////////////

	/* https://gafferongames.com/post/fix_your_timestep/
	double t = 0.0;
	const double dt = 0.01;

	double currentTime = hires_time_in_seconds();
	double accumulator = 0.0;

	while ( !quit )
	{
		double newTime = hires_time_in_seconds();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while ( accumulator >= dt )
		{
			integrate( state, t, dt );
			accumulator -= dt;
			t += dt;
		}

		render( state );
	}
	*/


	// Store the previous x position
	float original_x = ball.move.pos.x;

	//SDL_Log("Before %f  %f", ball.move.x, ball.move.y);
	
	// Move the ball based on dx and dy
	ball.move = move(ball.move);
	
	//SDL_Log("After %f  %f", ball.move.x, ball.move.y);

	int new_dir_x = 0;
	Paddle* paddle = NULL;
	// Ball could collide with paddle on the x-axis?
	if (SDL_abs((int)ball.move.pos.x - game.PLAYFIELD_HALF_WIDTH) >= 344 &&
		SDL_abs((int)original_x - game.PLAYFIELD_HALF_WIDTH) < 344) {

		if (ball.move.pos.x < game.PLAYFIELD_HALF_WIDTH) { // left paddle is computer player
			new_dir_x = 1;
			paddle = computerPaddle;
		}
		else { // right paddle is human player
			new_dir_x = -1;
			paddle = &humanPaddle;
		}

		float difference_y = ball.move.pos.y - paddle->move.pos.y;

		//  Check to see if ball needs to bounce off a bat
		if (difference_y > -64 and difference_y < 64) {
			// Ball has collided with bat - calculate new direction vector
			ball.move.vel.dx = -ball.move.vel.dx;

			// Deflect slightly up or down depending on where ball hit bat
			ball.move.vel.dy += difference_y / 128;

			// Limit the Y component of the vector so we don't get into a situation where the ball is bouncing
			// up and down too rapidly
			ball.move.vel.dy = SDL_min(SDL_max(ball.move.vel.dy, -1), 1);

			// Ensure our direction vector is a unit vector, i.e.represents a distance of the equivalent of
			// 1 pixel regardless of its angle
			normalised(ball.move.vel.dx, ball.move.vel.dy);

			// Create an impact effect
			/*game.impacts.append(
					Impact((self.x - new_dir_x * 10, self.y)))*/

					// Increase speed with each hit
			ball.move.vel.speed += 1;

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
	if (SDL_abs((int)ball.move.pos.y - game.PLAYFIELD_HALF_HEIGHT) > 220) {
		//Invert vertical direction and apply new dy to y so that the ball is no longer overlapping with the
		// edge of the arena
		ball.move.vel.dy = -ball.move.vel.dy;
		ball.move.pos.y += ball.move.vel.dy;

		// Create impact effect
		// TODO game.impacts.append(Impact(self.pos))

		// TODO Sound effect
		/*	game.play_sound("bounce", 5)
		game.play_sound("bounce_synth", 1)*/
	}



}




void game_on_update(Game& game) {


	switch (game.state)
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

void game_on_draw(const Game& game, const float lag_ratio) {
	image_render("table.png", 0, 0);

	image_render(game.humanPaddle.image,
		game.humanPaddle.move.pos.x - game.humanPaddle.w / 2,
		game.humanPaddle.move.pos.y - game.humanPaddle.h / 2);

	image_render(game.computerPaddle.image,
		game.computerPaddle.move.pos.x - game.computerPaddle.w / 2,
		game.computerPaddle.move.pos.y - game.computerPaddle.h / 2);

	const Ball& ball = game.ball;
	
	auto m = move(ball.move, lag_ratio);

	image_render(ball.image, (m.pos.x - ball.w / 2), (m.pos.y - ball.h / 2) );
}

void game_on_event(Game& game, const SDL_Event& e) {

	switch (game.state)
	{
	case Game::State::INIT:
		break;
	case Game::State::MENU:
		break;
	case Game::State::GAMEPLAY:
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_RETURN) {
			game.humanPaddle.command_play = true;
		}

		if (e.type == SDL_EVENT_MOUSE_WHEEL) {
			SDL_Log("Mouse wheel: %.0f", e.wheel.y);
			game.humanPaddle.move_command = e.wheel.y < 0 ? Paddle::MoveCommand::DOWN : Paddle::MoveCommand::UP;
		}
		break;
	}


}

