#include "LvGameEngine.h"
#include "LvRulesChecker.h"

#include <cstdio>

int main()
{
	lv::GameState game{};

	lv::GameEngine engine{};
    lv::RulesChecker checker{};

	engine.SetupInitGameState(game, 2);
    engine.SetupRound(game);
    if (checker.ValidateGameState(game)) {
        printf("Game state is valid\n");
    } else {
        printf("Game state is invalid\n");
    }

	return 0;
}