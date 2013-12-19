#include <iostream>

#include "SDL2/SDL.h"

using namespace std;

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;


void logSDLError(ostream &os, const string msg)
{
	os << msg << " error: " << SDL_GetError() << endl;
}

int main(int argc, char* args[])
{

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		logSDLError(cout, "SDL_Init");
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello world!", 100, 100, SCREEN_HEIGHT, SCREEN_WIDTH, SDL_WINDOW_SHOWN);
	if (win == NULL)
	{
		logSDLError(cout, "SDL_CreateWindow");
		return 1;
	}

	SDL_Surface *screen = SDL_GetWindowSurface(win);

	SDL_FillRect(screen, NULL, 0);

	SDL_Delay(2000);

	SDL_FreeSurface(screen);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
