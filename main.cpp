#include "LvGameEngine.h"
#include "LvRulesChecker.h"

#include <cstdio>

int main()
{
	lv::GameState game{};

	lv::GameEngine engine{};
    lv::RulesChecker checker{};

	engine.SetupInitGameState(game, 2);
    while (!engine.IsGameOver(game)) {
        engine.SetupRound(game);
        engine.StartRound(game);
        while (!engine.IsRoundOver(game)) {
            lv::DiceValue dice{};

            if (!game.current_turn.dices.empty()) {
                dice = game.current_turn.dices[0];
            } else if (!game.current_turn.white_dices.empty()) {
                dice = game.current_turn.white_dices[0];
            }
            engine.AllocateDices(game, dice);
            engine.AdvanceToNextPlayer(game);
        }
        engine.EndRound(game);
    }

    engine.SetupRound(game);
    if (checker.ValidateGameState(game)) {
        printf("Game state is valid\n");
    } else {
        printf("Game state is invalid\n");
    }

	return 0;
}