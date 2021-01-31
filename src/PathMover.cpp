#include <cstring>

#include "PathMover.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::UI;
using namespace RoninEngine::AI;

constexpr int MaxPopulation = 1000;
constexpr int InitPopuplation = 1000;
constexpr int NavMeshMagnitude = 250;
constexpr float NavMeshWorldScale = 0.32;

uid g_buttons;
uid b_mainMenu;
uid b_generateNav;
uid b_clearNav;
uid b_showGizmo;
uid b_add, b_sub;
uid t_infoblock;
uid t_speedLabel;

NavMesh *navMesh = nullptr;
std::deque<Vec2Int> paths;

Vec2 from, to;
bool showGizmosLayer = false;
int arranged = 0;
Neuron *last = nullptr;
float changeDrawPointPer;
decltype(paths.end()) _endIter;
MoveController2D *player;

void ui_event_handler(uid id, void *userData)
{
    if(b_mainMenu == id)
    {
        RoninSimulator::load_world(RoninMemory::alloc<PathMoverMenu>());
    }
    else if(b_generateNav == id || id == b_clearNav)
    {
        paths.clear();
        arranged = 0;
        if(b_generateNav == id)
            navMesh->randomize();
        else
            navMesh->clear(true);
    }
    else if(b_showGizmo == id)
    {
        showGizmosLayer = !showGizmosLayer;
    }
    else if(b_add == id)
    {
        // player->playerCamera->sca += Vec2::half / 2;
    }
    else if(b_sub == id)
    {
        // player->playerCamera->aspectRatio -= Vec2::half / 2;
    }
}

void PathMover::on_awake()
{
    Resolution res = RoninSimulator::get_current_resolution();

    GameObject *playerObj = create_game_object("Player");
    player = playerObj->add_component<MoveController2D>();

    player->playerCamera->visibleBorders = true;
    player->playerCamera->visibleGrids = true;

    g_buttons = get_gui()->push_group();
    b_mainMenu = get_gui()->push_button("Main Menu", Vec2Int(25, 25), nullptr, g_buttons);
    b_generateNav = get_gui()->push_button("generate Nav Mesh", Vec2Int(300, 25), nullptr, g_buttons);
    b_clearNav = get_gui()->push_button("flush Nav Mesh", Vec2Int(550, 25), nullptr, g_buttons);
    b_showGizmo = get_gui()->push_button("show GIZMOS", Vec2Int(25, 90), nullptr, g_buttons);

    b_add = get_gui()->push_button("+", {res.width - 32, res.height - 64, 32, 32});
    b_sub = get_gui()->push_button("-", {res.width - 32, res.height - 32, 32, 32});

    t_infoblock = get_gui()->push_label("", Vec2Int(res.width / 2 - 130, res.height / 2 + 25), 5);
    t_speedLabel = get_gui()->push_label("", Vec2Int(0, 70), 5);
    get_gui()->register_general_callback(ui_event_handler, nullptr);
}

SpriteRenderer *target;
GameObject *testObj;
std::vector<std::tuple<Transform *, float, std::list<Vec2Int>>> wrappers;
void PathMover::on_start()
{
    SpriteRenderer *spriteRenderer;
    Sprite *test_sprite, *floor_sprite, *target_sprite;
    Sprite *spr;

    const std::string sprite_dir = "./data/gfx/";
    test_sprite = Primitive::create_sprite2d_from(Resources::get_surface(Resources::load_surface(sprite_dir + "TestSprite.png", true)));
    floor_sprite = Primitive::create_sprite2d_from(Resources::get_surface(Resources::load_surface(sprite_dir + "floor.png", true)));
    target_sprite = Primitive::create_sprite2d_from(Resources::get_surface(Resources::load_surface(sprite_dir + "target.png", true)));

    testObj = create_game_object("Population");
    testObj->transform()->position(Vec2::zero);
    testObj->transform()->layer = 1; // layer

    spriteRenderer = testObj->add_component<SpriteRenderer>();
    spriteRenderer->renderType = SpriteRenderType::Simple;
    spriteRenderer->renderPresentMode = SpriteRenderPresentMode::Fixed;
    spriteRenderer->flip = {1, 1};
    spriteRenderer->set_sprite(test_sprite);

    GameObject *floor = create_game_object("Floor");
    spriteRenderer = floor->add_component<SpriteRenderer>();
    spriteRenderer->renderType = SpriteRenderType::Tile;
    spriteRenderer->renderPresentMode = SpriteRenderPresentMode::Fixed;
    spriteRenderer->size = {NavMeshMagnitude, NavMeshMagnitude};
    spriteRenderer->size *= NavMeshWorldScale;
    spriteRenderer->set_sprite(floor_sprite);

    RoninMemory::alloc_self(navMesh, static_cast<std::size_t>(NavMeshMagnitude), static_cast<std::size_t>(NavMeshMagnitude));
    navMesh->worldScale = Vec2::one * NavMeshWorldScale;

    changeDrawPointPer = 0;
    target = create_game_object()->add_component<SpriteRenderer>();
    target->transform()->layer = 1;
    target->set_sprite(target_sprite);

    std::list<Vec2Int> points;
    for(size_t i = 0; i < Math::min(MaxPopulation, InitPopuplation); ++i)
    {
        auto t = instantiate(testObj)->transform();
        t->position(navMesh->point_to_world_position(Random::range(0, NavMeshMagnitude), Random::range(0, NavMeshMagnitude)));

        wrappers.emplace_back(std::make_tuple(t, 0, points));
    }
}

