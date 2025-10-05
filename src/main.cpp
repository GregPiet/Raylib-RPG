#include "core/Game.hpp"

int main() {
    Game game;
    game.Init();

    while (!game.ShouldClose()) {
        game.Update();
        game.Draw();
    }

    game.Unload();
    return 0;
}
