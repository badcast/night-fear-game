#pragma once

#include "framework.h"

using namespace std;
namespace RoninEngine::ui
{
	class UC;
	class GUI;

	void InitalizeControls();
	void Free_Controls();
	void ResetControls();

	typedef void(*ui_callback)(const uint8_t& id, void* userdata);

	struct RenderData
	{
		Rect_t rect;
		uint8_t options;
		int8_t id;
		int8_t parentId;
		string text;
		void* resources;
		UC* prototype;
		list<uint8_t> childs;
	};

	typedef void(*ui_render_callback)(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);

	list<UC*> nativeControls();

	UC* findControl(const char* name);
	
	class UC
	{
	public: 
		UC();
		virtual const char* class_name() { return "UC"; }
		virtual bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		virtual void destructor(void* resources);
	};
	 
	class CText : public UC
	{
	public:
		const char* class_name() { return "text"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover); 
		void destructor(void* resources);
	};

	class CButton : public UC
	{
	public: 
		const char* class_name() { return "button"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};

	class CEdit : public UC
	{
	public:
		const char* class_name() { return "edit"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};

	class CHorizontalSlider : public UC
	{
	public:
		const char* class_name() { return "slider"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};

	class CVerticalSlider : public UC
	{
	public:
		const char* class_name() { return "vslider"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};

	class CImage : public UC
	{
	public:
		const char* class_name() { return "image"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};

	class CImageAnimator : public UC
	{
	public:
		const char* class_name() { return "textureAnimator"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};

	class CTextRandomizerDisplay : public UC
	{
	public:
		const char* class_name() { return "textRandom"; }
		bool __stdcall render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool *hover);
		void destructor(void* resources);
	};
}