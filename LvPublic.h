#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace lv {

enum { MAX_PLAYER_COUNT = 5 };
enum { CASINO_COUNT = 6 };
enum { DICE_COUNT = 8};
enum { ROUND_COUNT = 4};
enum { CASINO_MIN_MONEY_VALUE = 50};

enum class DiceValue : int32_t {
    Invalid = 0,
    _1 = 1,
    _2 = 2,
    _3 = 3,
    _4 = 4,
    _5 = 5,
    _6 = 6,
};

enum class Color : int32_t {
    Invalid = 0,
    Red = 1,
    Blue = 2,
    Green = 3,
    Black = 4,
    White = 5,
};

constexpr DiceValue DICE_VALUE_MIN = DiceValue::_1;
constexpr DiceValue DICE_VALUE_MAX = DiceValue::_6;

static_assert(CASINO_COUNT == static_cast<int32_t>(DICE_VALUE_MAX),
              "CASINO_COUNT must be equal to DICE_VALUE_MAX");

using PlayerIdx = uint32_t;
using CasinoIdx = uint32_t;
using DiceIndex = uint32_t;

enum class Bill : int32_t {
    Invalid = 0,
    _10 = 10,
    _20 = 20,
    _30 = 30,
    _40 = 40,
    _50 = 50,
    _60 = 60,
    _70 = 70,
    _80 = 80,
    _90 = 90,
};

struct PlayerState {
    PlayerIdx idx = 0;
    Color color = Color::Invalid;
    std::vector<Bill> bills;
    int32_t dices = 0;
    int32_t white_dices = 0;
};

struct NeutralPlayerState {
    std::vector<Bill> bills;
};

struct CasinoState {
    CasinoIdx idx = 0;
    DiceValue dice = DiceValue::Invalid;
    std::vector<Bill> bills;
    std::array<int32_t, MAX_PLAYER_COUNT> dice_bets{};
    int32_t neutral_dice_bet = 0;
};

struct PlayerTurnState {
    PlayerIdx player_idx = 0;
    std::vector<DiceValue> dices;
    std::vector<DiceValue> white_dices;
};

struct GameState {
    int32_t round = 0;

    PlayerIdx first_player_idx = 0;

    int32_t player_count = 0;
    bool neutral_player_present = false;

    std::vector<PlayerState> players{};
    std::array<CasinoState, CASINO_COUNT> casinos{};
    NeutralPlayerState neutral_player{};

    PlayerTurnState current_turn{};

    std::vector<Bill> bank{};  
};

struct BankEntry {
    Bill bill = Bill::Invalid;
    int32_t count = 0;
};
constexpr BankEntry BANK_INIT_STOCK_TABLE[] = {
    {Bill::_10, 6},
    {Bill::_20, 8},
    {Bill::_30, 8},
    {Bill::_40, 6},
    {Bill::_50, 6},
    {Bill::_60, 5},
    {Bill::_70, 5},
    {Bill::_80, 5},
    {Bill::_90, 5},
};

struct ExtraWhiteDiceEntry {
    int32_t player_count = 0;
    int32_t white_dice_count = 0;
};

constexpr ExtraWhiteDiceEntry EXTRA_WHITE_DICE_COUNT_TABLE[] = {
    {2, 4},
    {3, 2},
    {4, 2},
    {5, 0},
};

} // namespace lv