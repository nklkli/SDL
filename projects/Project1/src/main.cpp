#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL_mixer.h>
#include "image.h"
#include "sound.h"
#include "game.h"


/* We will use this renderer to draw into this window every frame. */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;




/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetLogPriorities(SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);

	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	Game * game = new Game();
	//game->paddle.command_play = true;
	*appstate = game;

	if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", game->PLAYFIELD_WIDTH, game->PLAYFIELD_HEIGHT, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_SetRenderVSync(renderer, 1)) {
		SDL_Log("Renderer sync failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!sound_init()) {		
		//return SDL_APP_FAILURE;
	}	

	if (!image_init(renderer)) {
		return SDL_APP_FAILURE;
	}


	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT || (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)) {
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}
		
	game_on_event(*static_cast<Game*>(appstate), *event);

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	static Uint64 lastTime=0;
	static float lag=0;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  
	SDL_RenderClear(renderer);  /* start with a blank canvas. */
		
	Game* game = (Game*)appstate;

	Uint64 currentTime = SDL_GetTicks();
	Uint64 elapsed = currentTime - lastTime;
	lag += elapsed/1000.f;

	while (lag >= Game::UPDATE_STEP)
	{
		lag -= Game::UPDATE_STEP;
		game_on_update(*game);
	}

	game_on_draw(*game, lag / Game::UPDATE_STEP);

	lastTime = currentTime;

	/* put the newly-cleared rendering on the screen. */
	SDL_RenderPresent(renderer);	

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	delete (Game*)appstate;
	sound_free();
	image_free();

	/* SDL will clean up the window/renderer for us. */

	SDL_Quit();

}