void PathMover::on_update()
{

    static bool lastLockVal = false;
    static int activeCount = 0;
    if(Camera::main_camera() == nullptr)
    {
        return;
    }

    Vec2 p = Camera::main_camera()->screen_to_world(Input::get_mouse_pointf());
    Neuron *selNeuron = navMesh->get_neuron(p);
    from = testObj->transform()->position();

    if(Input::key_down(KB_ESCAPE))
    {
        if(!get_gui()->get_visible(g_buttons))
            get_gui()->show_group(g_buttons);
        else
            get_gui()->close_group(g_buttons);
    }

    if(TimeEngine::frame() % 25 == 0)
    {
        std::string s = Input::key_down(KB_LSHIFT) ? "Speed x2" : "Speed x1";
        s.push_back(';');
        s += "Count ";
        s += std::to_string(wrappers.size());
        s += " Active ";
        s += std::to_string(activeCount);
        get_gui()->set_text(t_speedLabel, s);
    }
    if(selNeuron && selNeuron != last)
    {
        if(last)
            navMesh->get_nlock(last, lastLockVal);

        lastLockVal = navMesh->get_nlocked(selNeuron);
        navMesh->get_nlock(selNeuron, true);
        char p[70];
        p[0] = 0;
        if(navMesh->get_ntotal(selNeuron))
        {
            std::strcat(p, "cost: ");
            std::strcat(p, std::to_string(navMesh->get_ncost(selNeuron)).data());
            std::strcat(p, "; ");
            std::strcat(p, "h: ");
            std::strcat(p, std::to_string(navMesh->get_nheuristic(selNeuron)).data());
            std::strcat(p, "; ");
            get_gui()->set_visible(t_infoblock, true);
            get_gui()->set_text(t_infoblock, p);
        }
        else
            get_gui()->set_visible(t_infoblock, false);

        last = selNeuron;
    }
    if(target)
    {
        if(!get_gui()->has_focused_ui() && Input::is_mouse_down())
        {
            p = Input::get_mouse_pointf();
            p = Camera::main_camera()->screen_to_world(p);
            paths.clear();
            auto a = navMesh->get_neuron(from);
            navMesh->get_nlock(a, false);
            auto b = navMesh->get_neuron(p);

            testObj->transform()->position(navMesh->point_to_world_position(a));

            if(b)
            {
                navMesh->get_nlock(b, lastLockVal);
                target->transform()->position(navMesh->point_to_world_position(b));
            }

            navMesh->clear();

            NavResult result;
            navMesh->find(result, NavMethodRule::NavigationIntelegency, navMesh->get_npoint(a), navMesh->get_npoint(b));
            paths = result.roads;
            if(result.status < NavStatus::Closed)
            {
                // todo: error
                arranged = 0;
                paths.clear();
            }
            else
            {
                arranged = 1;
                _endIter = begin(paths);
            }
            navMesh->clear();
        }

        float ang = target->transform()->angle();

        ang += 11;
        if(ang > 360)
            ang -= 360;

        target->transform()->angle(ang);
    }

    if(!paths.empty())
    {
        int j;
        if(_endIter != std::end(paths) && TimeEngine::time() > changeDrawPointPer)
        {
            changeDrawPointPer = TimeEngine::time() + 0.001f;
            for(j = 0; _endIter != std::end(paths) && j < paths.size() / Math::sqrt(paths.size()); ++j, ++_endIter)
                ;
        }

        Vec2 lp = navMesh->point_to_world_position(*begin(paths));
        auto iter = paths.cbegin();
        for(j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
            lp = navMesh->point_to_world_position(*iter);
        to = lp;

        if(arranged && _endIter == std::end(paths))
        {
            if(from != to)
            {
                testObj->transform()->look_at_lerp(to, 0.5f);
            }
            testObj->transform()->position(
                from = Vec2::move_towards(
                    from,
                    to,
                    Input::key_down(KB_LSHIFT)      ? .2
                        : Input::key_down(KB_LCTRL) ? .0005f
                                                             : .05f));
            if(from == to)
            {
                if(paths.size() != arranged)
                    ++arranged;
                else
                    arranged = 0;
            }
        }
        else
        {
            // testObj->transform()->LookAt(p);
        }
    }

    activeCount = 0;
    for(auto &iter : wrappers)
    {
        auto *trans = std::get<Transform *>(iter);
        float &t = std::get<float>(iter);
        std::list<Runtime::Vec2Int> &paths = std::get<std::remove_reference<decltype(paths)>::type>(iter);

        if(t == 0)
            t = 1;

        if(trans == nullptr)
        {
            int ccc = 0;
        }

        if(!paths.empty())
        {
            if(trans->position() == navMesh->point_to_world_position(paths.back()))
            {
            }
            else
            {
                Vec2 dir = navMesh->point_to_world_position(paths.front());
                if(trans->position() != dir)
                    trans->look_at_lerp(dir, 0.5f);
                trans->position(Vec2::move_towards(
                    trans->position(),
                    dir,
                    Input::key_down(KB_LSHIFT)      ? 2
                        : Input::key_down(KB_LCTRL) ? .005f
                                                    : .05f));
                if(trans->position() == dir)
                {
                    paths.pop_front();
                }
                ++activeCount;
            }
        }
        else
        {
            NavResult result;
            navMesh->find(
                result,
                NavMethodRule::NavigationIntelegency,
                navMesh->get_neuron(trans->position()),
                navMesh->get_neuron(Random::range(0, NavMeshMagnitude), Random::range(0, NavMeshMagnitude)));
            paths = result.roads;
            if(result.status != NavStatus::Opened)
            {
                float a = trans->angle() + 1;
                if(a > 360)
                    a -= 360;
                trans->angle(a);
                paths.clear();
            }
            else
                paths.pop_front();
            navMesh->clear();
        }
    }

    player->transform()->position(Vec2::move_towards(player->transform()->position(), from, 0.085f));
}

void PathMover::on_gizmo()
{
    if(!showGizmosLayer)
        return;

    // draw nav path
    Gizmos::draw_nav_mesh(navMesh);

    Color c(0, 200, 0);

    Gizmos::set_color(c);

    if(!paths.empty())
    {
        auto iter = paths.cbegin();
        int j;

        Vec2 lp = navMesh->point_to_world_position(*begin(paths));
        for(j = 0; iter != _endIter && j <= arranged; ++iter, ++j)
        {
            if(lp != navMesh->point_to_world_position(*iter))
                Gizmos::draw_line(lp, navMesh->point_to_world_position(*iter));
            lp = navMesh->point_to_world_position(*iter);
        }
        to = lp;

        c.b = 200;
        Gizmos::set_color(c);
        for(; iter != _endIter; ++iter)
        {
            Gizmos::draw_line(lp, navMesh->point_to_world_position(*iter));
            lp = navMesh->point_to_world_position(*iter);
        }
    }

    Gizmos::set_color(0xff00ff00);
    Gizmos::draw_triangle(testObj->transform()->position(), 1.f, 1.f);
    // Gizmos::DrawLine(testObj->transform()->position(),
    // testObj->transform()->position() + testObj->transform()->forward() * 2);
}

void PathMover::on_unloading()
{
    testObj = nullptr;
    to = Vec2::zero;
    last = nullptr;
    RoninMemory::free(navMesh);
    paths.clear();
    wrappers.clear();
    wrappers.shrink_to_fit();
}
