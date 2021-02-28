#include "pch.h"
#include "inputSystem.h"

namespace RoninEngine::ui
{
	std::list<UC*> m_controls;
	std::list<UC*> nativeControls()
	{
		return m_controls;
	}

	uint32_t _controlId;
	uint32_t _focusedId;

	void InitalizeControls()
	{
		m_controls =
		{
						allocate_class<CText>(),
						allocate_class<CEdit>(),
						allocate_class<CButton>(),
						allocate_class<CVerticalSlider>(),
						allocate_class<CImage>(),
						allocate_class<CImageAnimator>(),
						allocate_class<CTextRandomizerDisplay>()
		};

		for (auto i : m_controls) {
			if (i == NULL)
				RoninApplication::instance()->fail_OutOfMemory();
		}
	}

	void Free_Controls()
	{
		Foreach(m_controls, [](auto& f)
		{
			deallocate_variable(f);
		});
		m_controls.clear();
	}

	UC* findControl(const char* name)
	{
		auto iter = find_if(begin(m_controls), end(m_controls), [name](UC* ref)
		{
			return !SDL_strcmp(ref->class_name(), name);
		});

		if (iter == end(m_controls))
			return NULL;

		return *iter;
	}

	void ResetControls()
	{
		_controlId = 0;
	}

	UC::UC()
	{
	}


	bool UC::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{
		return false;
	}

	bool CText::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{
		Render_String(render, data.rect, data.text.c_str(), data.text.size());
		return false;
	}

	bool CButton::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{
		static uint8_t pSize = 2; // pen size
		static Rect_t inside = Rect_t(pSize, pSize, -pSize * 2, -pSize * 2);
		auto ms = input::getMousePoint();
		Rect_t rect;
		*hover = SDL_PointInRect((SDL_Point*)&ms, (SDL_Rect*)&data.rect);
		//border
		guiInstance->GUI_SetMainColorRGB(0xa75100);
		SDL_RenderDrawRect(render, (SDL_Rect*)&data.rect);
		//fill
		guiInstance->GUI_SetMainColorRGB(*hover ? input::isMouseDown() ? 0xab0000 : 0xe70000 : 0xc90000);
		rect = data.rect;
		rect += inside / 2;
		SDL_RenderFillRect(render, (SDL_Rect*)&rect);

		//render text
		Render_String(render, data.rect, data.text.c_str(), data.text.size(), 13, TextAlign::MiddleCenter, true, *hover);
		bool msClick = input::isMouseUp();
		return (*hover) && msClick;
	}

	bool CEdit::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{
		return false;
	}

	bool CHorizontalSlider::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{
		float* value = (float*)data.resources;
		//Minimal support


		return false;
	}

	bool CVerticalSlider::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{

		return false;
	}

	bool CImage::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{

		SDL_RenderCopy(render, (SDL_Texture*)data.resources, NULL, (SDL_Rect*)&data.rect);
		return false;
	}

	bool CImageAnimator::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover) {
		Timeline* timeline = (Timeline*)data.resources;
		Texture* texture = timeline->Evaluate(Time::time())->texture;
		SDL_RenderCopy(render, texture->native(), NULL, (SDL_Rect*)&data.rect);
		return false;
	}

	bool CTextRandomizerDisplay::render_control(GUI* gui, RenderData& data, SDL_Renderer* render, bool* hover)
	{
		//todo: Доработать рандомайзера!
		TextRandomizer_Format format = TextRandomizer_Format::OnlyNumber;// (int)(data.resources);

		data.text.resize(15);
		size_t i;

		static float lasttime = 0;
		if (Time::time() > lasttime)
		{
			lasttime = Time::time() + 0.1f;
			switch (format)
			{
			case TextRandomizer_Format::OnlyNumber:
				for (i = 0; i < data.text.length(); ++i)
				{
					data.text[i] = '0' + (rand() % ('9'));
				}
				break;
			case TextRandomizer_Format::All:
			default:
				for (i = 0; i < data.text.length(); ++i)
				{
					data.text[i] = 32 + rand() % 128;
				}
				break;
			}
		}

		Render_String(gui->renderer, data.rect, data.text.c_str(), data.text.size());

		return false;
	}

	//***************** destructors

	void UC::destructor(void* resources) {

	}
	void CText::destructor(void* resources) {

	}
	void CButton::destructor(void* resources) {

	}
	void CEdit::destructor(void* resources) {

	}
	void CHorizontalSlider::destructor(void* resources) {

	}
	void CVerticalSlider::destructor(void* resources) {

	}
	void CImage::destructor(void* resources) {

	}
	void CImageAnimator::destructor(void* resources) {

	}
	void CTextRandomizerDisplay::destructor(void* resources) {

	}

}