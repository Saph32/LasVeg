#pragma once

#include "LvPublic.h"

namespace lv {

constexpr int32_t GetCasinoMoneyValue(const CasinoState &rCasino) {
    int32_t value = 0;
    for (const Bill &rBill : rCasino.bills) {
        value += static_cast<int32_t>(rBill);
    }
    return value;
}

constexpr int32_t GetPlayerMoneyValue(const PlayerState &rPlayer) {
    int32_t value = 0;
    for (const Bill &rBill : rPlayer.bills) {
        value += static_cast<int32_t>(rBill);
    }
    return value;
}

} // namespace lv
