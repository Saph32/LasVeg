#include "LvGameEngine.h"
#include "LvRulesChecker.h"

#include <cstdio>

int main()
{
	lv::GameState game{};

	lv::GameEngine engine{};
    lv::RulesChecker checker{};

	engine.SetupGameState(game, 2);
    if (checker.ValidateGameState(game)) {
        printf("Game state is valid\n");
    } else {
        printf("Game state is invalid\n");
    }

	return 0;
}