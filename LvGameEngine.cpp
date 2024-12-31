#include "LvGameEngine.h"
#include <random>

bool lv::GameEngine::SetupInitGameState(GameState& rGame, int32_t player_count)
{
    // Check player count
    if (player_count < 2 || player_count > MAX_PLAYER_COUNT) {
        return false;
    }

    rGame = {};

    // Setup bank
    for (const BankEntry &rEntry : BANK_INIT_STOCK_TABLE) {
        for (int32_t i = 0; i < rEntry.count; ++i)
        {
            rGame.bank.push_back(rEntry.bill);
        }
    }

    // Setup players
    rGame.player_count = player_count;
    rGame.neutral_player_present = player_count < MAX_PLAYER_COUNT;
    rGame.players.resize(player_count);

    int32_t extra_white_dices_count = 0;
    for (const ExtraWhiteDiceEntry &rEntry : EXTRA_WHITE_DICE_COUNT_TABLE) {
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

    return true;
}

bool lv::GameEngine::SetupRound(GameState& rGame)
{
    ShuffleBank(rGame.bank);

    if (!SetupCasinoBills(rGame)) {
        return false;
    }

    if (!SetupPlayerTurnState(rGame.current_turn, rGame, rGame.first_player_idx)) {
        return false;
    }

    return true;
}

bool lv::GameEngine::AllocateDices(GameState& rGame, DiceValue dice)
{
    // Validate dice value
    if (dice < DICE_VALUE_MIN || dice > DICE_VALUE_MAX) {
        return false;
    }

    // Get the dice's casino
    CasinoState &rCasino = rGame.casinos[static_cast<CasinoIdx>(dice) - 1];

    auto allocate_fn = [&](std::vector<DiceValue> &rDices, int32_t &rCasino_bet, int32_t rPlayer_dices) {
        uint32_t dices_allocated = 0;

        // Iterate through the current turn's dices and find the one matching the dice value
        for (DiceValue &rDice : rGame.current_turn.dices) {
            if (rDice == dice) {
                ++dices_allocated;
                ++rCasino_bet;
                --rPlayer_dices;
            }
        }
        return dices_allocated;
    };

    // Allocate player dices
    uint32_t dices_allocated = allocate_fn(rGame.current_turn.dices, rCasino.dice_bets[rGame.current_turn.player_idx],
                                           rGame.players[rGame.current_turn.player_idx].dices);

    // Allocate white dices
    uint32_t white_dices_allocated = allocate_fn(rGame.current_turn.white_dices, rCasino.neutral_dice_bet,
                                                 rGame.players[rGame.current_turn.player_idx].white_dices);
    
    // Clear current turn dices
    rGame.current_turn.dices.clear();
    rGame.current_turn.white_dices.clear();

    // Do some validation
    if (dices_allocated == 0 && white_dices_allocated == 0) {
        return false;
    }

    return true;
}

bool lv::GameEngine::IsRoundOver(const GameState& rGame) const
{
    // Round is over is no players have any dice left
    for (const PlayerState &rPlayer : rGame.players) {
        if (rPlayer.dices > 0) {
            return false;
        }
    }

    return true;
}

bool lv::GameEngine::IsGameOver(const GameState& rGame) const
{
    // Game is over if this is the last round
    if (rGame.round + 1 >= ROUND_COUNT) {
        return true;
    }
    return false;
}

bool lv::GameEngine::AdvanceToNextPlayer(GameState& rGame)
{
    // Find the next player with some dices
    PlayerIdx initial_player_idx = rGame.current_turn.player_idx;
    PlayerIdx next_player_idx = (initial_player_idx + 1) % rGame.players.size();

    while (next_player_idx != initial_player_idx)
    {
        const PlayerState &rPlayer = rGame.players[next_player_idx];

        if (rPlayer.dices > 0 || rPlayer.white_dices > 0)
        {
            return true;
        }
    }

    return false; // We got back to first player, logic error
}

bool lv::GameEngine::DistributeCasinoBills(GameState& rGame)
{
    // Go through all casinos
    for (CasinoState &rCasino : rGame.casinos) {

        // Sort bills in ascending order
        std::sort(rCasino.bills.begin(), rCasino.bills.end(), std::greater<Bill>());

        bool more_to_distribute = true;
        while (more_to_distribute)
        {
            // Check if there are no more bills to distribute
            if (rCasino.bills.empty()) {
                more_to_distribute = false;
                break;
            }

            // Take the bill with highest value
            Bill highest_bill = rCasino.bills.back();
            rCasino.bills.pop_back();

            // Find the player with the highest bet
            int32_t highest_bet = 0;
            enum {INVALID_PLAYER_IDX = -1};
            PlayerIdx highest_bet_player_idx = INVALID_PLAYER_IDX;
            for (PlayerIdx player_idx = 0; player_idx < rGame.players.size(); ++player_idx) {
                if (rCasino.dice_bets[player_idx] > highest_bet) {
                    highest_bet = rCasino.dice_bets[player_idx];
                    highest_bet_player_idx = player_idx;
                }
            }

            // Check if the neutral player has the highest bet
            bool highest_bet_neutral = false;
            if (rCasino.neutral_dice_bet > highest_bet) {
                highest_bet = rCasino.neutral_dice_bet;
                highest_bet_neutral = true;
            }

            // Was there any bet ?
            if (highest_bet_player_idx == INVALID_PLAYER_IDX && !highest_bet_neutral) {
                more_to_distribute = false;
                break;
            } else {
                // Add bill to highest player's money
                if (!highest_bet_neutral) {
                    rGame.players[highest_bet_player_idx].bills.push_back(highest_bill);
                }
            }
        }
    }

    return true;
}

bool lv::GameEngine::SetupCasinoBills(GameState& rGame)
{
    // Allocate bills to each casino
    for (CasinoState &rCasino : rGame.casinos) {
        rCasino.bills.clear();
        int32_t current_value = 0;
        while (current_value < CASINO_MIN_MONEY_VALUE) {
            rCasino.bills.push_back(rGame.bank.back());
            rGame.bank.pop_back();
            current_value += static_cast<int32_t>(rCasino.bills.back());
        }
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

void lv::GameEngine::ShuffleBank(std::vector<Bill>& rBank) { 
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(rBank.begin(), rBank.end(), gen);
}
