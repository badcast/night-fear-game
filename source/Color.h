#pragma once
#include "framework.h"

namespace RoninEngine::Runtime {
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color() {
			memset(this, NULL, sizeof(Color));
		}

		explicit Color(const int32_t rgba) {
			memcpy(this, &rgba, sizeof(Color));
		}

		explicit Color(const uint32_t rgba) {
			memcpy(this, &rgba, sizeof(Color));
		}

		explicit Color(const char* colorHex) {
			//todo: hex to int
		}

		Color(const uint8_t r, const  uint8_t g, const  uint8_t b, const uint8_t a) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}

	};

}
