#pragma once
#include<raylib.h>
#include<raymath.h>
 
class ut {
	public:
		static float LinearInterpolation(float x1, float x2, float alpha) {
			return x1 * (1 - alpha) + x2 * alpha;
		}

		static float DegToRad(float degrees) {
			return degrees * (PI / 180);
		}

		static Vector2 RadToVec(float radians) {
			float x = cos(radians);
			float y = sin(radians);

			return Vector2Normalize(Vector2{x, y});
		}
};