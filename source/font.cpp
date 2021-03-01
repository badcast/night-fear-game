#include "pch.h"
#include <stdlib.h>

#pragma warning(disable : 26812)

namespace RoninEngine::ui
{
	Font_t* pFont = NULL;
	uint8_t VH[]{ 0,32,16,2,34,18,1,33,17 };

	SDL_Texture* fontTexture = NULL;
	SDL_Texture* fontTextureHilight = NULL;

	void Initialize_Fonts(Font_t* fontData, bool optimizeDeffects)
	{
		pFont = fontData;

		if (fontTexture)
		{
			SDL_DestroyTexture(fontTexture);
			SDL_DestroyTexture(fontTextureHilight);
		}

		//convert texture to texture
		fontTexture = SDL_CreateTextureFromSurface(RoninApplication::instance()->GetRenderer(), pFont->texture);
		fontTextureHilight = SDL_CreateTextureFromSurface(RoninApplication::instance()->GetRenderer(), pFont->surfaceHilight);

		if (optimizeDeffects)
		{
			SDL_Surface* model = fontData->texture;
			int i, x, y, cx, cy;
			Rect_t rect_point;
			SDL_Color* pixel;
			for (i = 33; i != 255; ++i)
			{
				//todo: оптимизация прямоугольника
				rect_point = fontData->data[i];
				uint8_t flagBreaker = 0;
				//pitch это строка (ширина с объеденением в байтах) то есть x по оси
				//если умножить pitch * на h-высоту то можно получить последний пиксель
				for (x = 0; x < fontData->data[i].w / 2; ++x)
				{
					//оптимизация с левой до правой 
					if (!(flagBreaker & 1))
					{
						for (y = rect_point.h - 1; y >= 0; --y)
						{
							cx = rect_point.x + x;
							cy = rect_point.y + y;
							//Формула Y Offset * (Pitch/BytesPerPixel) + X Offset
							pixel = (SDL_Color*)fontData->texture->pixels + cy * (model->pitch / model->format->BytesPerPixel) + cx;
							if (pixel->a) // isn't transparent
								break;
						}
						if (y == ~0)
							++rect_point.x;
						else
							flagBreaker |= 1;
					}
					//оптимизация с права до лева
					if (!(flagBreaker & 2))
					{
						for (y = rect_point.h - 1; y >= 0; --y)
						{
							//Формула Y Offset * (Pitch/BytesPerPixel) + X Offset
							cx = rect_point.x + fontData->data[i].w - 1 - x;
							cy = rect_point.y + y;
							pixel = (SDL_Color*)fontData->texture->pixels + cy * (model->pitch / model->format->BytesPerPixel) + cx;
							if (pixel->a) // isn't transparent
								break;
						}
						if (y == -1)
							--rect_point.w;
						else
							flagBreaker |= 2;
					}

					if (flagBreaker == 3)
						break;
				}
				fontData->data[i] = rect_point;

			}
		}
	}

	int Single_TextWidth_WithCyrilic(const string& text, int fontSize)
	{
		int width = 0;
		for (auto iter = begin(text); iter != end(text); ++iter)
			//todo: Учитывать размер шрифта (fontSize)
			width += pFont->data[(unsigned char)*iter].w;
		return width;
	}

	Rect_t Multiline_TextWidth_WithCyrilic(const string& text, int fontSize)
	{
		//todo: вычисление мультистрок
		Rect_t rect;
		throw std::exception();
		return rect;
	}

	void Render_String(SDL_Renderer* renderer, Rect_t rect, const char* text, int len, int fontWidth, TextAlign textAlign, bool textWrap, bool hilight)
	{
		if (len <= 0 || !fontTexture || hilight && !fontTextureHilight)
			return;

		uint8_t temp;
		Rect_t* src;
		uint16_t pos;
		SDL_Rect dst = *(SDL_Rect*)&rect;
		Vector2i fontSize = pFont->fontSize + Vector2i(1, 1) * (fontWidth - FONT_WIDTH);
		SDL_Texture* targetFont = NULL;
		int textWidth = Single_TextWidth_WithCyrilic(text, fontWidth);

		if (!rect.w)
			rect.w = textWidth;
		if (!rect.h)
			rect.h = pFont->fontSize.y; // todo: для мультий строк требуется вычислить h высоту

		targetFont = hilight ? fontTextureHilight : fontTexture;

		//x
		temp = (VH[textAlign] >> 4 & 15);
		if (temp)
		{
			dst.x += rect.w / temp;
			if (temp == 2)
				dst.x += -textWidth / 2;
			else if (temp == 1)
				dst.x += -textWidth;
		}
		//y
		temp = (VH[textAlign] & 15);
		if (temp)
		{
			dst.y += rect.h / temp - fontSize.y / 2;
			if (temp == 1)
				dst.y += -textWidth / 2;
		}

		Vector2i begin = *(Vector2i*)&dst;
		int deltax;
		for (pos = 0; pos < len; ++pos)
		{
			temp = *(uint8_t*)(text + pos);
			src = (pFont->data + temp);
			if (temp != '\n')
			{
				dst.w = src->w;
				dst.h = src->h;
				deltax = rect.x + rect.w;

				if (dst.x >= deltax)
				{
					//todo: закончить до \n символа и выйти с этого блока кода. Это нужно для оптимизаций. Дальнейшая вычисление бессмыслена.
					for (++pos; pos < len;)
					{
						temp = *(uint8_t*)(text + pos);
						if (temp != '\n')
							++pos;
						else
							break;
					}
					continue;
				}

				//отрисовываем остаток входящую в область 
				dst.w = Mathf::Max(0, Mathf::Min(deltax - dst.x, dst.w));
				//if (dst.x <= src.x + src.w && dst.y <= src.y + src.h)
				SDL_RenderCopy(renderer, targetFont, (SDL_Rect*)src, &dst);
				dst.x += src->w;
			}
			else
			{
				dst.x = begin.x;
				dst.y += src->h;
			}
		}
	}
}

