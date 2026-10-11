/**
 * @file KnightContracts.h
 * @brief 跨 DLL 共享的值类型、枚举及返回结果。
 *
 * 本文件不生成 DLL，也不暴露私有对象。字符串使用 UTF-8，坐标向右、向下为正。
 * 快照和批次均为值副本；计算模块返回候选，由业务层合并后交给状态模块提交。
 * 含 STL 容器的契约要求所有 DLL 使用相同的编译器、运行时、位数和构建配置。
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace knight {

/// @brief 世界内唯一的实体编号；0 表示无效实体或无目标。
using EntityId = std::uint64_t;

/// @brief 固定逻辑帧编号；不是实际时间或绘制帧编号。
using FrameId = std::uint64_t;

/// @brief 业务层分配的命令编号；0 表示尚未分配。
using CommandId = std::uint64_t;

/// @brief 持久化存档槽位编号；0 表示未选择槽位。
using SlotId = std::int64_t;

/// @brief 稳定的关卡标识，不是任意文件路径。
using LevelId = std::string;

/// @brief 原型、图片、攻击等资源的稳定标识。
using ResourceId = std::string;

/// @brief 内容根目录的 UTF-8 路径。
using ContentRoot = std::string;

/// @brief 当前用户存档数据库的 UTF-8 文件路径。
using DatabasePath = std::string;

/// @brief 二维世界坐标或向量；x 向右、y 向下为正，数值须有限。
struct Vec2 {
    float x = 0.0f; ///< 横向分量，向右为正。
    float y = 0.0f; ///< 纵向分量，向下为正。
};

/// @brief 轴对齐矩形；实体碰撞盒采用局部坐标，地图触发区采用世界坐标，宽高须大于零。
struct Rect {
    float x = 0.0f; ///< 左上角横坐标，坐标系由矩形用途决定。
    float y = 0.0f; ///< 左上角纵坐标。
    float width = 0.0f; ///< 矩形宽度。
    float height = 0.0f; ///< 矩形高度。
};

/// @brief 跨 DLL 统一错误码；调用方须先检查是否为 Ok。
enum class StatusCode : std::uint8_t {
    Ok = 0, ///< 操作成功。
    InvalidArgument, ///< 参数为空、越界或不符合约定。
    InvalidState, ///< 当前会话或世界状态不允许此操作。
    NotFound, ///< 所需关卡、资源、实体或存档不存在。
    InvalidFormat, ///< 数据结构或文件内容不合法。
    VersionMismatch, ///< 共享契约、关卡或存档版本不受支持。
    IoFailure, ///< 文件或数据库读写失败。
    OutOfMemory, ///< 所需内存分配失败。
    InternalFailure ///< 所属模块的其他内部错误。
};

/// @brief 无额外返回值的操作状态，以及可供诊断的 UTF-8 错误消息。
struct Status {
    StatusCode code = StatusCode::Ok; ///< 操作状态；默认 Ok。
    std::array<char, 256> messageUtf8{}; ///< 以零字节结尾的 UTF-8 诊断消息，容量包含终止符。
};

/// @brief 带状态的值结果；仅 status.code == StatusCode::Ok 时才允许读取 value。
template<class T>
struct Result {
    Status status{}; ///< 先检查此状态，再决定是否读取 value。
    T value{}; ///< 成功时的值副本或不透明句柄；失败时不可使用。
};

/// @brief 与物理键位无关的业务动作，由界面完成键位到动作的映射。
enum class Action : std::uint8_t {
    MoveLeft, ///< 向左移动。
    MoveRight, ///< 向右移动。
    Jump, ///< 申请跳跃，使用新按下边沿。
    Attack, ///< 申请攻击，使用新按下边沿。
    Interact, ///< 申请拾取、检查点或出口等交互。
    Pause, ///< 暂停世界推进。
    Resume, ///< 从暂停状态继续。
    NewGame, ///< 创建首关候选资源和新存档。
    ContinueGame, ///< 从指定存档恢复进度。
    Retry, ///< 从最近检查点重试。
    SaveAndReturn, ///< 保存成功后返回主菜单。
    NextLevel, ///< 载入并切换到下一关。
    Exit ///< 请求退出程序。
};

/// @brief 命令的输入阶段；持续动作区分按下和释放，菜单使用一次性触发。
enum class CommandPhase : std::uint8_t {
    Press, ///< 物理按键新按下，自动重复不算新按下。
    Release, ///< 物理按键释放，失焦时也应补发释放。
    Trigger ///< 一次性菜单或鼠标命令。
};

/// @brief 实体类别，决定状态模块应具备哪些运行组件。
enum class EntityKind : std::uint8_t {
    Player, ///< 玩家角色。
    Enemy, ///< 敌人角色。
    Pickup ///< 可拾取物品。
};

/// @brief 世界中保存的敌人决策状态，不由 AI DLL 私自持有。
enum class AIState : std::uint8_t {
    None, ///< 无决策状态。
    Patrol, ///< 按巡逻点移动。
    Alert, ///< 发现目标后的警戒。
    Chase, ///< 追逐目标。
    Attack, ///< 进入攻击决策。
    Return ///< 脱离追击后返回。
};

/// @brief 一次挥击的阶段；仅 Active 阶段参与攻击命中判定。
enum class AttackPhase : std::uint8_t {
    None, ///< 无有效挥击阶段。
    Windup, ///< 起手准备。
    Active, ///< 有效攻击判定期。
    Recovery ///< 收招恢复。
};

/// @brief 静态地图触发区的业务类别。
enum class TriggerKind : std::uint8_t {
    Hazard, ///< 危险区或陷阱。
    Pickup, ///< 拾取物交互区。
    Checkpoint, ///< 检查点。
    Door, ///< 门或机关交互区。
    Exit ///< 关卡出口。
};

/// @brief 界面提交的业务命令；界面填写动作等输入，业务层分配编号和目标帧。
struct PlayerCommand {
    CommandId id = 0; ///< 业务层分配的命令编号，界面提交时无需分配。
    Action action = Action::MoveLeft; ///< 动作语义，不携带具体键位。
    CommandPhase phase = CommandPhase::Press; ///< 按下、释放或一次性触发。
    float direction = 0.0f; ///< 方向输入，只取 -1、0、1。
    SlotId slotId = 0; ///< 继续游戏等命令指定的槽位，0 表示未选槽。
    FrameId targetFrame = 0; ///< 业务层安排命令生效的逻辑帧。
};

/// @brief 创建业务会话所需的路径、首关及共享契约版本。
struct GameConfig {
    ContentRoot contentRoot; ///< 静态关卡和原型资源的根目录。
    DatabasePath databasePath; ///< 当前用户存档数据库路径。
    LevelId firstLevelId; ///< 新游戏的首关编号。
    std::uint32_t contractVersion = 1; ///< 共享接口契约版本，首版为 1。
};

/// @brief 世界持有的一次跨帧挥击，记录已命中目标以防同一挥击重复扣血。
struct AttackInstance {
    std::uint64_t id = 0; ///< 标识本次挥击，用于跨帧追踪。
    EntityId owner = 0; ///< 发起挥击的实体编号。
    FrameId startedFrame = 0; ///< 挥击开始的逻辑帧。
    AttackPhase phase = AttackPhase::None; ///< 当前挥击阶段。
    std::vector<EntityId> hitTargets; ///< 本次挥击已经命中的实体编号，用于去重。
};

/// @brief 实体状态的独立传输副本；修改副本不会改变 World，非角色字段按约定置零。
struct EntitySnapshot {
    EntityId id = 0; ///< 本实体编号。
    EntityId target = 0; ///< 敌人当前目标编号，0 表示无目标。
    EntityKind kind = EntityKind::Player; ///< 实体类别。
    Vec2 position{}; ///< 实体世界位置。
    Vec2 velocity{}; ///< 当前速度向量。
    Rect collider{}; ///< 相对实体位置的碰撞盒。
    int health = 0; ///< 当前生命值。
    int energy = 0; ///< 当前能量值。
    int remainingJumps = 0; ///< 玩家剩余跳跃次数。
    int facing = 1; ///< 横向朝向：-1 向左，1 向右。
    bool active = true; ///< 是否仍启用；拾取后物品为 false。
    bool grounded = false; ///< 本帧已确认的落地状态。
    FrameId invulnerableUntil = 0; ///< 受击无敌的截止逻辑帧。
    FrameId attackReadyFrame = 0; ///< 下一次攻击可申请的逻辑帧。
    std::uint64_t patrolPointIndex = 0; ///< 敌人当前巡逻点下标。
    AIState aiState = AIState::None; ///< 敌人当前决策状态。
    ResourceId prototypeId; ///< 对应数值原型编号。
    ResourceId spriteKey; ///< 界面使用的图片资源键。
    ResourceId equippedAttackId; ///< 当前装备的攻击原型编号。
};

/// @brief 跨关或跨会话恢复的进度；不保存实时坐标、速度或正在进行的挥击。
struct ProgressState {
    LevelId levelId; ///< 恢复进度所对应的关卡编号。
    std::string checkpointId; ///< 最近激活的检查点编号。
    int health = 0; ///< 需要持久化的生命进度。
    int energy = 0; ///< 需要持久化的能量进度。
    std::vector<std::string> abilities; ///< 已解锁能力标识。
    std::vector<std::string> keyItems; ///< 已取得的关键物品标识。
    std::vector<std::string> permanentSwitches; ///< 已激活的永久机关标识。
    std::vector<std::string> clearedObjectives; ///< 已完成的目标标识。
};

/// @brief 状态模块生成的完整世界副本，作为一帧计算的只读起点。
struct WorldSnapshot {
    LevelId levelId; ///< 当前世界的关卡编号。
    FrameId frame = 0; ///< 该快照已提交的逻辑帧号。
    std::vector<EntitySnapshot> entities; ///< 所有实体的独立状态副本。
    std::vector<AttackInstance> attacks; ///< 世界持有的跨帧挥击实例副本。
    ProgressState progress; ///< 可持久化的进度副本。
    bool levelComplete = false; ///< 当前关卡是否已完成。
};

/// @brief 存档进度值对象；必须从已提交世界生成，不是完整现场快照。
struct SaveData {
    std::uint32_t formatVersion = 1; ///< 存档值格式版本，首版为 1。
    SlotId slotId = 0; ///< 本次读取或写入的槽位编号。
    ProgressState progress; ///< 检查点、能力和关键物品等持久进度。
};

/// @brief 创建新槽位时写入的初始进度。
struct SlotSeed {
    ProgressState initialProgress; ///< 从候选首关生成的初始进度。
};

/// @brief 主菜单列出存档槽位所需的摘要。
struct SlotSummary {
    SlotId slotId = 0; ///< 槽位编号。
    LevelId levelId; ///< 存档对应关卡。
    std::string checkpointId; ///< 存档对应检查点。
    std::uint64_t updatedEpochSeconds = 0; ///< 最后更新时间，Unix 纪元秒数。
};

/// @brief 存档槽位摘要列表，不携带数据库连接。
struct SlotSummaryBatch {
    std::vector<SlotSummary> slots; ///< 菜单可展示的槽位摘要。
};

/// @brief 静态玩家原型；运行中的生命、速度和冷却不得写回此结构。
struct PlayerRules {
    float moveSpeed = 0.0f; ///< 水平移动速度参数。
    float jumpSpeed = 0.0f; ///< 跳跃速度参数，起跳方向由运动规则处理。
    float gravity = 0.0f; ///< 重力加速度参数。
    float maxFallSpeed = 0.0f; ///< 下落速度上限。
    int maxJumps = 0; ///< 最大跳跃次数。
    int maxHealth = 0; ///< 生命上限。
    int maxEnergy = 0; ///< 能量上限。
    Rect collider{}; ///< 默认局部碰撞盒。
    ResourceId spriteKey; ///< 默认图片资源键。
    ResourceId defaultAttackId; ///< 默认攻击原型编号。
};

/// @brief 静态敌人原型及索敌、移动和攻击参数。
struct EnemyRules {
    float moveSpeed = 0.0f; ///< 敌人水平移动速度。
    float detectRadius = 0.0f; ///< 索敌距离。
    float attackRange = 0.0f; ///< 可发起攻击的距离。
    FrameId attackCooldown = 0; ///< 攻击冷却时长，单位为逻辑帧。
    int maxHealth = 0; ///< 生命上限。
    int maxEnergy = 0; ///< 能量上限。
    Rect collider{}; ///< 默认局部碰撞盒。
    ResourceId spriteKey; ///< 默认图片资源键。
    ResourceId attackId; ///< 敌人使用的攻击原型编号。
    ResourceId behaviorId; ///< 敌人行为配置编号。
};

/// @brief 攻击判定框、伤害与各阶段持续时间。
struct AttackRules {
    Rect hitBox{}; ///< 相对攻击者的判定框，由朝向和修正站位确定世界区域。
    int damage = 0; ///< 基础伤害值。
    FrameId windupFrames = 0; ///< 起手持续逻辑帧数。
    FrameId activeFrames = 0; ///< 有效判定持续逻辑帧数。
    FrameId recoveryFrames = 0; ///< 收招持续逻辑帧数。
    FrameId cooldownFrames = 0; ///< 再次发起攻击所需冷却帧数。
};

/// @brief 陷阱伤害与按逻辑帧计算的周期参数。
struct TrapRules {
    int damage = 0; ///< 陷阱基础伤害。
    FrameId periodFrames = 0; ///< 机关完整周期，单位为逻辑帧。
    FrameId activeFrames = 0; ///< 每周期的危险持续逻辑帧数。
};

/// @brief 内容模块解析完成的数值原型集合，只包含共享值类型。
struct PrototypeCatalog {
    PlayerRules player; ///< 玩家数值原型。
    std::unordered_map<ResourceId, EnemyRules> enemies; ///< 按资源编号查找敌人原型。
    std::unordered_map<ResourceId, AttackRules> attacks; ///< 按资源编号查找攻击原型。
    std::unordered_map<ResourceId, TrapRules> traps; ///< 按资源编号查找陷阱原型。
};

/// @brief 按行存储的图块层，tiles 数量应等于 width * height。
struct TileLayer {
    int width = 0; ///< 图层列数。
    int height = 0; ///< 图层行数。
    std::vector<int> tiles; ///< 按行排列的图块；下标为 row * width + column。
};

/// @brief 关卡中敌人或拾取物的初始定义，不是运行中的实体。
struct EntitySpawn {
    EntityId id = 0; ///< 世界内唯一的初始实体编号。
    EntityKind kind = EntityKind::Enemy; ///< 初始实体类别，通常为敌人或拾取物。
    ResourceId prototypeId; ///< 要应用的数值原型编号。
    ResourceId spriteKey; ///< 图片资源键。
    Vec2 position{}; ///< 出生时的世界坐标。
    Rect collider{}; ///< 拾取物必须填写；敌人未指定时使用原型碰撞盒。
    std::vector<Vec2> patrolPoints; ///< 敌人巡逻点，使用世界坐标；拾取物不使用。
};

/// @brief 关卡内唯一的触发区定义，拾取区通过 linkedEntity 关联物品实体。
struct TriggerDefinition {
    std::string id; ///< 关卡内唯一的触发区编号。
    TriggerKind kind = TriggerKind::Hazard; ///< 危险、物品、检查点、门或出口类别。
    Rect area{}; ///< 世界坐标中的触发矩形。
    ResourceId prototypeId; ///< 关联的陷阱、物品或机关原型编号。
    EntityId linkedEntity = 0; ///< 拾取区关联的物品实体，其他触发区为 0。
};

/// @brief 内容模块完成解析、资源引用解析和验证后的完整静态关卡。
struct LevelDefinition {
    std::uint32_t formatVersion = 1; ///< 关卡文件格式版本。
    LevelId id; ///< 本关稳定编号。
    LevelId nextLevelId; ///< 下一关编号，最后一关可为空。
    float tileSize = 0.0f; ///< 单个图块的世界尺寸。
    TileLayer collisionLayer; ///< 用于阻挡裁决的静态图层。
    std::vector<TileLayer> visualLayers; ///< 仅用于场景绘制的图层。
    Vec2 playerSpawn{}; ///< 新关玩家出生位置。
    std::vector<EntitySpawn> spawns; ///< 初始敌人和拾取物定义。
    std::vector<TriggerDefinition> triggers; ///< 危险区和交互区定义。
    std::vector<std::string> objectives; ///< 开启出口所需的目标标识。
    PrototypeCatalog prototypes; ///< 已完成资源引用解析的数值原型。
};

/// @brief 单个实体本帧希望执行的动作，不直接修改世界。
struct Intent {
    EntityId entity = 0; ///< 动作对应的实体编号。
    float horizontal = 0.0f; ///< 水平意图：-1 向左，0 无方向，1 向右。
    bool jump = false; ///< 本帧是否申请跳跃。
    bool attack = false; ///< 本帧是否申请新攻击。
};

/// @brief 同一逻辑帧的玩家和敌人动作意图。
struct IntentBatch {
    FrameId frame = 0; ///< 意图所属的目标逻辑帧。
    std::vector<Intent> items; ///< 本帧所有实体动作。
};

/// @brief 单个实体未经地图阻挡裁决的建议位置、速度和扫过区域。
struct MotionItem {
    EntityId entity = 0; ///< 运动实体编号。
    Vec2 proposedPosition{}; ///< 未裁决地图阻挡的建议世界位置。
    Vec2 proposedVelocity{}; ///< 运动规则计算的建议速度。
    Rect sweptBounds{}; ///< 本帧运动扫过的世界区域，用于缩小碰撞候选。
};

/// @brief 同一帧的运动提案，由协调器转交地图模块。
struct MotionBatch {
    FrameId frame = 0; ///< 运动提案所属逻辑帧。
    std::vector<MotionItem> items; ///< 本帧各实体的运动提案。
};

/// @brief 地图对单个实体运动的裁决，不包含已提交的状态变化。
struct CollisionItem {
    EntityId entity = 0; ///< 被裁决实体编号。
    Vec2 allowedPosition{}; ///< 阻挡裁决后允许到达的位置。
    Vec2 normal{}; ///< 碰撞法线。
    bool grounded = false; ///< 裁决后是否落地。
    bool hitCeiling = false; ///< 裁决中是否撞顶。
};

/// @brief 同一帧的地图裁决结果，用于运动收尾、触发查询与战斗。
struct CollisionBatch {
    FrameId frame = 0; ///< 碰撞裁决所属逻辑帧。
    std::vector<CollisionItem> items; ///< 各实体允许位置与接触标志。
};

/// @brief 实体在裁决后位置命中的一个触发区。
struct TriggerHit {
    EntityId entity = 0; ///< 进入或重叠触发区的实体。
    std::string triggerId; ///< 命中的关卡触发区编号。
    TriggerKind kind = TriggerKind::Hazard; ///< 触发区类别。
};

/// @brief 同一帧的触发区命中列表，仅表示检测事实。
struct TriggerBatch {
    FrameId frame = 0; ///< 触发查询所属逻辑帧。
    std::vector<TriggerHit> items; ///< 危险区及交互区命中事实。
};

/// @brief 状态变更字段及其载荷类型；状态模块提交前校验 kind/value 匹配。
enum class ChangeKind : std::uint8_t {
    Position, ///< 位置，载荷 Vec2。
    Velocity, ///< 速度，载荷 Vec2。
    Facing, ///< 朝向，载荷 int。
    Health, ///< 生命值，载荷 int。
    Energy, ///< 能量值，载荷 int。
    Active, ///< 是否启用，载荷 bool；拾取后停用物品。
    Grounded, ///< 是否落地，载荷 bool。
    JumpCount, ///< 剩余跳跃次数，载荷 int。
    InvulnerableUntil, ///< 无敌截止帧，载荷 uint64_t。
    AttackReadyFrame, ///< 攻击就绪帧，载荷 uint64_t。
    AttackInstance, ///< 挥击实例，载荷 knight::AttackInstance。
    AIState, ///< 敌人决策状态，载荷 knight::AIState。
    Target, ///< 目标实体编号，载荷 uint64_t。
    PatrolPointIndex, ///< 巡逻点下标，载荷 uint64_t。
    EquippedAttack, ///< 装备攻击编号，载荷 string。
    Checkpoint, ///< 检查点编号，载荷 string。
    Inventory, ///< 物品标识，载荷 string。
    Switch, ///< 永久机关标识，载荷 string。
    Objective, ///< 已完成目标标识，载荷 string。
    LevelComplete ///< 关卡完成标记，载荷 bool。
};

/// @brief 变更载荷；实际类型必须与 ChangeKind 对应，错误由 applyChanges 整批拒绝。
using ChangeValue = std::variant<
    Vec2, int, bool, std::string, std::uint64_t, AttackInstance, AIState>;

/// @brief 一条待提交的状态候选；target == 0 表示关卡或进度字段。
struct StateChange {
    EntityId target = 0; ///< 目标实体；0 表示关卡或持久进度。
    ChangeKind kind = ChangeKind::Position; ///< 要修改的字段，同时决定 value 的合法类型。
    ChangeValue value{}; ///< 对应字段的变更载荷，类型须匹配 kind。
    FrameId sourceFrame = 0; ///< 产生此候选的目标帧，须与批次来源帧一致。
    int priority = 0; ///< 同帧冲突的优先级，供协调器稳定排序和合并。
};

/// @brief 业务层合并的整帧候选，sourceFrame 必须为当前世界帧号加一。
struct ChangeBatch {
    FrameId sourceFrame = 0; ///< 必须等于当前 WorldSnapshot.frame + 1。
    std::vector<StateChange> items; ///< 本帧合并后的变更；整批校验、整批提交。
};

/// @brief 一次性业务反馈类别；玩法事件提交后发布，存档事件在数据库返回后发布。
enum class EventKind : std::uint8_t {
    AttackStarted, ///< 开始一次挥击。
    Hit, ///< 攻击命中。
    Hurt, ///< 实体受伤。
    Died, ///< 实体死亡。
    PickedUp, ///< 完成拾取。
    CheckpointActivated, ///< 检查点已激活。
    LevelCompleted, ///< 本关完成。
    SaveSucceeded, ///< 进度已写入数据库。
    SaveFailed ///< 保存失败，需提示用户。
};

/// @brief 一次业务事件；候选事件须经提交确认并由协调器编号后才能供界面消费。
struct DomainEvent {
    std::uint64_t id = 0; ///< 协调器发布时分配的事件编号，供一次性消费追踪。
    FrameId frame = 0; ///< 事件所属逻辑帧。
    EventKind kind = EventKind::AttackStarted; ///< 事件语义。
    EntityId source = 0; ///< 来源实体编号，非实体事件可为 0。
    EntityId target = 0; ///< 目标实体编号，非实体事件可为 0。
    Vec2 position{}; ///< 反馈发生的世界位置。
    int value = 0; ///< 随事件种类解释的数值，例如伤害量。
    std::string messageKey; ///< 界面提示使用的消息键，不是显示文本。
};

/// @brief 事件值列表；是否为候选由所在返回结构和调用阶段决定。
struct EventBatch {
    std::vector<DomainEvent> items; ///< 本次返回或待发布的事件列表。
};

/// @brief 尚未扣血的命中事实，等待去重及无敌规则裁决。
struct DamageCandidate {
    EntityId source = 0; ///< 伤害来源实体。
    EntityId target = 0; ///< 待受伤实体。
    std::uint64_t attackId = 0; ///< 挥击实例编号；陷阱伤害为 0。
    int amount = 0; ///< 待裁决的伤害量。
    Vec2 knockback{}; ///< 待裁决的击退向量。
};

/// @brief 协调器组装的战斗输入；旧世界提供状态，地图裁决提供修正站位。
struct CombatInput {
    WorldSnapshot before; ///< 本帧开始时的已提交世界副本。
    CollisionBatch resolved; ///< 地图裁决后的站位副本，攻击空间判定使用此结果。
    IntentBatch intents; ///< 本帧玩家和敌人动作意图。
    LevelDefinition level; ///< 攻击、陷阱原型等完整关卡规则。
};

/// @brief 战斗模块输出的状态、伤害和事件候选，不代表世界已改变。
struct CombatResult {
    ChangeBatch changes; ///< 攻击阶段、冷却、生命等尚未提交的候选。
    std::vector<DamageCandidate> damage; ///< 待统一去重和无敌裁决的命中事实。
    EventBatch events; ///< 未确认的战斗事件，世界提交失败时不得发布。
};

/// @brief 敌人决策产生的意图和决策状态候选，交由协调器合并。
struct AIResult {
    IntentBatch intents; ///< 敌人的移动或攻击请求。
    ChangeBatch changes; ///< 决策状态、目标和巡逻点等候选变更。
};

/// @brief 状态提交成功后的新世界快照及提交事件。
struct CommitResult {
    WorldSnapshot world; ///< 整批提交成功后的完整世界副本。
    EventBatch events; ///< 由状态提交确认的事件，供协调器合并发布。
};

/// @brief 业务层提供给界面的实体绘制数据，不暴露权威实体对象。
struct RenderEntity {
    EntityId id = 0; ///< 用于对应绘制对象的实体编号。
    Vec2 position{}; ///< 实体绘制位置。
    ResourceId spriteKey; ///< 图片资源键。
    int animationFrame = 0; ///< 界面要展示的动画帧下标。
};

/// @brief 供界面读取的完整画面副本，与一次性事件队列分开消费。
struct RenderSnapshot {
    FrameId frame = 0; ///< 画面对应的已提交逻辑帧。
    LevelId levelId; ///< 当前展示关卡编号。
    Rect view{}; ///< 当前视口世界矩形。
    std::vector<RenderEntity> entities; ///< 可绘制实体列表。
    int playerHealth = 0; ///< 生命 UI 显示值。
    int playerEnergy = 0; ///< 能量 UI 显示值。
    std::string messageKey; ///< 菜单或状态提示消息键。
};

} // namespace knight
