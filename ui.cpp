#include "pch.h"
#include "inputSystem.h"

constexpr int _mEnabled = 1;
constexpr int _mVisibled = 2;
constexpr int _mGroup = 4;

namespace RoninEngine::ui
{
	GUI* guiInstance;

	GUI::GUI(Scene* m_scene)
	{
		this->m_scene = m_scene;
		hitCast = true;
		renderer = NULL;
		guiInstance = this;
		callback = NULL;
		callbackData = NULL;
		visible = true;
		_focusedUI = false;
	}

	GUI::~GUI()
	{

	}

	//private--------------------------------------

	list<UIID> GUI::get_groups()
	{
		list<UIID> __;

		Foreach(m_Sticks.controls, [&__, this](auto& iter)
		{
			if (this->Is_Group(iter.id))
				__.push_back(iter.id);
		});

		return __;
	}

	UIID GUI::register_ui(const UIID& parent) throw()
	{
		if (parent && !Has_ID(parent))
			throw exception("Is not ed parent");

		RenderData data{ 0 };
		data.parentId = parent;
		data.options = _mVisibled | _mEnabled;
		m_Sticks.controls.emplace_back(data);
		data.id = m_Sticks.controls.size();
		if (parent)
			//add the child
			m_Sticks.controls[parent - 1].childs.emplace_back(m_Sticks.controls.size() - 1);
		else
			m_Sticks._rendering.push_back(data.id);

		return data.id;
	}

	RenderData& GUI::ID(const UIID& id)
	{
		return m_Sticks.controls[id - 1];
	}

	//public---------------------------------------

	bool GUI::Has_ID(const UIID& id)
	{
		return m_Sticks.controls.size() >= id;
	}
	UIID GUI::Create_Group(const Rect_t& rect) {
		UIID id = register_ui();
		auto& data = ID(id);
		data.rect = rect;
		data.options |= _mGroup;
		return id;
	}
	UIID GUI::Create_Group() {
		return Create_Group(Rect_t::zero());
	}

	UIID GUI::Push_Label(const string& text, const Rect_t& rect, const int& fontWidth, const UIID& parent) {
		//todo: fontWidth 
		UIID id = register_ui(parent);
		auto& data = ID(id);
		data.text = text;
		data.rect = rect;
		data.prototype = findControl("text");
		return id;
	}
	UIID GUI::Push_Label(const string& text, const Vector2i& point, const int& fontWidth, const UIID& parent) {
		return Push_Label(text, { point.x, point.y, 0, 0 }, fontWidth, parent);
	}

	UIID GUI::Push_Button(const string& text, const Rect_t& rect, const UIID& parent) {
		int id = register_ui(parent);
		auto& data = ID(id);
		data.prototype = findControl("button");
		data.rect = rect;
		data.text = text;
		return id;
	}
	UIID GUI::Push_Button(const string& text, const Vector2i point, const UIID& parent)
	{
		return Push_Button(text, { point.x, point.y, 256, 32 }, parent);
	}

	UIID GUI::Push_DisplayRandomizer(TextRandomizer_Format format, const Vector2i& point, const UIID& parent) {
		UIID id = register_ui(parent);

		auto& data = ID(id);

		data.prototype = findControl("textRandom");
		data.resources = (void*)format;
		return id;
	}
	UIID GUI::Push_DisplayRandomizer(TextRandomizer_Format format, const UIID& parent) {
		return Push_DisplayRandomizer(format, Vector2i(numeric_limits<int>::max(), numeric_limits<int>::max()), parent);
	}
	UIID GUI::Push_DisplayRanomizer_Text(const string& text, const Vector2i& point, const UIID& parent) {
		UIID id = Push_DisplayRandomizer(TextRandomizer_Format::OnlyText, point, parent);
		auto& data = ID(id);

		return id;
	}
	UIID GUI::Push_DisplayRanomizer_Number(const int& min, const int& max, TextAlign textAlign, const UIID& parent) {
		UIID id = Push_DisplayRandomizer(OnlyNumber, parent);
		return id;
	}

	UIID GUI::Push_TextureStick(Texture* texture, const Rect_t& rect, const UIID& parent)
	{
		UIID id = register_ui(parent);

		auto& data = ID(id);
		data.prototype = findControl("image");
		data.rect = rect;
		data.resources = texture;
		return id;
	}
	UIID GUI::Push_TextureStick(Texture* texture, const Vector2i point, const UIID& parent)
	{
		return Push_TextureStick(texture, { point.x, point.y, texture->width(), texture->height() }, parent);
	}
	UIID GUI::Push_TextureAnimator(Timeline* timeline, const Rect_t& rect, const UIID& parent)
	{
		UIID id = register_ui(parent);

		auto& data = ID(id);
		data.prototype = findControl("image");
		data.rect = rect;
		data.resources = timeline;
		return id;
	}
	UIID GUI::Push_TextureAnimator(Timeline* timeline, const Vector2i& point, const UIID& parent) {
		return Push_TextureAnimator(timeline, { point.x, point.y, 0, 0 }, parent);
	}
	UIID GUI::Push_TextureAnimator(const list<Texture*>& roads, float duration, TimelineOptions option, const Rect_t& rect, const UIID& parent)
	{
		Timeline* timeline = NULL;
		UIID id = register_ui(parent);
		auto& data = ID(id);
		data.prototype = findControl("textureAnimator");
		data.rect = rect;
		allocate_variable(timeline);
		timeline->SetOptions(option);
		ResourceManager::GoMemoryCache(timeline);

		//todo: wBest и hBest - вычисляется даже когда rect.w != 0 
		int wBest = 0;
		int hBest = 0;
		for (auto i = begin(roads); i != end(roads); ++i)
		{
			timeline->AddRoad(*i, duration);
			if (wBest < (*i)->width())
				wBest = (*i)->width();
			if (hBest < (*i)->height())
				hBest = (*i)->height();
		}
		if (!data.rect.w)
			data.rect.w = wBest;
		if (!data.rect.h)
			data.rect.h = hBest;
		//can destroy
		data.resources = timeline;
		return id;
	}
	UIID GUI::Push_TextureAnimator(const list<Texture*>& roads, float duration, TimelineOptions option, const Vector2i& point, const UIID& parent) {
		return Push_TextureAnimator(roads, duration, option, { point.x, point.y, 0, 0 }, parent);
	}

