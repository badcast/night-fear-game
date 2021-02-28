#include <tuple>
#include "pch.h"

#include "GameObject.h"
#include "Camera2D.h"
#include "SpriteRenderer.h"
#include "Sprite.h"
#include "inputSystem.h"
#include "ObjectParser.h"
#include "Player.h"
#include "Gizmos.h"
#include "NavMesh.h"

using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;

namespace RoninEngine
{
	constexpr int MaxPopulation = 1000;
	constexpr int InitPopuplation = 1000;
	constexpr int NavMeshMagnitude = 100;
	constexpr float NavMeshWorldScale = 0.32;

	uint8_t g_buttons;
	uint8_t b_mainMenu;
	uint8_t b_generateNav;
	uint8_t b_clearNav;
	uint8_t t_infoblock;
	uint8_t t_speedLabel;
	RoninEngine::ui::GUI* gui;
	AIPathFinder::NavMesh* navMesh = NULL;
	list<AIPathFinder::Neuron*> paths;

	Vec2 from, to;
	int arranged = 0;
	static Neuron* last = NULL;
	float changeDrawPointPer;
	decltype(paths.end()) _endIter;

	void ui_event_handler(const uint8_t& id, void* userData) {

		if (b_mainMenu == id)
		{
			RoninApplication::instance()->LoadScene(allocate_class<MainMenu>());
		}
		else if (b_generateNav == id || id == b_clearNav) {
			paths.clear();
			arranged = 0;
			if (b_generateNav == id)
				navMesh->randomGenerate();
			else
				navMesh->clear(true);
		}
	}

	Player* player;
	void GameScene::awake() {
		gui = this->ui;

		auto display = RoninApplication::instance()->display();

		GameObject* playerObj = CreateObject<GameObject>("Player");
		player = playerObj->Add_Component<Player>();
		g_buttons = ui->Create_Group();
		b_mainMenu = this->ui->Push_Button("Назад в главное меню", { 25,25 }, g_buttons);
		b_generateNav = this->ui->Push_Button("Генерировать Nav Mesh", { 300, 25 }, g_buttons);
		b_clearNav = this->ui->Push_Button("Очистить Nav Mesh", { 550, 25 }, g_buttons);
		t_infoblock = this->ui->Push_Label("", { display.w / 2 - 130, display.h / 2 + 25 }, 5);
		t_speedLabel = this->ui->Push_Label("", { 0, 70 }, 5);
		gui->Register_Callback(ui_event_handler, NULL);
	}


	SpriteRenderer* target;
	GameObject* testObj;
	vector<tuple<Transform*, float, list<Neuron*>>> ais;
	void GameScene::start() {
		SpriteRenderer* spriteRenderer;
		Sprite* spr;
		GameObject* floor = CreateObject<GameObject>("Floor");

		testObj = CreateObject<GameObject>("Population");
		spriteRenderer = testObj->Add_Component<SpriteRenderer>();
		spriteRenderer->renderType = SpriteRenderType::Simple;
		spriteRenderer->tileRenderPresent = SpriteRenderTile::Fixed;
		spriteRenderer->flip = { 1, 1 };
		spriteRenderer->setSprite(spr = ResourceManager::GetSprite(ResourceManager::GetTexture("test")));
		testObj->transform()->position(Vec2::zero);
		testObj->Get_Component<Renderer>()->zOrder = 1; // layer

		spriteRenderer = floor->Add_Component<SpriteRenderer>();
		spriteRenderer->renderType = SpriteRenderType::Tile;
		spriteRenderer->tileRenderPresent = SpriteRenderTile::Place;
		spriteRenderer->size = { NavMeshMagnitude, NavMeshMagnitude };
		spriteRenderer->size *= NavMeshWorldScale;
		spriteRenderer->setSprite(spr = allocate_class<Sprite>(ResourceManager::GetTexture("concrete", FolderKind::TEXTURES)));

		allocate_variable(navMesh, NavMeshMagnitude, NavMeshMagnitude);
		navMesh->worldScale = Vec2::one * NavMeshWorldScale;

		//navMesh->randomGenerate();
		//auto a = navMesh->neuron(0, 0);
		//a->lock(false);
		//auto b = navMesh->neuron(NavMeshMagnitude - 1, NavMeshMagnitude - 1);
		//b->lock(false);

		//astar.SearchPath(navMesh, NavMethodRule::PlusMethod, a, b, &paths);
		//navMesh->clear();
		//astar.SearchPath(navMesh, NavMethodRule::SquareMethod, a, b, &paths);
		//navMesh->clear();
		//astar.SearchPath(navMesh, NavMethodRule::NavigationIntelegency, a, b, &paths);
		//navMesh->clear();
		
		changeDrawPointPer = 0;
		target = CreateObject<GameObject>()->Add_Component<SpriteRenderer>();
		spr = ResourceManager::GetSprite(ResourceManager::GetTexture("target", FolderKind::GFX));
		target->setSprite(spr);
		target->zOrder = 1;
		list<Neuron*> c;
		for (size_t i = 0; i < InitPopuplation; ++i)
		{
			auto t = Instantiate(testObj)->transform();
			t->position(navMesh->PointToWorldPosition(Random::range(0, NavMeshMagnitude), Random::range(0, NavMeshMagnitude)));
			ais.emplace_back(tuple(t, 0, c));
		}
	}

