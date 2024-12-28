#pragma once

#include "LvPublic.h"

namespace lv {

class RulesChecker {
public:
    bool ValidateGameState(const GameState &state) const;
};
} // namespace lv