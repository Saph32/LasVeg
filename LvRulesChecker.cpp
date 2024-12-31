#include "LvRulesChecker.h"
#include "LvUtils.h"

#include <numeric>
#include <array>
#include <vector>
#include <algorithm>
#include <set>

bool lv::RulesChecker::ValidateGameState(const GameState& rGame) const
{
    // It's a game for 2-5 players
    if (rGame.player_count < 2 || rGame.player_count > MAX_PLAYER_COUNT) {
        return false;
    }

    // The players vector must have the correct size
    if (rGame.players.size() != rGame.player_count) {
        return false;
    }

    // Neutral player is present only if there's less than 5 players
    if (rGame.neutral_player_present && rGame.player_count == MAX_PLAYER_COUNT) {
        return false;
    }

    // Each active player has 8 dices, which must either be in player's stock or in a casino
    for (PlayerIdx player_idx = 0; player_idx < rGame.players.size(); ++player_idx) {
        const auto &rPlayer = rGame.players[player_idx];
        
        int32_t dices = rPlayer.dices;

        for (const auto &rCasino : rGame.casinos) {
            dices += rCasino.dice_bets[player_idx];
        }

        if (dices != DICE_COUNT) {
            return false;
        }
    }

    // Compute the expected neutral player dice count
    int32_t neutral_dices = 0;
    for (const ExtraWhiteDiceEntry& rEntry : EXTRA_WHITE_DICE_COUNT_TABLE) {
        if (rGame.player_count == rEntry.player_count) {
            neutral_dices = rEntry.white_dice_count * rGame.player_count;
            break;
        }
    }

    // The neutral dices must either be in a player stocks's or allocated to a casino
    PlayerIdx neutral_player_idx = rGame.player_count + 1;
    if (rGame.neutral_player_present) {
        int32_t dices = 0;
        for (const auto &rCasino : rGame.casinos) {
            dices += rCasino.neutral_dice_bet;
        }
        for (const PlayerState &rPlayer : rGame.players) {
            dices += rPlayer.white_dices;
        }
        if (dices != neutral_dices) {
            return false;
        }
    }

    // Each bank note bill must be either in the bank, in a casino or in a player's stock
    for (const BankEntry& rInitBankEntry : BANK_INIT_STOCK_TABLE) {
        const size_t expected_count = rInitBankEntry.count;
        const Bill bill = rInitBankEntry.bill;

        size_t current_count = 0;

        // Check bank
        current_count = std::count(rGame.bank.begin(), rGame.bank.end(), bill);

        // Check casinos
        for (const CasinoState &rCasino : rGame.casinos) {
            for (const Bill &rCasinoBill : rCasino.bills) {
                if (rCasinoBill == bill) {
                    ++current_count;
                }
            }
        }

        // Check players
        for (const PlayerState &rPlayer : rGame.players) {
            for (const Bill &rPlayerBill : rPlayer.bills) {
                if (rPlayerBill == bill) {
                    ++current_count;
                }
            }
        }

        // Check neutral player
        for (const Bill &rNeutralPlayerBill : rGame.neutral_player.bills) {
            if (rNeutralPlayerBill == bill) {
                ++current_count;
            }
        }

        // Check if the count is correct
        if (current_count != expected_count) {
            return false;
        }
    }

    // Players index must match the index in the game state
    for (PlayerIdx player_idx = 0; player_idx < rGame.players.size(); ++player_idx) {
        if (rGame.players[player_idx].idx != player_idx) {
            return false;
        }
    }

    // Casinos index must match the index in the game state
    for (CasinoIdx casino_idx = 0; casino_idx < CASINO_COUNT; ++casino_idx) {
        if (rGame.casinos[casino_idx].idx != casino_idx) {
            return false;
        }
    }

    // The first player index must be a valid player index
    if (rGame.first_player_idx < 0 || rGame.first_player_idx >= rGame.players.size()) {
        return false;
    }

    // The current turn player index must be a valid player index
    if (rGame.current_turn.player_idx < 0 || rGame.current_turn.player_idx >= rGame.players.size()) {
        return false;
    }

    // The current turn player must have the correct number of dices
    if (rGame.current_turn.dices.size() != rGame.players[rGame.current_turn.player_idx].dices) {
        return false;
    }

    // The current turn player must have the correct number of white dices
    if (rGame.current_turn.white_dices.size() != rGame.players[rGame.current_turn.player_idx].white_dices) {
        return false;
    }

    // Each player must have an unique color
    std::set<Color> colors;
    for (const PlayerState &rPlayer : rGame.players) {
        if (colors.find(rPlayer.color) != colors.end()) {
            return false;
        }
        colors.insert(rPlayer.color);
    }

    // If neutral player is present, then no player can be white
    if (rGame.neutral_player_present) {
        if (colors.find(Color::White) != colors.end()) {
            return false;
        }
    }

    // Each casino dice value must match it's index
    for (CasinoIdx casino_idx = 0; casino_idx < CASINO_COUNT; ++casino_idx) {
        if (rGame.casinos[casino_idx].dice != static_cast<DiceValue>(casino_idx + 1)) {
            return false;
        }
    }

    // Game's round must be positive
    if (rGame.round < 0) {
        return false;
    }

    // Game's round must be less than the round count
    if (rGame.round >= ROUND_COUNT) {
        return false;
    }

    // Casino value must be at least 50
    for (const CasinoState &rCasino : rGame.casinos) {
        if (GetCasinoMoneyValue(rCasino) < CASINO_MIN_MONEY_VALUE) {
            return false;
        }
    }

    // Players cannot have a negative amount of dices or white dices
    for (const PlayerState &rPlayer : rGame.players) {
        if (rPlayer.dices < 0 || rPlayer.white_dices < 0) {
            return false;
        }
    }

    return true;
}
