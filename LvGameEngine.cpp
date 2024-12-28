#include "LvGameEngine.h"
#include <random>

bool lv::GameEngine::SetupGameState(GameState& rGame, int32_t player_count)
{
    // Check player count
    if (player_count < 2 || player_count > MAX_PLAYER_COUNT) {
        return false;
    }

    rGame = {};

    // Setup bank
    rGame.bank = std::vector<BankEntry>(std::begin(BANK_INIT_STOCK), std::end(BANK_INIT_STOCK));

    // Setup players
    rGame.player_count = player_count;
    rGame.neutral_player_present = player_count < MAX_PLAYER_COUNT;
    rGame.players.resize(player_count);

    int32_t extra_white_dices_count = 0;
    for (const ExtraWhiteDiceEntry &rEntry : EXTRA_WHITE_DICE_COUNT) {
        if (player_count == rEntry.player_count) {
            extra_white_dices_count = rEntry.white_dice_count;
            break;
        }
    }

    for (PlayerIdx player_idx = 0; player_idx < rGame.players.size(); ++player_idx) {
        PlayerState &rPlayer = rGame.players[player_idx];
        rPlayer.idx = player_idx;
        rPlayer.color = static_cast<Color>(player_idx + 1);
        rPlayer.dices = DICE_COUNT;
        rPlayer.white_dices = extra_white_dices_count;
    }

    // Setup casinos
    for (CasinoIdx casino_idx = 0; casino_idx < CASINO_COUNT; ++casino_idx) {
        CasinoState &rCasino = rGame.casinos[casino_idx];
        rCasino.idx = casino_idx;
        rCasino.dice = static_cast<DiceValue>(casino_idx + 1);
        rCasino.bills.clear();
        rCasino.dice_bets.fill(0);
    }

    // Start round 0
    rGame.first_player_idx = 0;

    if (!SetupRound(rGame)) {
        return false;
    }

    return true;
}

bool lv::GameEngine::SetupPlayerTurnState(PlayerTurnState& rPlayerTurn, const GameState &rGame, PlayerIdx player_idx)
{
    // Validate current player index
    if (player_idx < 0 || player_idx >= rGame.players.size()) {
        return false;
    }

    // Copy player information into player turn state
    const PlayerState &rPlayer = rGame.players[player_idx];
    rPlayerTurn.player_idx = player_idx;

    if (!RollDices(rPlayerTurn.dices, rPlayer.dices)) {
        return false;
    }
    if (!RollDices(rPlayerTurn.white_dices, rPlayer.white_dices)) {
        return false;
    }

    return true;
}

bool lv::GameEngine::RollDices(std::vector<DiceValue>& rDices, int32_t dice_count)
{
    rDices.clear();

    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(static_cast<int32_t>(DICE_VALUE_MIN), static_cast<int32_t>(DICE_VALUE_MAX));

    // Choose a number dice value for each dice
    for (int32_t i = 0; i < dice_count; ++i) {
        // Choose a random value between 1 and 6
        rDices.push_back(static_cast<DiceValue>(dis(gen)));
    }

    return true;
}

bool lv::GameEngine::SetupRound(GameState& rGame)
{
    // TODO : Allocate bills to casinos

    if (!SetupPlayerTurnState(rGame.current_turn, rGame, rGame.first_player_idx)) {
        return false;
    }

    return true;
}
