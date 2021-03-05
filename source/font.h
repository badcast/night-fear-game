#pragma once

#include "framework.h"

namespace RoninEngine::UI 
{

   constexpr int fontWidth = 13;
   constexpr int fontHeight = 18;

	enum TextAlign
	{
		Left, 
		Center,
		Right,
		MiddleLeft,
		MiddleCenter,
		MiddleRight,
		BottomLeft,
		BottomCenter,
		BottomRight
	};
	struct Font_t
	{
        SDL_Surface* surfNormal;
        SDL_Surface* surfHilight;
		Vector2i fontSize;
		Rect_t data[255];
	};
    void Initialize_Fonts(bool optimizeDeffects = true);
	int Single_TextWidth_WithCyrilic(const std::string& text, int fontSize);
	Rect_t Multiline_TextWidth_WithCyrilic(const std::string& text, int fontSize);
    void Render_String(SDL_Renderer* renderer, Rect_t rect, const char* text, int len, int fontWidth = RoninEngine::UI::fontWidth, TextAlign textAlign = TextAlign::Left, bool textWrap = true, bool hilight=false);
}

