"""Module/header/function specifications for the business-flow figures."""

from dataclasses import dataclass, field


MODULES = {
    "game": ("KnightGame.dll", "KnightGame.h", "src/game/KnightGame.h"),
    "internal": ("KnightGame.dll · 内部", "GamePrivate.h", "src/game/GamePrivate.h"),
    "state": ("KnightState.dll", "KnightState.h", "src/state/KnightState.h"),
    "map": ("KnightMap.dll", "KnightMap.h", "src/map/KnightMap.h"),
    "motion": ("KnightMotion.dll", "KnightMotion.h", "src/motion/KnightMotion.h"),
    "combat": ("KnightCombat.dll", "KnightCombat.h", "src/combat/KnightCombat.h"),
    "ai": ("KnightAI.dll", "KnightAI.h", "src/ai/KnightAI.h"),
    "content": ("KnightContent.dll", "KnightContent.h", "src/content/KnightContent.h"),
    "save": ("KnightSave.dll", "KnightSave.h", "src/save/KnightSave.h"),
}


@dataclass
class Step:
    title: str
    module: str
    functions: tuple[str, ...] = ()
    state: str = ""


def call(title, module, *functions):
    return Step(title, module, functions)


def internal(title, state):
    return Step(title, "internal", state=state)


@dataclass
class Figure:
    title: str
    steps: list[Step]
    context: str
    note: str
    failure_at: tuple[int, ...] = ()
    failure: str = ""
    conditions: dict[int, str] = field(default_factory=dict)


