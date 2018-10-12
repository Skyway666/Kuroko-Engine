
#include "Color.h"
#include "Random.h"

void Color::setRandom()
{
	r = randomFromTo(0, 1);
	g = randomFromTo(0, 1);
	b = randomFromTo(0, 1);
	a = 1.0f;
}

Color Red = Color(1.0f, 0.0f, 0.0f);
Color Green = Color(0.0f, 1.0f, 0.0f);
Color Blue = Color(0.0f, 0.0f, 1.0f);
Color Black = Color(0.0f, 0.0f, 0.0f);
Color White = Color(1.0f, 1.0f, 1.0f);