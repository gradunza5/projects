#include "paddle.h"

Paddle::Paddle (SDL_Surface *field, int x, int y, int w, int h)
{
	this->field = field;

	box.x = x;
	box.y = y;
	box.w = w;
	box.h = h;
	
	// set default keys
	setKeys (SDLK_UP, SDLK_DOWN);
}

void Paddle::handleEvents (SDL_Event e)
{
	if (e.type == SDL_KEYDOWN)
	{
		if (e.key.keysym.sym == upKey)
		{
			moveUp();
		}
		if (e.key.keysym.sym == dnKey)
		{
			moveDown();
		}
	}
}

void Paddle::moveUp()
{
	box.y -= 16;
	if (box.y < 0)
	{
		box.y = 0;
	}
}

void Paddle::moveDown()
{
	box.y += 16;
	if (box.y > field->h - box.h)
	{
		box.y = field->h - box.h;
	}
}

void Paddle::draw()
{
	SDL_FillRect (field, &box, SDL_MapRGB (field->format, 0x00, 0x00, 0x00));
}

int Paddle::getYPos()
{
	return box.y;
}

void Paddle::setKeys (SDL_Keycode upKey, SDL_Keycode dnKey)
{
	this->upKey = upKey;
	this->dnKey = dnKey;
}