FIGURES = {
    "flow_startup": Figure("3.1 启动与主菜单", [
        call("创建业务会话", "game", "createGame"),
        call("打开存档库", "save", "openSaveStore"),
        call("读取槽位摘要", "save", "listSlots"),
        call("界面读取菜单", "game", "readRenderSnapshot", "takeEvents"),
    ], "入口：用户启动程序；尚未创建关卡世界。",
        "createGame() 内完成存档初始化；菜单与事件由界面主动读取。",
        (1, 2), "初始化失败：createGame() 返回错误，界面显示原因"),

    "flow_new_game": Figure("3.2a 新游戏：准备候选关卡", [
        call("读取完整首关", "content", "loadLevel"),
        call("建立候选世界", "state", "createWorld", "initializeWorld"),
        call("建立候选地图", "map", "createMap"),
    ], "入口：UI → KnightGame.h::submitCommand() / advanceElapsed()；Action::NewGame。",
        "准备成功后进入 3.2b；初始化存档参数为空，表示从首关开始。",
        (0, 1, 2), "任一步失败：清理候选句柄，留在主菜单"),
    "flow_new_game-activate": Figure("3.2b 新游戏：建档与切换", [
        call("创建进度槽位", "save", "createSlot"),
        internal("切换当前会话", "GameHandle / SessionState"),
        call("界面读取新画面", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.2a：候选世界与地图均已就绪。",
        "成功后 session = Playing；失败清理使用 3.13b 中的对应销毁接口。",
        (0,), "建档失败：释放候选世界与地图，保留主菜单"),

    "flow_continue": Figure("3.3a 继续游戏：读取与恢复", [
        call("读取存档副本", "save", "readSlot"),
        call("载入对应关卡", "content", "loadLevel"),
        call("恢复候选世界", "state", "createWorld", "initializeWorld"),
    ], "入口：UI → KnightGame.h::submitCommand() / advanceElapsed()；Action::ContinueGame。",
        "initializeWorld() 使用已校验的 SaveData；检查点与永久物品必须仍有效。",
        (0, 1, 2), "读取或恢复失败：保留主菜单，不覆盖原安全会话"),
    "flow_continue-activate": Figure("3.3b 继续游戏：启用关卡", [
        call("建立候选地图", "map", "createMap"),
        internal("切换当前会话", "GameHandle / SessionState"),
        call("界面读取恢复结果", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.3a：存档和候选世界已验证完成。",
        "地图成功后才切换到 Playing，并发布关卡进入事件。",
        (0,), "建图失败：释放候选世界，原会话保持安全"),

    "flow_movement": Figure("3.4a 左右移动：输入与位移裁决", [
        call("接收输入与计时", "game", "submitCommand", "advanceElapsed"),
        call("取得本帧旧世界", "state", "readWorld"),
        call("提出运动候选", "motion", "proposeMotion"),
        call("裁决地图阻挡", "map", "resolveMovement"),
    ], "方向键按下／释放更新按住状态；定时器唤醒业务层。",
        "左右同时按住抵消方向；裁决后在 3.4b 收尾并提交。"),
    "flow_movement-commit": Figure("3.4b 左右移动：收尾与提交", [
        call("根据裁决收尾", "motion", "finishMotion"),
        call("统一提交整帧", "state", "applyChanges"),
        call("界面读取结果", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.4a：使用 MotionBatch 与 CollisionBatch。",
        "每帧只有一次 applyChanges()；按住时下一帧继续，松开后停止方向意图。"),

    "flow_jump": Figure("3.5a 跳跃：输入与资格检查", [
        call("接收新按下", "game", "submitCommand", "advanceElapsed"),
        call("读取玩家状态", "state", "readWorld"),
        internal("检查跳跃资格", "InputState / GameHandle"),
    ], "入口：Action::Jump 的新按下边沿，持续按住不会重复申请。",
        "检查存活、动作限制、落地规则与剩余次数；通过后进入 3.5b。",
        (2,), "不允许跳跃：忽略本次请求，不产生跳跃变更"),
    "flow_jump-motion": Figure("3.5b 跳跃：运动计算与提交", [
        call("提出跳跃运动", "motion", "proposeMotion"),
        call("裁决撞顶与落地", "map", "resolveMovement"),
        call("计算最终运动", "motion", "finishMotion"),
        call("提交跳跃结果", "state", "applyChanges"),
    ], "承接 3.5a：跳跃资格检查已通过。",
        "提交后 UI 通过 KnightGame.h::readRenderSnapshot() / takeEvents() 读取反馈。"),

    "flow_moving_attack": Figure("3.6a 连续攻击：输入与资格检查", [
        call("接收移动与攻击", "game", "submitCommand", "advanceElapsed"),
        call("读取本帧旧世界", "state", "readWorld"),
        internal("合并本帧意图", "InputState / GameHandle"),
    ], "移动键持续按住；J 记录本帧新按下，两个输入可以并存。",
        "攻击合法则加入新攻击意图；不合法只保留移动及已有攻击，均进入 3.6b。"),
    "flow_moving_attack-motion": Figure("3.6b 连续攻击：修正站位与命中", [
        call("提出运动候选", "motion", "proposeMotion"),
        call("裁决阻挡与触发", "map", "resolveMovement", "queryTriggers"),
        call("修正最终站位", "motion", "finishMotion"),
        call("形成战斗候选", "combat", "evaluateAttacks", "evaluateTraps"),
    ], "承接 3.6a：意图与旧快照由业务层传递，各模块之间不直接调用。",
        "CombatInput 使用碰撞修正后的站位；同次挥击对同一目标只计一次命中。"),
    "flow_moving_attack-commit": Figure("3.6c 连续攻击：伤害与统一提交", [
        call("裁决伤害与无敌", "combat", "resolveDamage"),
        call("提交整帧候选", "state", "applyChanges"),
        call("界面读取反馈", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.6b：合并攻击、陷阱与运动候选后，一次提交世界。",
        "J 松开再按后回到 3.6a；冷却结束且资格满足，才能开始下一次挥击。"),

    "flow_enemy_ai": Figure("3.7a 敌人：决策与运动提案", [
        call("取得只读旧世界", "state", "readWorld"),
        call("生成敌人意图", "ai", "decideEnemies"),
        call("提出敌人运动", "motion", "proposeMotion"),
    ], "由 KnightGame.h::advanceElapsed() 驱动，每个固定逻辑帧执行。",
        "AI 返回移动／攻击意图和决策候选，不直接修改位置。"),
    "flow_enemy_ai-action": Figure("3.7b 敌人：执行与统一提交", [
        call("裁决敌人阻挡", "map", "resolveMovement"),
        call("收尾敌人运动", "motion", "finishMotion"),
        call("计算敌人攻击", "combat", "evaluateAttacks", "resolveDamage"),
        call("统一提交整帧", "state", "applyChanges"),
    ], "承接 3.7a：攻击使用修正站位，决策候选与运动、战斗一起合并。",
        "下一帧重新决策；离开脱离范围后按配置返回巡逻。"),

    "flow_trap_damage": Figure("3.8a 陷阱：触发与伤害候选", [
        call("查询危险区触发", "map", "queryTriggers"),
        call("计算陷阱候选", "combat", "evaluateTraps"),
        call("裁决最终伤害", "combat", "resolveDamage"),
    ], "地图报告危险区命中；业务层传递陷阱规则与玩家旧状态。",
        "按机关相位、重复伤害间隔与无敌截止帧裁决；周期机关使用逻辑时间。"),
    "flow_trap_damage-commit": Figure("3.8b 陷阱：死亡优先与提交", [
        internal("合并并检查死亡", "GameHandle / SessionState"),
        call("提交整帧候选", "state", "applyChanges"),
        call("界面读取伤亡反馈", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.8a：致命伤害优先取消同帧拾取、检查点与出口候选。",
        "提交后生命归零则 SessionState::Dead；提交失败不发布成功事件。"),

    "flow_pickup_checkpoint": Figure("3.9a 交互：拾取与检查点提交", [
        call("查询交互触发区", "map", "queryTriggers"),
        internal("检查并生成变更", "InputState / GameHandle"),
        call("统一提交交互", "state", "applyChanges"),
    ], "入口：UI → KnightGame.h::submitCommand() / advanceElapsed()；Action::Interact。",
        "检查距离、存活与对象条件；需持久化时进入 3.9b。",
        (1,), "条件未满足或玩家死亡：不提交本次交互候选"),
    "flow_pickup_checkpoint-save": Figure("3.9b 交互：保存已提交进度", [
        call("事务写入进度", "save", "writeSlot"),
        call("界面读取反馈", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.9a：SaveData 来自 applyChanges() 返回的 CommitResult.world。",
        "共享类型声明于 KnightContracts.h；检查点或关键物品才需要持久化。",
        (0,), "写入失败：保留内存进度，反馈尚未保存并允许重试"),

    "flow_next_level-completion": Figure("3.10a 完成本关：目标与存档", [
        call("查询出口命中", "map", "queryTriggers"),
        internal("检查出口目标", "GameHandle / SessionState"),
        call("提交本关完成", "state", "applyChanges"),
        call("保存完成进度", "save", "writeSlot"),
    ], "目标需检查存活、必要物品、指定敌人与出口开启条件。",
        "存档副本来自 CommitResult.world；写盘失败保留内存中的已完成状态。",
        (1,), "目标未完成：提示出口未开启，继续当前关卡", {1: "满足"}),
    "flow_next_level": Figure("3.10b 下一关：准备候选资源", [
        call("载入并校验新关", "content", "loadLevel"),
        call("建立候选新世界", "state", "createWorld", "initializeWorld"),
        call("建立候选新地图", "map", "createMap"),
    ], "入口：UI → KnightGame.h::submitCommand() / advanceElapsed()；Action::NextLevel。",
        "本关已完成；三个步骤均成功后进入 3.10c。",
        (0, 1, 2), "准备失败：清理候选，保留已完成的旧关卡供重试"),
    "flow_next_level-activate": Figure("3.10c 下一关：切换与旧资源释放", [
        internal("切换当前关卡", "GameHandle / SessionState"),
        call("释放旧地图", "map", "destroyMap"),
        call("释放旧世界", "state", "destroyWorld"),
        call("界面读取新关", "game", "readRenderSnapshot", "takeEvents"),
    ], "承接 3.10b：新关候选世界与地图均已就绪，再替换当前句柄。",
        "销毁接口只接收旧句柄；候选失败时不会释放旧关卡。"),

    "flow_pause_focus": Figure("3.11 暂停、继续与失焦", [
        call("提交暂停或释放", "game", "submitCommand"),
        call("控制逻辑帧推进", "game", "advanceElapsed"),
        internal("更新会话与输入", "InputState / SessionState"),
        call("界面读取当前画面", "game", "readRenderSnapshot", "takeEvents"),
    ], "暂停／继续命令以及失焦后的释放命令，都从界面进入业务层。",
        "暂停：停止世界计时；继续：清空旧输入并重置计时；失焦：释放按住动作。"),

    "flow_respawn": Figure("3.12 复活或重试", [
        call("提交复活或重试", "game", "submitCommand", "advanceElapsed"),
        internal("验证复活状态", "GameHandle / SessionState"),
        call("重建检查点世界", "state", "resetAtCheckpoint"),
        call("界面读取复活结果", "game", "readRenderSnapshot", "takeEvents"),
    ], "使用当前 LevelDefinition 与最近检查点进度；同关地图索引可复用。",
        "重建玩家、敌人与陷阱；按进度保留永久能力与关键物品。",
        (1, 2), "校验或重建失败：保留安全状态，显示错误供重试"),

    "flow_save_exit": Figure("3.13a 保存并返回菜单", [
        call("接收返回菜单命令", "game", "submitCommand", "advanceElapsed"),
        call("读取已提交世界", "state", "readWorld"),
        call("事务写入存档", "save", "writeSlot"),
        internal("切回主菜单", "GameHandle / SessionState"),
    ], "Action::SaveAndReturn 先停止玩法推进；从已提交世界制作 SaveData。",
        "只有保存成功才释放本关句柄；对应销毁接口见 3.13b。",
        (2,), "保存失败：保留当前会话及内存进度，提示重试", {2: "成功"}),
    "flow_save_exit-cleanup": Figure("3.13b 程序退出：按所有权释放", [
        call("界面销毁业务会话", "game", "destroyGame"),
        call("销毁地图句柄", "map", "destroyMap"),
        call("销毁世界句柄", "state", "destroyWorld"),
        call("关闭存档连接", "save", "closeSaveStore"),
    ], "入口：从主菜单退出程序；destroyGame() 安排各模块释放自己的句柄。",
        "返回菜单仅销毁本关世界与地图，存档连接在退出程序时关闭。"),
}


SCENARIOS = [
    ("3.1 启动程序并显示主菜单", ["flow_startup"],
     "启动时建立业务会话并读取存档摘要，成功后显示主菜单。初始化失败显示原因；此时尚未创建玩家世界。"),
    ("3.2 鼠标点击“开始游戏”", ["flow_new_game", "flow_new_game-activate"],
     "先准备首关的候选世界与地图，再创建存档并切换会话。任何准备步骤失败都清理候选，保留主菜单。"),
    ("3.3 点击“继续游戏”并选择存档", ["flow_continue", "flow_continue-activate"],
     "验证存档与关卡引用后恢复最近检查点，全部资源就绪后才切换到游玩。损坏存档或载入失败不会覆盖原安全会话。"),
    ("3.4 按下、按住和松开左右移动键", ["flow_movement", "flow_movement-commit"],
     "按住状态在每个逻辑帧形成移动意图；碰撞修正后统一提交。松开后停止该方向意图，左右同时按住则抵消。"),
    ("3.5 按下跳跃键", ["flow_jump", "flow_jump-motion"],
     "新按下先检查跳跃资格，合法才参与运动计算。持续按住不会反复申请，撞顶、落地与次数变化随整帧提交。"),
    ("3.6 按住移动键时按 J 攻击，以及再次攻击", ["flow_moving_attack", "flow_moving_attack-motion", "flow_moving_attack-commit"],
     "移动按住与攻击新按下独立记录，先修正站位再判定攻击。J 松开再按且冷却结束后可再次攻击；一次挥击不重复伤害同一目标。"),
    ("3.7 敌人巡逻、发现玩家并攻击", ["flow_enemy_ai", "flow_enemy_ai-action"],
     "敌人从旧快照产生意图，运动、战斗和决策状态在同一帧统一提交。敌人模块不直接移动实体。"),
    ("3.8 碰到陷阱、受伤和死亡", ["flow_trap_damage", "flow_trap_damage-commit"],
     "伤害受机关相位、再次伤害间隔和无敌时间限制。致命伤害取消同帧交互候选，提交后再进入死亡状态。"),
    ("3.9 拾取物品、激活检查点", ["flow_pickup_checkpoint", "flow_pickup_checkpoint-save"],
     "交互先验证并提交，需要持久化时才保存已提交进度。保存失败保留内存中的变化，并提示尚未保存。"),
    ("3.10 到达出口、完成目标并进入下一关", ["flow_next_level-completion", "flow_next_level", "flow_next_level-activate"],
     "出口条件满足后提交本关完成并请求保存；选择下一关时另行准备候选资源。新关全部就绪后切换，失败保留已完成的旧关。"),
    ("3.11 暂停、继续与窗口失焦", ["flow_pause_focus"],
     "暂停停止世界计时，继续清空旧输入并重置计时基准。窗口失焦释放按住动作，获焦后需重新按键。"),
    ("3.12 死亡后复活或菜单中选择重试", ["flow_respawn"],
     "验证检查点后重建玩家、敌人与陷阱，保留永久能力和关键物品。同关地图可复用，失败时保留安全状态。"),
    ("3.13 保存并返回菜单、退出程序", ["flow_save_exit", "flow_save_exit-cleanup"],
     "保存成功后才返回菜单并释放本关资源。真正退出程序时再销毁业务会话与存档连接，各句柄由所属模块释放。"),
]
