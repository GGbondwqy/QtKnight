#pragma once

#include "KnightContracts.h"

#if defined(_WIN32)
#  if defined(KNIGHTGAME_BUILD)
#    define KNIGHTGAME_API __declspec(dllexport)
#  else
#    define KNIGHTGAME_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTGAME_API
#endif

namespace knight {

// 界面只持有业务句柄，不访问业务层内部数据。
struct GameHandle;

KNIGHTGAME_API Result<GameHandle*> createGame(const GameConfig& config) noexcept;
KNIGHTGAME_API void destroyGame(GameHandle* game) noexcept;
KNIGHTGAME_API Status submitCommand(
    GameHandle* game, const PlayerCommand& command) noexcept;
KNIGHTGAME_API Status advanceElapsed(
    GameHandle* game, std::uint64_t elapsedMicroseconds) noexcept;
KNIGHTGAME_API Result<RenderSnapshot> readRenderSnapshot(GameHandle* game) noexcept;
KNIGHTGAME_API Result<EventBatch> takeEvents(GameHandle* game) noexcept;

} // namespace knight