	void* GUI::Resources(const UIID& id) {
		return ID(id).resources;
	}
	void GUI::Resources(const UIID& id, void* data) {
		ID(id).resources = data;
	}
	Rect_t GUI::Rect(const UIID& id)
	{
		return ID(id).rect;
	}
	void GUI::Rect(const UIID& id, const Rect_t& rect)
	{
		ID(id).rect = rect;
	}
	string GUI::Text(const UIID& id)
	{
		return ID(id).text;
	}
	void GUI::Text(const UIID& id, const string& text)
	{
		ID(id).text = text;
	}

	void GUI::Visible(const UIID& id, bool state) {
		ID(id).options = ID(id).options & (~_mVisibled) | (_mVisibled * state);
	}
	bool GUI::Visible(const UIID& id) {
		return (ID(id).options & _mVisibled) >> 1;
	}

	void GUI::Enable(const UIID& id, bool state) {
		ID(id).options = ID(id).options & (~_mEnabled) | (_mEnabled * state);
	}
	bool GUI::Enable(const UIID& id) {
		return ID(id).options & _mEnabled;
	}

	//grouping-----------------------------------------------------------------------------------------------------------

	bool GUI::Is_Group(const UIID& id) {
		return ID(id).options & _mGroup;
	}

	void GUI::Show_GroupUnique(const UIID& id) throw() {
		if (!Is_Group(id))
			throw exception("Is't group");

		this->m_Sticks._rendering.remove_if([this](auto v)
		{
			return this->Is_Group(v);
		});

		Show_Group(id);
	}
	void GUI::Show_Group(const UIID& id) throw() {
		if (!Is_Group(id))
			throw exception("Is't group");

		auto iter = find_if(begin(m_Sticks._rendering), end(m_Sticks._rendering), [&id](auto& _id)
		{
			return _id == id;
		});

		if (iter == end(m_Sticks._rendering))
		{
			m_Sticks._rendering.emplace_back(id);
			Visible(id, true);
		}

	}

	bool GUI::Close_Group(const UIID& id) throw() {
		if (!Is_Group(id))
			throw exception("Is't group");
		m_Sticks._rendering.remove(id);
		Visible(id, false);
	}

	void GUI::Cast(bool state) {
		hitCast = state;
	}
	bool GUI::Cast() {
		return hitCast;
	}

	void GUI::Register_Callback(ui_callback callback, void* userData) {
		this->callback = callback;
		this->callbackData = userData;
	}
	bool GUI::Remove(const UIID& id) {
		//todo: мда. Тут проблема. ID которое удаляется может задеть так же и другие. Нужно исправить и найти способ! T``T
		return false;
	}
	void GUI::RemoveAll() {

		m_Sticks.controls.clear();
	}
	void GUI::Do_Present(SDL_Renderer* renderer)
	{
		if (visible)
		{
			UIID id;
			RenderData* x;
			list<UIID> _render;
			ResetControls(); // Reset
			bool targetFocus = false;
			_focusedUI = false;
			for (auto iter = begin(m_Sticks._rendering); iter != end(m_Sticks._rendering); ++iter)
				_render.emplace_back(*iter);

			while (_render.size())
			{
				id = _render.front();
				x = &ID(id);

				_render.pop_front();
				if (!(x->options & _mGroup) && x->options & _mVisibled && x->prototype)
				{
					if (x->prototype->render_control(this, *x, renderer, &targetFocus) && hitCast)
					{
						_focusedUI = true;
						//Избавляемся от перекликов в UI
						input::event->type = SDL_FIRSTEVENT;

						if (x->options & _mEnabled && callback)
						{
							//Отправка сообщения о действий.
							callback(id, callbackData);
						}
					}
					else //disabled state
						;

					if (!_focusedUI)
						_focusedUI = targetFocus;
				}

				if (this->m_scene->m_isUnload)
					break;

				for (auto iter = begin(x->childs); iter != end(x->childs); ++iter)
					_render.emplace_back(*iter + 1);
			}
		}
	}
	void GUI::GUI_SetMainColorRGB(uint32_t RGB)
	{
		GUI_SetMainColorRGBA(RGB << 8 | SDL_ALPHA_OPAQUE);
	}
	void GUI::GUI_SetMainColorRGBA(uint32_t ARGB)
	{
		SDL_SetRenderDrawColor(renderer, (UIID)(ARGB >> 24) & 0xFF,
			(UIID)(ARGB >> 16) & 0xFF,
			(UIID)(ARGB >> 8) & 0xFF,
			(UIID)ARGB & 0xFF);
	}
	bool GUI::Focused_UI() {
		return _focusedUI;
	}
}