#pragma once

#include "KnightContracts.h"
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <vector>

namespace knight {

// 权威实体数据只允许状态模块修改，其他模块只能读取快照。
struct EntityCore {
    EntityId id = 0;
    EntityKind kind = EntityKind::Player;
    ResourceId prototypeId;
    Vec2 position{};
    Rect collider{};
    ResourceId spriteKey;
    bool active = true;
};

struct CharacterRuntime {
    Vec2 velocity{};
    int facing = 1;
    int health = 0;
    int maxHealth = 0;
    int energy = 0;
    int maxEnergy = 0;
    bool grounded = false;
    FrameId invulnerableUntil = 0;
};

struct PlayerRuntime {
    int remainingJumps = 0;
    FrameId attackReadyFrame = 0;
    ResourceId equippedAttackId;
};

struct EnemyRuntime {
    AIState state = AIState::Patrol;
    EntityId target = 0;
    std::uint64_t patrolPointIndex = 0;
    FrameId attackReadyFrame = 0;
};

struct EntityRecord {
    EntityCore core;
    std::optional<CharacterRuntime> character;
    std::optional<PlayerRuntime> player;
    std::optional<EnemyRuntime> enemy;
};

struct World {
    LevelId levelId;
    FrameId frame = 0;
    std::unordered_map<EntityId, EntityRecord> entities;
    std::vector<AttackInstance> attacks;
    ProgressState progress;
    bool levelComplete = false;
};

struct WorldDraft {
    World candidate;
};

} // namespace knight
