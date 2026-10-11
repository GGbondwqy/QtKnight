/**
 * @file KnightCombat.h
 * @brief 攻击、陷阱和伤害候选的公开接口。
 *
 * 当前函数仅有声明，注释说明目标约定；本模块不直接扣除权威生命值。
 * 攻击实例、冷却和无敌状态由世界持有，计算结束后不保留输入指针。
 * Result 的 value 仅在状态为 Ok 时有效；异常须在本 DLL 内转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTCOMBAT_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTCOMBAT_BUILD)
#    define KNIGHTCOMBAT_API __declspec(dllexport)
#  else
#    define KNIGHTCOMBAT_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTCOMBAT_API
#endif

namespace knight {

/**
 * @brief 推进现有挥击并判定玩家或敌人的新攻击。
 * @details 按挥击实例和目标去重，仅有效攻击阶段检测命中，不直接扣除权威生命。
 * @param input 旧世界、修正站位、动作意图及完整攻击规则。
 * @return 成功返回攻击阶段和冷却变更、命中事实及事件候选。
 */
KNIGHTCOMBAT_API Result<CombatResult> evaluateAttacks(
    const CombatInput& input) noexcept;

/**
 * @brief 根据危险区命中和逻辑时间生成陷阱候选。
 * @details 周期机关使用逻辑帧，不依赖界面动画时间。
 * @param input 旧世界、修正站位及陷阱原型。
 * @param triggers 业务层转交的同帧危险区命中列表。
 * @return 成功返回陷阱伤害及相关变更、事件候选。
 */
KNIGHTCOMBAT_API Result<CombatResult> evaluateTraps(
    const CombatInput& input, const TriggerBatch& triggers) noexcept;

/**
 * @brief 统一裁决已合并攻击与陷阱的伤害候选。
 * @details 结果仍需 applyChanges 提交；失败或提交失败均不发布命中成功反馈。
 * @param before 受击前生命及无敌截止帧等状态。
 * @param candidates 协调器合并的本帧战斗与陷阱候选。
 * @return 成功返回去重并应用无敌规则后的生命、击退、死亡等变更和事件候选。
 */
KNIGHTCOMBAT_API Result<CombatResult> resolveDamage(
    const WorldSnapshot& before, const CombatResult& candidates) noexcept;

} // namespace knight
