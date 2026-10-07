#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// 所有跨模块传递的值类型都在此定义；此文件本身不生成 DLL。
namespace knight {

using EntityId = std::uint64_t;
using FrameId = std::uint64_t;
using CommandId = std::uint64_t;
using SlotId = std::int64_t;
using LevelId = std::string;
using ResourceId = std::string;
using ContentRoot = std::string;
using DatabasePath = std::string;

// 世界坐标：向右为 x 正方向，向下为 y 正方向。
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

// 实体碰撞盒的 x/y 相对实体位置；地图触发区的 x/y 是世界坐标。
struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

enum class StatusCode : std::uint8_t {
    Ok = 0,
    InvalidArgument,
    InvalidState,
    NotFound,
    InvalidFormat,
    VersionMismatch,
    IoFailure,
    OutOfMemory,
    InternalFailure
};

struct Status {
    StatusCode code = StatusCode::Ok;
    std::array<char, 256> messageUtf8{};
};

// 只有 status.code == Ok 时，调用方才能使用 value。
template<class T>
struct Result {
    Status status{};
    T value{};
};

enum class Action : std::uint8_t {
    MoveLeft, MoveRight, Jump, Attack, Interact, Pause, Resume,
    NewGame, ContinueGame, Retry, SaveAndReturn, NextLevel, Exit
};

enum class CommandPhase : std::uint8_t { Press, Release, Trigger };
enum class EntityKind : std::uint8_t { Player, Enemy, Pickup };
enum class AIState : std::uint8_t {
    None, Patrol, Alert, Chase, Attack, Return
};
enum class AttackPhase : std::uint8_t {
    None, Windup, Active, Recovery
};
enum class TriggerKind : std::uint8_t {
    Hazard, Pickup, Checkpoint, Door, Exit
};

// UI 填写动作、阶段、方向和槽位；业务层分配编号与目标帧。
struct PlayerCommand {
    CommandId id = 0;
    Action action = Action::MoveLeft;
    CommandPhase phase = CommandPhase::Press;
    float direction = 0.0f;
    SlotId slotId = 0;
    FrameId targetFrame = 0;
};

struct GameConfig {
    ContentRoot contentRoot;
    DatabasePath databasePath;
    LevelId firstLevelId;
    std::uint32_t contractVersion = 1;
};

// 一次攻击的跨帧状态由世界持有，而不是由战斗 DLL 私自保存。
struct AttackInstance {
    std::uint64_t id = 0;
    EntityId owner = 0;
    FrameId startedFrame = 0;
    AttackPhase phase = AttackPhase::None;
    std::vector<EntityId> hitTargets;
};

// 状态模块输出的独立副本；更改副本不会更改权威世界。
struct EntitySnapshot {
    EntityId id = 0;
    EntityId target = 0;
    EntityKind kind = EntityKind::Player;
    Vec2 position{};
    Vec2 velocity{};
    Rect collider{};
    int health = 0;
    int energy = 0;
    int remainingJumps = 0;
    int facing = 1;
    bool active = true;
    bool grounded = false;
    FrameId invulnerableUntil = 0;
    FrameId attackReadyFrame = 0;
    std::uint64_t patrolPointIndex = 0;
    AIState aiState = AIState::None;
    ResourceId prototypeId;
    ResourceId spriteKey;
    ResourceId equippedAttackId;
};

// 只保存需要跨关或跨会话恢复的进度。
struct ProgressState {
    LevelId levelId;
    std::string checkpointId;
    int health = 0;
    int energy = 0;
    std::vector<std::string> abilities;
    std::vector<std::string> keyItems;
    std::vector<std::string> permanentSwitches;
    std::vector<std::string> clearedObjectives;
};

struct WorldSnapshot {
    LevelId levelId;
    FrameId frame = 0;
    std::vector<EntitySnapshot> entities;
    std::vector<AttackInstance> attacks;
    ProgressState progress;
    bool levelComplete = false;
};

struct SaveData {
    std::uint32_t formatVersion = 1;
    SlotId slotId = 0;
    ProgressState progress;
};

struct SlotSeed {
    ProgressState initialProgress;
};

struct SlotSummary {
    SlotId slotId = 0;
    LevelId levelId;
    std::string checkpointId;
    std::uint64_t updatedEpochSeconds = 0;
};

struct SlotSummaryBatch {
    std::vector<SlotSummary> slots;
};

// 数值原型与地图定义由内容模块加载，经业务层传给各计算模块。
struct PlayerRules {
    float moveSpeed = 0.0f;
    float jumpSpeed = 0.0f;
    float gravity = 0.0f;
    float maxFallSpeed = 0.0f;
    int maxJumps = 0;
    int maxHealth = 0;
    int maxEnergy = 0;
    Rect collider{};
    ResourceId spriteKey;
    ResourceId defaultAttackId;
};

struct EnemyRules {
    float moveSpeed = 0.0f;
    float detectRadius = 0.0f;
    float attackRange = 0.0f;
    FrameId attackCooldown = 0;
    int maxHealth = 0;
    int maxEnergy = 0;
    Rect collider{};
    ResourceId spriteKey;
    ResourceId attackId;
    ResourceId behaviorId;
};

struct AttackRules {
    Rect hitBox{};
    int damage = 0;
    FrameId windupFrames = 0;
    FrameId activeFrames = 0;
    FrameId recoveryFrames = 0;
    FrameId cooldownFrames = 0;
};

struct TrapRules {
    int damage = 0;
    FrameId periodFrames = 0;
    FrameId activeFrames = 0;
};

struct PrototypeCatalog {
    PlayerRules player;
    std::unordered_map<ResourceId, EnemyRules> enemies;
    std::unordered_map<ResourceId, AttackRules> attacks;
    std::unordered_map<ResourceId, TrapRules> traps;
};

struct TileLayer {
    int width = 0;
    int height = 0;
    std::vector<int> tiles; // 下标为 row * width + column。
};

struct EntitySpawn {
    EntityId id = 0;
    EntityKind kind = EntityKind::Enemy;
    ResourceId prototypeId;
    ResourceId spriteKey;
    Vec2 position{};
    Rect collider{};
    std::vector<Vec2> patrolPoints;
};

struct TriggerDefinition {
    std::string id;
    TriggerKind kind = TriggerKind::Hazard;
    Rect area{};
    ResourceId prototypeId;
    EntityId linkedEntity = 0;
};

struct LevelDefinition {
    std::uint32_t formatVersion = 1;
    LevelId id;
    LevelId nextLevelId;
    float tileSize = 0.0f;
    TileLayer collisionLayer;
    std::vector<TileLayer> visualLayers;
    Vec2 playerSpawn{};
    std::vector<EntitySpawn> spawns;
    std::vector<TriggerDefinition> triggers;
    std::vector<std::string> objectives;
    PrototypeCatalog prototypes;
};

// 以下批次只表示本帧提案或候选，不会自行更改 World。
struct Intent {
    EntityId entity = 0;
    float horizontal = 0.0f;
    bool jump = false;
    bool attack = false;
};

struct IntentBatch {
    FrameId frame = 0;
    std::vector<Intent> items;
};

struct MotionItem {
    EntityId entity = 0;
    Vec2 proposedPosition{};
    Vec2 proposedVelocity{};
    Rect sweptBounds{};
};

struct MotionBatch {
    FrameId frame = 0;
    std::vector<MotionItem> items;
};

struct CollisionItem {
    EntityId entity = 0;
    Vec2 allowedPosition{};
    Vec2 normal{};
    bool grounded = false;
    bool hitCeiling = false;
};

struct CollisionBatch {
    FrameId frame = 0;
    std::vector<CollisionItem> items;
};

struct TriggerHit {
    EntityId entity = 0;
    std::string triggerId;
    TriggerKind kind = TriggerKind::Hazard;
};

struct TriggerBatch {
    FrameId frame = 0;
    std::vector<TriggerHit> items;
};

enum class ChangeKind : std::uint8_t {
    Position, Velocity, Facing, Health, Energy, Active, Grounded,
    JumpCount, InvulnerableUntil, AttackReadyFrame, AttackInstance,
    AIState, Target, PatrolPointIndex, EquippedAttack, Checkpoint,
    Inventory, Switch, Objective, LevelComplete
};

using ChangeValue = std::variant<
    Vec2, int, bool, std::string, std::uint64_t, AttackInstance, AIState>;

struct StateChange {
    EntityId target = 0; // 0 表示关卡或进度，而非某个实体。
    ChangeKind kind = ChangeKind::Position;
    ChangeValue value{};
    FrameId sourceFrame = 0;
    int priority = 0;
};

struct ChangeBatch {
    FrameId sourceFrame = 0;
    std::vector<StateChange> items;
};

enum class EventKind : std::uint8_t {
    AttackStarted, Hit, Hurt, Died, PickedUp, CheckpointActivated,
    LevelCompleted, SaveSucceeded, SaveFailed
};

struct DomainEvent {
    std::uint64_t id = 0;
    FrameId frame = 0;
    EventKind kind = EventKind::AttackStarted;
    EntityId source = 0;
    EntityId target = 0;
    Vec2 position{};
    int value = 0;
    std::string messageKey;
};

struct EventBatch {
    std::vector<DomainEvent> items;
};

struct DamageCandidate {
    EntityId source = 0;
    EntityId target = 0;
    std::uint64_t attackId = 0;
    int amount = 0;
    Vec2 knockback{};
};

struct CombatInput {
    WorldSnapshot before;
    CollisionBatch resolved;
    IntentBatch intents;
    LevelDefinition level;
};

struct CombatResult {
    ChangeBatch changes;
    std::vector<DamageCandidate> damage;
    EventBatch events;
};

struct AIResult {
    IntentBatch intents;
    ChangeBatch changes;
};

struct CommitResult {
    WorldSnapshot world;
    EventBatch events;
};

struct RenderEntity {
    EntityId id = 0;
    Vec2 position{};
    ResourceId spriteKey;
    int animationFrame = 0;
};

struct RenderSnapshot {
    FrameId frame = 0;
    LevelId levelId;
    Rect view{};
    std::vector<RenderEntity> entities;
    int playerHealth = 0;
    int playerEnergy = 0;
    std::string messageKey;
};

} // namespace knight
