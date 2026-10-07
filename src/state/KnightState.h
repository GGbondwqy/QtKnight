#pragma once

#include "KnightContracts.h"

// 构建本模块时导出接口；其他模块包含本文件时导入接口。
#if defined(_WIN32)
#  if defined(KNIGHTSTATE_BUILD)
#    define KNIGHTSTATE_API __declspec(dllexport)
#  else
#    define KNIGHTSTATE_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTSTATE_API
#endif

namespace knight {

// 世界的成员只在状态模块内部可见；调用方只持有句柄。
struct World;

KNIGHTSTATE_API World* createWorld() noexcept;
KNIGHTSTATE_API void destroyWorld(World* world) noexcept;
KNIGHTSTATE_API bool getPlayer(World* world, EntitySnapshot* output) noexcept;
KNIGHTSTATE_API Status initializeWorld(
    World* world, const LevelDefinition& level, const SaveData* restore) noexcept;
KNIGHTSTATE_API Result<WorldSnapshot> readWorld(World* world) noexcept;
KNIGHTSTATE_API Result<CommitResult> applyChanges(
    World* world, const ChangeBatch& changes) noexcept;
KNIGHTSTATE_API Status resetAtCheckpoint(
    World* world, const LevelDefinition& level, const SaveData& progress) noexcept;

} // namespace knight
