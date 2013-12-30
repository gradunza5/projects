#include "ball.h"

Ball::~Ball() 
{
	SDL_FreeSurface(image);
}

Ball::Ball(SDL_Surface *field, SDL_Surface *image, int x, int y, int x_v, int y_v)
{
	this->field = field;
	this->image = image;

	box.x = x;
	box.y = y;
	box.h = 16;
	box.w = 16;

	y_vect = y_v;
	x_vect = x_v;

	right = false;
	down = false;
}

void Ball::update()
{
	if (down)
	{
		box.y = box.y + y_vect;
	}
	else
	{
		box.y = box.y - y_vect;
	}

	if (right)
	{
		box.x = box.x + x_vect;
	}
	else
	{
		box.x = box.x - x_vect;
	}

	if (!right && box.x < 0)
	{
		box.x = 0;
		right = !right;
	}
	if (right && box.x > field->w - 16)
	{
		right = !right;
		box.x = field->w - 16;
	}

	if (down && box.y > field->h-16)
	{
		down = !down;
		box.y = field->h - 16;
	}
	if (!down && box.y < 0)
	{
		down = !down;
		box.y = 0;
	}

	draw();
}

void Ball::draw()
{
	SDL_BlitSurface(image, NULL, field, &box);
}

void Ball::speedUp(int xInc, int yInc)
{
	y_vect += yInc;
	x_vect += xInc;
}

void Ball::bounce()
{
	right = !right;
}

int Ball::getXPos()
{
	return box.x;
}

int Ball::getYPos()
{
	return box.y;
}
