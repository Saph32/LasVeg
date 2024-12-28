#pragma once

#include "LvPublic.h"

namespace lv {

class GameEngine {
public:
    bool SetupGameState(GameState &rGame, int32_t player_count);
    bool SetupPlayerTurnState(PlayerTurnState &rPlayerTurn, const GameState &rGame, PlayerIdx player_idx);
    bool RollDices(std::vector<DiceValue>& rDices, int32_t dice_count);
    bool SetupRound(GameState &rGame);
};

} // namespace lv
