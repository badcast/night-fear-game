#pragma once
#pragma once

#include "framework.h"
#include "timeline.h"
#include "controls.h"

using namespace std;
using namespace RoninEngine;

namespace RoninEngine::UI
{
#define CI
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

        CI ui_callback callback;
        CI void* callbackData;
		Scene* m_scene;
		bool hitCast;
		bool _focusedUI;

        ///Регистрирует хранилище для пользовательских инструментов и возвращает id
        CI list<UIID> get_groups();
        CI UIID register_ui(const UIID& parent = NOPARENT)throw();
        CI RenderData& ID(const UIID& id);
        CI bool has_action(void* outPos);
	public:
		bool visible;

        CI SDL_Renderer* renderer;

        CI GUI(Scene* m_scene);
        CI virtual ~GUI();

        CI bool Has_ID(const UIID& id);

        CI UIID Create_Group(const Rect_t& rect);
        CI UIID Create_Group();

        CI UIID Push_Label(const string& text, const Rect_t& rect, const int& fontWidth = 13, const UIID& parent = NOPARENT);
        CI UIID Push_Label(const string& text, const Vector2i& point, const int& fontWidth = 13, const UIID& parent = NOPARENT);
        CI UIID Push_Button(const string& text, const Rect_t& rect, const UIID& parent = NOPARENT);
        CI UIID Push_Button(const string& text, const Vector2i point, const UIID& parent = NOPARENT);
        CI UIID Push_DisplayRandomizer(TextRandomizer_Format format, const Vector2i& point, const UIID& parent = NOPARENT);
        CI UIID Push_DisplayRandomizer(TextRandomizer_Format format = TextRandomizer_Format::All, const UIID& parent = NOPARENT);
        CI UIID Push_DisplayRanomizer_Text(const string& text, const Vector2i& point, const UIID& parent = 0);
        CI UIID Push_DisplayRanomizer_Number(const int&min, const int& max, TextAlign textAlign, const UIID& parent = NOPARENT);
        CI UIID Push_TextureStick(Texture* texture, const Rect_t& rect, const UIID& parent = NOPARENT);
        CI UIID Push_TextureStick(Texture* texture, const Vector2i point, const UIID& parent = NOPARENT);
        CI UIID Push_TextureAnimator(Timeline* timeline, const Rect_t& rect, const UIID& parent = NOPARENT);
        CI UIID Push_TextureAnimator(Timeline* timeline, const Vector2i& point, const UIID& parent = NOPARENT);
        CI UIID Push_TextureAnimator(const list<Texture*>& roads, float duration, TimelineOptions option, const Rect_t& rect, const UIID& parent = NOPARENT);
        CI UIID Push_TextureAnimator(const list<Texture*>& roads, float duration, TimelineOptions option, const Vector2i& point, const UIID& parent = NOPARENT);

		//property-----------------------------------------------------------------------------------------------------------

        CI void* Resources(const UIID& id);
        CI void Resources(const UIID& id, void* data);

        CI Rect_t Rect(const UIID& id);
        CI void Rect(const UIID& id, const Rect_t& rect);

        CI string Text(const UIID& id);
        CI void Text(const UIID& id, const string& text);

        CI void Visible(const UIID& id, bool state);
        CI bool Visible(const UIID& id);

        CI void Enable(const UIID& id, bool state);
        CI bool Enable(const UIID& id);

		//grouping-----------------------------------------------------------------------------------------------------------

        CI bool Is_Group(const UIID& id);
        CI void Show_GroupUnique(const UIID& id) throw();
        CI void Show_Group(const UIID& id) throw();
        CI bool Close_Group(const UIID& id) throw();

		//other--------------------------------------------------------------------------------------------------------------

        CI void Cast(bool state);
        CI bool Cast();

        CI void Register_Callback(ui_callback callback, void* userData);

        CI bool Remove(const UIID& id);
        CI void RemoveAll();

        CI void Do_Present(SDL_Renderer* renderer);

        CI void GUI_SetMainColorRGB(uint32_t RGB);
        CI void GUI_SetMainColorRGBA(uint32_t ARGB);

        CI bool Focused_UI();
	};

	extern GUI* guiInstance;

#undef CI
}
