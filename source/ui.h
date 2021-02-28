#pragma once
#pragma once

#include "framework.h"
#include "timeline.h"
#include "controls.h"

using namespace std;
using namespace RoninEngine;

namespace RoninEngine::ui
{
#define UI
    //Тип идентификатора GUI
#define UIID uint8_t
#define NOPARENT 0

	enum TextRandomizer_Format
	{
        All = -1,
		OnlyText = 0,
		OnlyNumber = 1,
		OnlyReal = 3,
		MaskText = 2,
		MaskNumber = 4,
		MaskReal = 8,
		MaskUpperChar = 16,
		MaskLowwerChar = 32,
	};

	struct RenderData;

	class GUI final
	{
	private:

		struct
		{
			//controls
			vector<RenderData> controls;
			list<UIID> _rendering;
		}m_Sticks;

		UI ui_callback callback;
		UI void* callbackData;
		Scene* m_scene;
		bool hitCast;
		bool _focusedUI;

		//Регистрирует хранилище для пользовательских инструментов и возвращает id 
		UI list<UIID> get_groups();
		UI UIID register_ui(const UIID& parent = NOPARENT)throw();
        UI RenderData& ID(const UIID& id);
		UI bool has_action(void* outPos);
	public:
		bool visible;

		UI SDL_Renderer* renderer;

		UI GUI(Scene* m_scene);
		UI virtual ~GUI();

		UI bool Has_ID(const UIID& id);

		UI UIID Create_Group(const Rect_t& rect);
		UI UIID Create_Group();

		UI UIID Push_Label(const string& text, const Rect_t& rect, const int& fontWidth = 13, const UIID& parent = NOPARENT);
		UI UIID Push_Label(const string& text, const Vector2i& point, const int& fontWidth = 13, const UIID& parent = NOPARENT);
		UI UIID Push_Button(const string& text, const Rect_t& rect, const UIID& parent = NOPARENT);
		UI UIID Push_Button(const string& text, const Vector2i point, const UIID& parent = NOPARENT);
		UI UIID Push_DisplayRandomizer(TextRandomizer_Format format, const Vector2i& point, const UIID& parent = NOPARENT);
		UI UIID Push_DisplayRandomizer(TextRandomizer_Format format = TextRandomizer_Format::All, const UIID& parent = NOPARENT);
		UI UIID Push_DisplayRanomizer_Text(const string& text, const Vector2i& point, const UIID& parent = 0);
		UI UIID Push_DisplayRanomizer_Number(const int&min, const int& max, TextAlign textAlign, const UIID& parent = NOPARENT);
		UI UIID Push_TextureStick(Texture* texture, const Rect_t& rect, const UIID& parent = NOPARENT);
		UI UIID Push_TextureStick(Texture* texture, const Vector2i point, const UIID& parent = NOPARENT);
		UI UIID Push_TextureAnimator(Timeline* timeline, const Rect_t& rect, const UIID& parent = NOPARENT);
		UI UIID Push_TextureAnimator(Timeline* timeline, const Vector2i& point, const UIID& parent = NOPARENT);
		UI UIID Push_TextureAnimator(const list<Texture*>& roads, float duration, TimelineOptions option, const Rect_t& rect, const UIID& parent = NOPARENT);
		UI UIID Push_TextureAnimator(const list<Texture*>& roads, float duration, TimelineOptions option, const Vector2i& point, const UIID& parent = NOPARENT);

		//property-----------------------------------------------------------------------------------------------------------

		UI void* Resources(const UIID& id);
		UI void Resources(const UIID& id, void* data);

		UI Rect_t Rect(const UIID& id);
		UI void Rect(const UIID& id, const Rect_t& rect);

		UI string Text(const UIID& id);
		UI void Text(const UIID& id, const string& text);

		UI void Visible(const UIID& id, bool state);
		UI bool Visible(const UIID& id);

		UI void Enable(const UIID& id, bool state);
		UI bool Enable(const UIID& id);

		//grouping-----------------------------------------------------------------------------------------------------------

		UI bool Is_Group(const UIID& id);
		UI void Show_GroupUnique(const UIID& id) throw();
		UI void Show_Group(const UIID& id) throw();
		UI bool Close_Group(const UIID& id) throw();

		//other--------------------------------------------------------------------------------------------------------------

		UI void Cast(bool state);
		UI bool Cast();

		UI void Register_Callback(ui_callback callback, void* userData);

		UI bool Remove(const UIID& id);
		UI void RemoveAll();

		UI void Do_Present(SDL_Renderer* renderer);

		UI void GUI_SetMainColorRGB(uint32_t RGB);
		UI void GUI_SetMainColorRGBA(uint32_t ARGB);

		UI bool Focused_UI();
	};

	extern GUI* guiInstance;

#undef UI
}
