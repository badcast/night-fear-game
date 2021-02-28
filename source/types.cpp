#include "pch.h"

namespace RoninEngine
{
	const xVector2<float> xVector2<float>::up = { 0, 1 }, xVector2<float>::right = { 1, 0 }, xVector2<float>::left = { -1, 0 },
		xVector2<float>::down = { 0, -1 }, xVector2<float>::zero = { 0, 0 },
		xVector2<float>::one = { 1, 1 }, xVector2<float>::minusOne = { -1, -1 };

	double Get_Angle(Vec2 lhs, Vec2 rhs)
	{
		double YEnd = rhs.y;
		double YStart = lhs.y;
		double XEnd = rhs.x;
		double XStart = lhs.x;

		double a = SDL_atan2(YEnd - YStart, XEnd - XStart);
		if (a < 0) a += 2 * M_PI; //angle is now in radians

		a -= (M_PI/2); //shift by 90deg
							//restore value in range 0-2pi instead of -pi/2-3pi/2
		if (a < 0) a += 2 * M_PI;
		if (a < 0) a += 2 * M_PI;
		a = SDL_abs((M_PI * 2) - a); //invert rotation
		a = a * 180 / M_PI; //convert to deg

		return a;
	}

}

