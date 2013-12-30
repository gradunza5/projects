#ifndef _ball_h_
#define _ball_h_

#include "SDL2/SDL.h"

#include <string>

class Ball
{
	private:
		SDL_Surface *field;
		SDL_Surface *image;

		SDL_Rect box;

		int x_vect;
		int y_vect;

		bool down;
		bool right;

	public:
		// destructor
		~Ball();

		// constructor
		Ball(SDL_Surface *field, SDL_Surface *image, int x, int y, int x_v, int y_v);

		void update();

		void draw();

		void speedUp(int xInc, int yInc);

		void bounce();

		int getXPos();

		int getYPos();

};

#endif
