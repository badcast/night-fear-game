#pragma once

#include "framework.h"

namespace RoninEngine::ui 
{

#define FONT_WIDTH 13
#define FONT_HEIGHT 18

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
		SDL_Surface* texture;
		SDL_Surface* surfaceHilight;
		Vector2i fontSize;
		Rect_t data[255];
	};
	void Initialize_Fonts(Font_t*fontData, bool optimizeDeffects);
	int Single_TextWidth_WithCyrilic(const std::string& text, int fontSize);
	Rect_t Multiline_TextWidth_WithCyrilic(const std::string& text, int fontSize);
	void Render_String(SDL_Renderer* renderer, Rect_t rect, const char* text, int len, int fontWidth = FONT_WIDTH, TextAlign textAlign = TextAlign::Left, bool textWrap = true, bool hilight=false);
}

