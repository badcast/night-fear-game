#pragma once

#include "framework.h"

using namespace std;

namespace RoninEngine {

	struct Vec2;

	extern float FloatMinNormal;
	extern float FloatMinDenormal;
	extern bool IsFlushToZeroEnabled;
	extern float Epsilon;

	extern long double LongPI;
	extern float PI;
	extern float Infinity;
	extern float NegativeInfinity;
	extern float Deg2Rad;
	extern float Rad2Deg;

	class Random {
	public:
		static void srand(int seed);

		static int range(int min, int max);

		static float range(float min, float max);

		static float value();

		static Vec2 randomVector();

		static float randomAngle();
	};

	class Mathf
	{
	public:
		static bool Approximately(float a, float b);

		static float cos(float x);

		static float sin(float x);

		static float tan(float x);

		static float acos(float x);

		static float atan(float x);

		static float atan2(float y, float x);

		static float Clamp01(float val);

		static float Clamp(float val, const float min, const float max);

		static int Max(int x, int y);

		static float Max(float x, float y);

		static int Min(int x, int y);

		static float Min(float x, float y);

		static float abs(float x);

		static int abs(int x);

		//negative absolute value
		static float nabs(float x);

		static int nabs(int x);

		static float sign(float x);

		static int sign(int x);

		static float round(float x);

		static float ceil(float x);

		static float floor(float x);

		static float frac(float x);

		static int number(float x);

		static float exp(float x);

		static int pow(int x, int y);

		static float pow(float x, float y);

		static float sqrt(float x);

		static float repeat(float t, float length);

		static float DeltaAngle(float current, float target);

		static float Gamma(float value, float absmax, float gamma);

		static float InverseLerp(float a, float b, float value);

		static float Lerp(float a, float b, float t);

		static float LerpAngle(float a, float b, float t);

		static float LerpUnclamped(float a, float b, float t);

		static bool LineIntersection(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec2& result);

		static float MoveTowards(float current, float target, float maxDelta);

		static bool LineSegmentIntersection(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec2& result);

		static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed);

		static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime);

		static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

		static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed);

		static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime);

		static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

		static float SmoothStep(float from, float to, float t);
	};
}

