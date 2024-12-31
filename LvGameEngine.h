#pragma once

#include "LvPublic.h"

namespace lv {

class GameEngine {
public:
    bool SetupInitGameState(GameState &rGame, int32_t player_count);
    bool SetupRound(GameState &rGame);
    bool AllocateDices(GameState &rGame, DiceValue dice);
    bool IsRoundOver(const GameState &rGame) const;
    bool IsGameOver(const GameState &rGame) const;
    bool AdvanceToNextPlayer(GameState &rGame);
    bool DistributeCasinoBills(GameState &rGame);

  private:
    bool SetupCasinoBills(GameState &rGame);
    bool SetupPlayerTurnState(PlayerTurnState &rPlayerTurn, const GameState &rGame, PlayerIdx player_idx);
    bool RollDices(std::vector<DiceValue>& rDices, int32_t dice_count);
    void ShuffleBank(std::vector<Bill> &rBank);
};

} // namespace lv
