/**
 * @file KnightAI.h
 * @brief 根据世界快照生成敌人意图与决策候选。
 *
 * 当前函数仅有声明；敌人状态保存在世界中，本模块不保存跨帧私有状态。
 * 不直接移动实体或调用其他底层 DLL；结果由业务层统一合并。
 * Result 的 value 仅在状态为 Ok 时有效；异常须在本 DLL 内转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTAI_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTAI_BUILD)
#    define KNIGHTAI_API __declspec(dllexport)
#  else
#    define KNIGHTAI_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTAI_API
#endif

namespace knight {

/**
 * @brief 从只读世界决定敌人本帧行动。
 * @details 每帧运动前由协调器调用，不直接移动实体或保存输入指针。
 * @param before 玩家、敌人及敌人当前目标和巡逻状态的旧快照。
 * @param level 敌人索敌、巡逻和攻击数值原型。
 * @param frame 当前要计算的逻辑帧号。
 * @return 成功返回移动/攻击意图，以及决策状态、目标和巡逻点变更候选。
 */
KNIGHTAI_API Result<AIResult> decideEnemies(
    const WorldSnapshot& before, const LevelDefinition& level,
    FrameId frame) noexcept;

} // namespace knight
