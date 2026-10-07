#include "KnightState.h"
#include "WorldPrivate.h"
#include <new>

namespace knight{
// 当前仅保留旧版演示函数。正式世界初始化接口仍只有头文件声明。
World* createWorld() noexcept{
    World* world = new(std::nothrow) World{};
    if (world == nullptr) return nullptr;

    EntityRecord player{};
    player.core.id = 1;
    player.core.kind = EntityKind::Player;
    player.character.emplace();
    player.character->health = 5;
    player.player.emplace();
    world->entities.emplace(player.core.id, player);
    return world;
}

void destroyWorld(World* world) noexcept{
    delete world;
}

bool getPlayer(World* world, EntitySnapshot* output)noexcept{
    if (world == nullptr || output == nullptr) return false;
    const auto found = world->entities.find(1);
    if (found == world->entities.end() || !found->second.character) return false;

    const EntityRecord& player = found->second;
    EntitySnapshot snapshot{};
    snapshot.id = player.core.id;
    snapshot.kind = player.core.kind;
    snapshot.position = player.core.position;
    snapshot.collider = player.core.collider;
    snapshot.active = player.core.active;
    snapshot.prototypeId = player.core.prototypeId;
    snapshot.spriteKey = player.core.spriteKey;
    snapshot.velocity = player.character->velocity;
    snapshot.health = player.character->health;
    snapshot.energy = player.character->energy;
    snapshot.facing = player.character->facing;
    snapshot.grounded = player.character->grounded;
    snapshot.invulnerableUntil = player.character->invulnerableUntil;
    if (player.player) {
        snapshot.remainingJumps = player.player->remainingJumps;
        snapshot.attackReadyFrame = player.player->attackReadyFrame;
        snapshot.equippedAttackId = player.player->equippedAttackId;
    }
    *output = snapshot;
    return true;
}
}
