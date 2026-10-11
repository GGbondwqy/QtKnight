/**
 * @file WorldPrivate.h
 * @brief 状态模块独占的实体记录、运行世界和提交草稿。
 *
 * 仅供 KnightState.dll 内部读写；业务层只持有 World 指针，其他底层模块只读快照。
 * 初始化、复活和整帧提交先构造草稿，全部成功后再替换权威世界。
 */

#pragma once

#include "KnightContracts.h"
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <vector>

namespace knight {

/// @brief 所有实体共有的权威数据，只允许状态模块修改。
struct EntityCore {
    EntityId id = 0; ///< 世界内唯一实体编号。
    EntityKind kind = EntityKind::Player; ///< 组件组合对应的实体类别。
    ResourceId prototypeId; ///< 数值原型编号。
    Vec2 position{}; ///< 权威世界位置。
    Rect collider{}; ///< 相对实体位置的碰撞盒。
    ResourceId spriteKey; ///< 图片资源键。
    bool active = true; ///< 是否启用；拾取后置 false。
};

/// @brief 玩家和敌人共有的运行属性；拾取物不具备此组件。
struct CharacterRuntime {
    Vec2 velocity{}; ///< 权威速度向量。
    int facing = 1; ///< 横向朝向：-1 向左，1 向右。
    int health = 0; ///< 当前生命。
    int maxHealth = 0; ///< 由原型确定的生命上限。
    int energy = 0; ///< 当前能量。
    int maxEnergy = 0; ///< 由原型确定的能量上限。
    bool grounded = false; ///< 落地状态。
    FrameId invulnerableUntil = 0; ///< 受击无敌截止逻辑帧。
};

/// @brief 玩家特有的跳跃和攻击运行状态，与永久进度分开保存。
struct PlayerRuntime {
    int remainingJumps = 0; ///< 剩余可用跳跃次数。
    FrameId attackReadyFrame = 0; ///< 下一次攻击就绪帧。
    ResourceId equippedAttackId; ///< 装备的攻击原型编号。
};

/// @brief 敌人特有的决策、目标、巡逻点与攻击冷却。
struct EnemyRuntime {
    AIState state = AIState::Patrol; ///< 敌人当前决策状态。
    EntityId target = 0; ///< 当前目标实体，0 表示无目标。
    std::uint64_t patrolPointIndex = 0; ///< 当前巡逻点下标。
    FrameId attackReadyFrame = 0; ///< 下一次攻击就绪帧。
};

/// @brief 组合实体核心与可选组件；玩家有 character/player，敌人有 character/enemy，拾取物均无。
struct EntityRecord {
    EntityCore core; ///< 所有实体必备的基础记录。
    std::optional<CharacterRuntime> character; ///< 玩家和敌人具备的角色属性。
    std::optional<PlayerRuntime> player; ///< 仅玩家具备的运行组件。
    std::optional<EnemyRuntime> enemy; ///< 仅敌人具备的运行组件。
};

/// @brief 状态模块拥有的权威世界；外部只能持有不透明指针并通过公开接口读写。
struct World {
    LevelId levelId; ///< 已初始化关卡的编号。
    FrameId frame = 0; ///< 最后成功提交的逻辑帧号。
    std::unordered_map<EntityId, EntityRecord> entities; ///< 以实体编号索引的权威注册表。
    std::vector<AttackInstance> attacks; ///< 跨帧攻击实例及命中去重信息。
    ProgressState progress; ///< 检查点、能力、物品和永久机关进度。
    bool levelComplete = false; ///< 当前关卡的权威完成标记。
};

/// @brief 用于初始化、复活和整批提交的候选世界；成功前不覆盖权威世界。
struct WorldDraft {
    World candidate; ///< 待验证或待原子替换的候选世界。
};

} // namespace knight
