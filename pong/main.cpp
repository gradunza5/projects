#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "paddle.h"
#include "ball.h"
#include "Timer.h"

#include <iostream>
#include <string>

// screen dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// paddle dimensions
const int PADDLE_HEIGHT = 64;
const int PADDLE_WIDTH = 10;

// frames per second
const int FRAMES_PER_SECOND = 65;

// speed up after this interval
const int SPEEDUP_INTERVAL = 30;

SDL_Window *window = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *ballImg = NULL;

SDL_Event event;

SDL_Surface *loadImage (std::string filename)
{
	SDL_Surface *loadedImage = NULL;

	loadedImage = IMG_Load (filename.c_str());

	return loadedImage;
}

bool init()
{
	if (SDL_Init (SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	window = SDL_CreateWindow ("Pong!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		return false;
	}

	screen = SDL_GetWindowSurface (window);

	if (screen == NULL)
	{
		return false;
	}

	return true;
}

void clean()
{
	SDL_FreeSurface(screen);
	SDL_Quit();
}

int main() 
{
	if (!init())
	{
		return 1;
	}

	ballImg = loadImage ("ball.png");
	Ball ball = Ball (screen, ballImg, SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 - 8, 1, 1);

	Paddle p1 = Paddle (screen, SCREEN_WIDTH-20, SCREEN_HEIGHT/2-PADDLE_HEIGHT/2, 
			PADDLE_WIDTH, PADDLE_HEIGHT);

	Paddle p2 = Paddle (screen, 20, SCREEN_HEIGHT/2-PADDLE_HEIGHT/2, 
			PADDLE_WIDTH, PADDLE_HEIGHT);

	// p1 can use default keys, p2 needs new ones bound.
	p2.setKeys (SDLK_w, SDLK_s);

	bool quit = false;
	bool winner = false;
	SDL_Event event;

	Timer fps;

	int bounceCount = 0;

	while (!quit)
	{
		fps.start();

		if (SDL_PollEvent (&event))
		{
			p1.handleEvents (event);
			p2.handleEvents (event);

			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		// white out the background
		SDL_FillRect (screen, &screen->clip_rect, SDL_MapRGB (screen->format, 0xFF, 0xFF, 0xFF));

		// update the paddles
		p1.draw ();
		p2.draw ();

		// update the ball
		ball.update();

		if (!winner) 
		{
			// check to see if the ball is past the paddle
			if (ball.getXPos() < 20)
			{
				// left player win
				std::cout << "right player win!" << std::endl;
				winner = true;
			}
			else if (ball.getXPos() > SCREEN_WIDTH - 20 - PADDLE_WIDTH)
			{
				// right player win
				std::cout << "left player win!" << std::endl;
				winner = true;
			}
		}

		// check to see if the ball should bounce off the right paddle
		if (ball.getXPos() + 6 == SCREEN_WIDTH - 20 - PADDLE_WIDTH 
				&& ball.getYPos() < p1.getYPos() + PADDLE_HEIGHT 
				&& ball.getYPos() > p1.getYPos())
		{
			std::cout << "right bounce!" << std::endl;
			ball.bounce();
		}

		SDL_UpdateWindowSurface (window);

		if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
		{
			SDL_Delay ((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
		}
	}

	clean();

	return 0;
}
