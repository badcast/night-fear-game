#include "PathMoverMenu.h"

using namespace RoninEngine::UI;

namespace RoninEngine::Runtime
{

    void controlEvent(uid, GUI*);
    // groups
    uint8_t group_mainmenu_screen, group_startgame_screen, group_setting_screen, group_author_screen;
    // controls
    uint8_t mmb_start_game, mmb_setting, mmb_aboutus, mmb_quit;
    uint8_t sgb_playgame, sgb_option, sgb_backmenu;
    uint8_t ssb_backmenu;
    uint8_t sab_backmenu;

    // Запуск после инициализаций
    void PathMoverMenu::on_awake()
    {

        constexpr int _OFSET = 0x21;
        int fontWidth = 10;

        // Инициализация меню
        auto gameDisplay = RoninSimulator::get_current_resolution();

        Vec2Int point = { gameDisplay.width / 2 - 127, 300 };
        Vec2Int lastPoint;
        // Main menu screen

        group_mainmenu_screen = get_gui()->push_group();
        {
            mmb_start_game = get_gui()->push_button("Start Game", point, nullptr, group_mainmenu_screen);
            point.y += _OFSET;
            mmb_setting = get_gui()->push_button("Options", point, nullptr, group_mainmenu_screen);
            point.y += _OFSET;
            mmb_aboutus = get_gui()->push_button("About US", point, nullptr, group_mainmenu_screen);
            point.y += _OFSET;
            mmb_quit = get_gui()->push_button("Quit", point, nullptr, group_mainmenu_screen);
        }
        lastPoint = point;
        point = { gameDisplay.width / 2 - 127, 300 };
        // Start game screen
        group_startgame_screen = get_gui()->push_group();
        {
            sgb_playgame = get_gui()->push_button("Play", point, nullptr, group_startgame_screen);
            point.y += _OFSET;
            sgb_backmenu = get_gui()->push_button("< Back", lastPoint, nullptr, group_startgame_screen);
        }
        // Setting screen
        group_setting_screen = get_gui()->push_group();
        {
            get_gui()->push_label("Graphics", Vec2Int { gameDisplay.width / 2 - 127, 300 }, fontWidth, group_setting_screen);
            get_gui()->push_slider(1, { gameDisplay.width / 2 - 127, 300 });
            ssb_backmenu = get_gui()->push_button("< Back", lastPoint, nullptr, group_setting_screen);
        }
        // Author & About US
        group_author_screen = get_gui()->push_group();
        {
            get_gui()->push_label("Night Fear", Vec2Int { gameDisplay.width / 2 - 127, 300 }, fontWidth, group_author_screen);
            sab_backmenu = get_gui()->push_button("< Back", lastPoint, nullptr, group_author_screen);
        }

        // Show as First
        get_gui()->show_group_unique(group_mainmenu_screen);
        // Register main event
        get_gui()->register_general_callback((ui_callback)controlEvent, get_gui());
    }

    void controlEvent(uid id, GUI* guiInstance)
    {
        // Эти определения просто сравнивают ID :)
#define e(ID) ID == id
#define e2(ID1, ID2) ID1 == id || ID2 == id
#define e3(ID1, ID2, ID3) ID1 == id || ID2 == id || ID3 == id
#define e4(ID1, ID2, ID3, ID4) ID1 == id || ID2 == id || ID3 == id || ID4 == id
        // main menu events
        if (e(mmb_start_game)) {
            guiInstance->show_group_unique(group_startgame_screen);
        } else if (e(mmb_setting)) {
            guiInstance->show_group_unique(group_setting_screen);
        } else if (e(mmb_aboutus)) {
            guiInstance->show_group_unique(group_author_screen);
        } else if (e(mmb_quit)) {
            RoninSimulator::request_quit();
        }
        // start game menu events
        else if (e(sgb_playgame)) {
            RoninSimulator::load_world(RoninMemory::alloc<PathMover>());
        }
        // setting menu events
        else if (e3(sgb_backmenu, ssb_backmenu, sab_backmenu)) {
            guiInstance->show_group_unique(group_mainmenu_screen);
        }
#undef e
#undef e2
#undef e3
#undef e4
    }

} // namespace RoninEngine
