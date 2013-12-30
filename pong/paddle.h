#ifndef _paddle_h_
#define _paddle_h_

#include "SDL2/SDL.h"

class Paddle
{
	private:
		SDL_Surface *field;

		SDL_Rect box;

		SDL_Keycode upKey;
		SDL_Keycode dnKey;

		void moveUp();

		void moveDown();
	public:
		Paddle (SDL_Surface *field, int x, int y, int w, int h);

		void handleEvents (SDL_Event e);

		void draw();

		int getYPos();

		void setKeys(SDL_Keycode upKey, SDL_Keycode dnKey);
};

#endif