	void GameScene::update() {
		static bool lastLockVal = false;
		static int activeCount = 0;
		Vec2 p = Camera::mainCamera()->ScreenToWorldPoint(input::getMousePointF());
		Neuron* c = navMesh->neuron(p);
		from = testObj->transform()->position();

		if (input::get_key_down(SDL_SCANCODE_ESCAPE)) {
			if (!ui->Visible(g_buttons))
				ui->Show_Group(g_buttons);
			else
				ui->Close_Group(g_buttons);
		}

		if (Time::frame() % 25 == 0)
		{

			string s = input::get_key(SDL_SCANCODE_LSHIFT) ? "Speed x2" : "Speed x1";
			s.push_back('; ');
			s += "Count ";
			s += to_string(ais.size());
			s += " Active ";
			s += to_string(activeCount);
			gui->Text(t_speedLabel, s);
		}
		if (c && c != last)
		{
			if (last)
				last->lock(lastLockVal);

			lastLockVal = c->locked();
			c->lock(true);
			char p[70];
			p[0] = 0;
			if (c->total())
			{
				strcat(p, "cost: ");
				strcat(p, to_string(c->cost()).data());
				strcat(p, "; ");
				strcat(p, "h: ");
				strcat(p, to_string(c->heuristic()).data());
				strcat(p, "; ");
				ui->Visible(t_infoblock, true);
				ui->Text(t_infoblock, p);
			}
			else
				ui->Visible(t_infoblock, false);

			last = c;
		}
		if (target)
		{
			if (!gui->Focused_UI() && input::isMouseDown()) {

				p = input::getMousePointF();
				p = Camera::mainCamera()->ScreenToWorldPoint(p);
				paths.clear();
				auto a = navMesh->neuron(from);
				a->lock(false);
				auto b = navMesh->neuron(p);
				target->transform()->position(p);
				//navMesh->clear();
				testObj->transform()->position(navMesh->PointToWorldPosition(a));
				if (b)
					b->lock(lastLockVal);
				navMesh->clear();
				auto status = navMesh->find(RoninEngine::AIPathFinder::NavMethodRule::NavigationIntelegency,
					a, b, &paths, RoninEngine::AIPathFinder::NavAlgorithm::AStar).status;
				if (status < AIPathFinder::NavStatus::Closed)
				{
					//todo: error
					arranged = 0;
					paths.clear();
				}
				else {
					arranged = 1;
					_endIter = begin(paths);
				}
				navMesh->clear();
			}

			float ang = target->transform()->angle();

			ang += 1;
			if (ang > 360)
				ang -= 360;

			target->transform()->angle(ang);

		}

		if (!paths.empty())
		{
			int j;
			if (_endIter != std::end(paths) && Time::time() > changeDrawPointPer) {
				changeDrawPointPer = Time::time() + 0.001f;
				for (j = 0; _endIter != std::end(paths) && j < paths.size() / Mathf::sqrt(paths.size()); ++j, ++_endIter);
			}

			Vec2 lp = navMesh->PointToWorldPosition(*begin(paths));
			auto iter = paths.cbegin();
			for (j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
				lp = navMesh->PointToWorldPosition(*iter);
			to = lp;

			if (arranged && _endIter == std::end(paths)) {
				if (from != to)
				{
					testObj->transform()->LookAtLerp(to, 0.5f);
				}
				testObj->transform()->position(from = Vec2::MoveTowards(from, to, input::get_key(SDL_SCANCODE_LSHIFT) ? .2 : input::get_key(SDL_SCANCODE_LCTRL) ? .0005f : .05f));
				if (from == to) {
					if (paths.size() != arranged)
						++arranged;
					else
						arranged = 0;
				}
			}
			else
			{
				//testObj->transform()->LookAt(p);

			}
		}

		activeCount = 0;
		for (auto& iter : ais) {
			auto* trans = get<Transform*>(iter);
			float& t = get<float>(iter);
			list<Neuron*>& rr = get<remove_reference<decltype(rr)>::type>(iter);

			if (t == 0)
				t = 1;

			if (!rr.empty())
			{
				if (trans->position() == navMesh->PointToWorldPosition(rr.back()))
				{

				}
				else {
					Vec2 dir = navMesh->PointToWorldPosition(rr.front());
					if (trans->position() != dir)
						trans->LookAtLerp(dir, 0.5f);
					trans->position(Vec2::MoveTowards(trans->position(), dir, input::get_key(SDL_SCANCODE_LSHIFT) ? 2 : input::get_key(SDL_SCANCODE_LCTRL) ? .005f : .05f));
					if (trans->position() == dir) {
						rr.pop_front();
					}
					++activeCount;
				}
			}
			else
			{
				auto result = navMesh->find(NavMethodRule::NavigationIntelegency,
					navMesh->neuron(trans->position()),
					navMesh->neuron(Random::range(0, NavMeshMagnitude), Random::range(0, NavMeshMagnitude)),
					&rr, NavAlgorithm::AStar);
				if (result.status != NavStatus::Opened)
				{
					float a = trans->angle() + 1;
					if (a > 360)
						a -= 360;
					trans->angle(a);
					rr.clear();
				}
				else
					rr.pop_front();
				navMesh->clear();
			}
		}

		//player->transform()->position(Vec2::MoveTowards(player->transform()->position(), from, 0.085f));
	}

	void GameScene::lateUpdate() {

	}

	void GameScene::onDrawGizmos() {

		//return;
		//draw nav path
		Gizmos::DrawNavMesh(navMesh);

		Gizmos::color.g = 200;
		Gizmos::color.r = 0;
		Gizmos::color.b = 0;

		if (!paths.empty()) {
			auto iter = paths.cbegin();
			int j;

			Vec2 lp = navMesh->PointToWorldPosition(*begin(paths));
			for (j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
			{
				if (lp != navMesh->PointToWorldPosition(*iter))
					Gizmos::DrawLine(lp, navMesh->PointToWorldPosition(*iter));
				lp = navMesh->PointToWorldPosition(*iter);
			}
			to = lp;

			Gizmos::color.b = 200;
			for (; iter != _endIter; ++iter)
			{
				Gizmos::DrawLine(lp, navMesh->PointToWorldPosition(*iter));
				lp = navMesh->PointToWorldPosition(*iter);
			}
		}


		//Gizmos::DrawLine(testObj->transform()->position(), testObj->transform()->position() + testObj->transform()->forward() * 2);
	}

	void GameScene::onUnloading() {
		testObj = NULL;
		to = Vec2::zero;
		last = NULL;
		deallocate_variable(navMesh);
		paths.clear();
		ais.clear();
		ais.shrink_to_fit();
	}
}