#pragma once

#include "framework.h"

namespace RoninEngine::Runtime {

	class Atlas
	{
		Texture* texture;
		std::map<string, Sprite*> _sprites;
	public:
		Atlas() = default;
		Atlas(const Atlas&) = default;
		~Atlas() = default;
		
		Sprite* Get_Sprite(const string& spriteName);
	};

}
