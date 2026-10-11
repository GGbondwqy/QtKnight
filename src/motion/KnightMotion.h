/**
 * @file KnightMotion.h
 * @brief 运动提案与碰撞收尾的公开接口。
 *
 * 当前函数仅有声明，注释说明目标约定；模块不保存跨帧状态或输入指针。
 * 不读取键盘、不调用地图或状态 DLL；结果由业务层传递和提交。
 * Result 的 value 仅在状态为 Ok 时有效；异常须在本 DLL 内转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTMOTION_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTMOTION_BUILD)
#    define KNIGHTMOTION_API __declspec(dllexport)
#  else
#    define KNIGHTMOTION_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTMOTION_API
#endif

namespace knight {

/**
 * @brief 根据当前状态、意图和固定步长提出运动候选。
 * @param before 已提交的旧世界快照。
 * @param level 玩家与敌人的静态运动参数。
 * @param intents 本帧玩家和敌人的移动、跳跃意图。
 * @param fixedSeconds 固定步长，单位秒，首版为 1.0f / 60.0f。
 * @return 成功返回建议位置、速度和扫过区域，不进行地图阻挡裁决。
 */
KNIGHTMOTION_API Result<MotionBatch> proposeMotion(
    const WorldSnapshot& before, const LevelDefinition& level,
    const IntentBatch& intents, float fixedSeconds) noexcept;

/**
 * @brief 将运动提案与地图裁决合成为最终运动变更候选。
 * @param proposed 本帧建议位置和速度。
 * @param resolved 业务层转交的同帧地图裁决结果。
 * @return 成功返回位置、速度、落地及跳跃次数等候选，等待统一提交。
 */
KNIGHTMOTION_API Result<ChangeBatch> finishMotion(
    const MotionBatch& proposed, const CollisionBatch& resolved) noexcept;

} // namespace knight
