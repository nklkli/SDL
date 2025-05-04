#include <SDL3/SDL.h>
#include <windows.h>


float s(double now) {
	//return (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
	return SDL_sin(now* SDL_PI_D * 4 / 3);
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	for (float i = 0; i <= SDL_PI_F*2; i+=0.1)
	{
		SDL_Log("%5.1f => %.2f", i, s(i));
	}
	/*SDL_Log("%f", s(1));
	SDL_Log("%f", s(1.5));
	SDL_Log("%f", s(2));
	SDL_Log("%f", s(2.5));*/
}

