# Qt Knight 论文风格示意图

当前共 29 张图：4 张总览图和 25 张业务调用图。业务图直接标注 DLL、头文件和函数，复杂场景分图连续展示，每张保留 2–4 个调用节点。“内部逻辑”节点只描述私有状态处理，不表示新增函数。

同名 PNG 用于 Markdown，SVG 可缩放，VSDX 由可编辑的 Visio 原生形状、文字和线段组成。

| 图 | PNG | SVG | Visio |
|---|---|---|---|
| 系统总体架构 | [PNG](architecture-overview-paper.png) | [SVG](architecture-overview-paper.svg) | [VSDX](architecture-overview-paper.vsdx) |
| 单帧业务逻辑数据流 | [PNG](business-data-flow-paper.png) | [SVG](business-data-flow-paper.svg) | [VSDX](business-data-flow-paper.vsdx) |
| 数据归属 | [PNG](data-ownership-paper.png) | [SVG](data-ownership-paper.svg) | [VSDX](data-ownership-paper.vsdx) |
| 1.1 调用方向 | [PNG](flow_overview-paper.png) | [SVG](flow_overview-paper.svg) | [VSDX](flow_overview-paper.vsdx) |
| 3.1 启动与主菜单 | [PNG](flow_startup-compact.png) | [SVG](flow_startup-compact.svg) | [VSDX](flow_startup-compact.vsdx) |
| 3.2a 新游戏：准备候选关卡 | [PNG](flow_new_game-compact.png) | [SVG](flow_new_game-compact.svg) | [VSDX](flow_new_game-compact.vsdx) |
| 3.2b 新游戏：建档与切换 | [PNG](flow_new_game-activate-compact.png) | [SVG](flow_new_game-activate-compact.svg) | [VSDX](flow_new_game-activate-compact.vsdx) |
| 3.3a 继续游戏：读取与恢复 | [PNG](flow_continue-compact.png) | [SVG](flow_continue-compact.svg) | [VSDX](flow_continue-compact.vsdx) |
| 3.3b 继续游戏：启用关卡 | [PNG](flow_continue-activate-compact.png) | [SVG](flow_continue-activate-compact.svg) | [VSDX](flow_continue-activate-compact.vsdx) |
| 3.4a 左右移动：输入与位移裁决 | [PNG](flow_movement-compact.png) | [SVG](flow_movement-compact.svg) | [VSDX](flow_movement-compact.vsdx) |
| 3.4b 左右移动：收尾与提交 | [PNG](flow_movement-commit-compact.png) | [SVG](flow_movement-commit-compact.svg) | [VSDX](flow_movement-commit-compact.vsdx) |
| 3.5a 跳跃：输入与资格检查 | [PNG](flow_jump-compact.png) | [SVG](flow_jump-compact.svg) | [VSDX](flow_jump-compact.vsdx) |
| 3.5b 跳跃：运动计算与提交 | [PNG](flow_jump-motion-compact.png) | [SVG](flow_jump-motion-compact.svg) | [VSDX](flow_jump-motion-compact.vsdx) |
| 3.6a 连续攻击：输入与资格检查 | [PNG](flow_moving_attack-compact.png) | [SVG](flow_moving_attack-compact.svg) | [VSDX](flow_moving_attack-compact.vsdx) |
| 3.6b 连续攻击：修正站位与命中 | [PNG](flow_moving_attack-motion-compact.png) | [SVG](flow_moving_attack-motion-compact.svg) | [VSDX](flow_moving_attack-motion-compact.vsdx) |
| 3.6c 连续攻击：伤害与统一提交 | [PNG](flow_moving_attack-commit-compact.png) | [SVG](flow_moving_attack-commit-compact.svg) | [VSDX](flow_moving_attack-commit-compact.vsdx) |
| 3.7a 敌人：决策与运动提案 | [PNG](flow_enemy_ai-compact.png) | [SVG](flow_enemy_ai-compact.svg) | [VSDX](flow_enemy_ai-compact.vsdx) |
| 3.7b 敌人：执行与统一提交 | [PNG](flow_enemy_ai-action-compact.png) | [SVG](flow_enemy_ai-action-compact.svg) | [VSDX](flow_enemy_ai-action-compact.vsdx) |
| 3.8a 陷阱：触发与伤害候选 | [PNG](flow_trap_damage-compact.png) | [SVG](flow_trap_damage-compact.svg) | [VSDX](flow_trap_damage-compact.vsdx) |
| 3.8b 陷阱：死亡优先与提交 | [PNG](flow_trap_damage-commit-compact.png) | [SVG](flow_trap_damage-commit-compact.svg) | [VSDX](flow_trap_damage-commit-compact.vsdx) |
| 3.9a 交互：拾取与检查点提交 | [PNG](flow_pickup_checkpoint-compact.png) | [SVG](flow_pickup_checkpoint-compact.svg) | [VSDX](flow_pickup_checkpoint-compact.vsdx) |
| 3.9b 交互：保存已提交进度 | [PNG](flow_pickup_checkpoint-save-compact.png) | [SVG](flow_pickup_checkpoint-save-compact.svg) | [VSDX](flow_pickup_checkpoint-save-compact.vsdx) |
| 3.10a 完成本关：目标与存档 | [PNG](flow_next_level-completion-compact.png) | [SVG](flow_next_level-completion-compact.svg) | [VSDX](flow_next_level-completion-compact.vsdx) |
| 3.10b 下一关：准备候选资源 | [PNG](flow_next_level-compact.png) | [SVG](flow_next_level-compact.svg) | [VSDX](flow_next_level-compact.vsdx) |
| 3.10c 下一关：切换与旧资源释放 | [PNG](flow_next_level-activate-compact.png) | [SVG](flow_next_level-activate-compact.svg) | [VSDX](flow_next_level-activate-compact.vsdx) |
| 3.11 暂停、继续与失焦 | [PNG](flow_pause_focus-compact.png) | [SVG](flow_pause_focus-compact.svg) | [VSDX](flow_pause_focus-compact.vsdx) |
| 3.12 复活或重试 | [PNG](flow_respawn-compact.png) | [SVG](flow_respawn-compact.svg) | [VSDX](flow_respawn-compact.vsdx) |
| 3.13a 保存并返回菜单 | [PNG](flow_save_exit-compact.png) | [SVG](flow_save_exit-compact.svg) | [VSDX](flow_save_exit-compact.vsdx) |
| 3.13b 程序退出：按所有权释放 | [PNG](flow_save_exit-cleanup-compact.png) | [SVG](flow_save_exit-cleanup-compact.svg) | [VSDX](flow_save_exit-cleanup-compact.vsdx) |

总览图由 `python docs/assets/build_paper_diagrams.py` 生成；业务调用图由 `python docs/assets/build_business_flow_diagrams.py` 生成，定义见 `business_flow_specs.py`。后者会逐项校验节点中的函数确实声明在对应头文件。

运行环境需要 PySide6、pywin32 和 Microsoft Visio；总览图生成器还需要 Graphviz。

[下载当前全部 29 张可编辑 Visio 图](QtKnight-paper-diagrams-Visio.zip)。
