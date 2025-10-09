// main.cpp - Point d’entrée du moteur Raylib + Tiled TMJ Game Engine
#include "Game/Game.h"

int main() {
    // Créer et exécuter le jeu
    Game game;

    // IMPORTANT : Indique le chemin vers ton fichier TMJ ici
    const std::string mapPath = "./assets/maps/map.tmj";

    game.Run(mapPath);

    return 0;
}
