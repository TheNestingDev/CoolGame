#include "GraphicsEngine.hpp"
#include "GameLogic.hpp"
#include "Settings.hpp"

int main() {
    GameLogic::Game* game = nullptr;

    try {
        game = new GameLogic::Game(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "COOL GAME :D");
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        delete game;
        return 1;
    }

    game->run();
    delete game;
    return 0;
}
