#pragma once

#include "KnightContracts.h"
#include <cstdint>
#include <deque>
#include <optional>

namespace knight {

struct World;
struct MapHandle;
struct SaveHandle;

enum class SessionState {
    MainMenu, Loading, Playing, Paused, Dead, LevelComplete, Exiting
};

// 按住状态与本帧新按下状态分开，防止移动时重复或遗漏攻击。
struct InputState {
    bool leftHeld = false, rightHeld = false;
    bool jumpHeld = false, attackHeld = false, interactHeld = false;
    bool jumpPressedThisFrame = false, attackPressedThisFrame = false;
    bool interactPressedThisFrame = false;
    CommandId nextCommandId = 1;
};

// 业务层拥有会话；底层句柄必须通过各自模块的销毁函数释放。
struct GameHandle {
    GameConfig config;
    SessionState session = SessionState::MainMenu;
    InputState input;
    std::deque<PlayerCommand> pendingCommands;
    std::uint64_t accumulatedMicroseconds = 0;
    std::optional<LevelDefinition> currentLevel;
    World* world = nullptr;
    MapHandle* map = nullptr;
    SaveHandle* save = nullptr;
    SlotId currentSlot = 0;
    RenderSnapshot rendered;
    EventBatch queuedEvents;
};

} // namespace knight
