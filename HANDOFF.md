# QtKnight 项目交接说明

**更新时间：** 2026-10-10（北京时间）
**用途：** 让接手本项目的开发者或下一次会话先了解真实开发状态。本文件记录现状；具体设计以 `docs` 中的三份正式文档为准。

## 1. 当前结论

QtKnight 是参考《Hollow Knight》玩法的 Qt 6 桌面横版闯关游戏，目标包含地图、移动、跳跃、攻击、陷阱、敌人、检查点、闯关和存档。三层架构和各模块接口已经设计，公开与私有头文件已经建立。**目前仍处在接口阶段，尚无可游玩的程序。**

仓库为 [GGbondwqy/QtKnight](https://github.com/GGbondwqy/QtKnight)，默认分支 `main`，截至本次整理前的代码基线为 `94eb983`。2026-10-10 核验时仓库为公开状态，`PerDuF-8142` 是具有写入权限的协作者。当前尚未约定两位开发者各自负责哪些模块。

## 2. 先读哪些文件

1. [游戏大体框架](docs/游戏大体框架.md)：项目目标、三层职责、七个底层模块和实施顺序。
2. [模块及接口设置](docs/模块及接口设置.md)：共享结构体、公开函数、私有对象、参数与返回值；**接口约定以此文件和对应 `.h` 为准**。
3. [业务逻辑与数据流](docs/业务逻辑与数据流.md)：菜单、输入、逐帧运算、战斗、陷阱、存档等调用顺序；配图位于 `docs/assets`。
4. [README](README.md) 和根目录 `CMakeLists.txt`：当前工程入口及构建范围。

`docs/qt_knight_architecture.md` 是较早的详细架构稿，`docs/figure_prompts.md` 是配图生成记录。它们保留作参考，出现差异时以上述三份正式文档和已确认的头文件为准。`docs` 内的 PDF 是便于阅读的导出版本，修改接口时应先改 Markdown 和代码，再重新导出 PDF。

## 3. 架构边界

目标调用方向是 `KnightUI.exe → KnightGame.dll → 七个底层 DLL`。界面只调用业务层；业务层整理输入、决定调用顺序、汇总候选结果；七个底层模块**彼此不包含对方的私有头文件、不直接链接或调用对方**。跨模块传输的值类型统一在 `src/contracts/KnightContracts.h`。

`KnightState` 拥有唯一可修改的运行世界。地图、运动、战斗和敌人决策读取 `WorldSnapshot` 等副本，返回候选结果；业务层汇总后调用 `applyChanges` 一次性提交。关卡内容由 `KnightContent` 解析，数据库连接只由 `KnightSave` 持有。

正常逻辑帧的目标顺序是：`readWorld → decideEnemies → proposeMotion → resolveMovement → queryTriggers → finishMotion → evaluateAttacks / evaluateTraps → resolveDamage → applyChanges`。每一步的结果都先回到业务层，再由业务层传给下一模块。输入中“持续按住”和“本帧新按下”分开记录，以避免移动时吞掉再次按下的攻击键。

## 4. 文件与实现状态

| 模块 | 已有文件 | 当前真实状态 |
|---|---|---|
| 共享契约 | `src/contracts/KnightContracts.h` | 已定义跨模块枚举、结构体、结果类型；本身不生成 DLL。 |
| 业务协调 | `src/game/KnightGame.h`、`GamePrivate.h` | 公开函数和会话数据已声明；没有 `.cpp` 和构建目标。 |
| 对象与状态 | `src/state/KnightState.h`、`WorldPrivate.h`、`KnightState.cpp`、`CMakeLists.txt` | 只有 `createWorld`、`destroyWorld`、`getPlayer` 三个旧演示函数有实现并可构建 `KnightState.dll`；其余公开函数只有声明。 |
| 地图与碰撞 | `src/map/KnightMap.h`、`MapPrivate.h` | 只有接口和私有地图结构，无实现或构建目标。 |
| 运动与物理 | `src/motion/KnightMotion.h` | 只有接口，无实现或构建目标。 |
| 战斗与陷阱 | `src/combat/KnightCombat.h` | 只有接口，无实现或构建目标。 |
| 敌人决策 | `src/ai/KnightAI.h` | 只有接口，无实现或构建目标。 |
| 关卡与配置 | `src/content/KnightContent.h`、`ContentPrivate.h` | 只有接口和解析草稿结构，无关卡文件或实现。 |
| 存档与数据库 | `src/save/KnightSave.h`、`SavePrivate.h` | 只有接口和私有数据库句柄，无表结构、实现或构建目标。 |
| 界面与绘制 | `src/ui/GameWindow.h`、`SceneRenderer.h` | 仅前向声明；没有界面实现或 `KnightUI.exe`。 |

不要把文档里的目标 DLL 清单误认为已经生成。当前根 `CMakeLists.txt` 只查找 Qt Core、声明共享头文件目标并进入 `src/state`；它还没有注册其余 DLL 和界面程序。头文件声明本身也不会生成可调用的 DLL 函数。

## 5. 开发环境和验证记录

- 目标平台：Windows 64 位、C++17、Qt 6.12、MSVC2022、CMake。当前本机 Qt 位于 `D:\QT`，Visual Studio 2022 位于 `D:\VS2022`；Qt 安装中有 `msvc2022_64` 套件。
- 两人可分别用 Qt Creator 或 Visual Studio 编辑同一套 CMake 源文件，但交付物必须使用相同的编译器、架构和构建配置。不要混用 MSVC 与 MinGW 生成的 `.lib`、`.dll`。
- 2026-10-07 曾用 Qt 6.12 MinGW 套件对核心头文件做语法检查，并构建现有 `KnightState` 演示 DLL。此前受限终端的 MSVC 配置检查被 Windows SDK 目录访问限制阻挡；这不等于已验证 MSVC 构建成功或代码有编译错误。
- 本次整理没有更改游戏函数实现，也没有重新构建。旧 `build` 目录已清理；下一次在 Qt Creator 中打开根 `CMakeLists.txt`，选择 `Desktop Qt 6.12.0 MSVC2022 64bit` 套件，重新配置并构建。

## 6. 本次清理

已删除 Git 忽略的 `tmp` 目录（临时脚本、语法检查文件和试验构建，约 51 MB）及 `build` 目录（旧 CMake 产物，约 5 MB）。它们都不含 Git 跟踪文件，可重新生成。

保留了 `output/pdf`：其中三个 PDF 与 `docs` 下的正式 PDF 不是完全相同的导出结果，暂不删除。也保留了 `.qtcreator` 中的本机套件设置，以及所有已跟踪源码、图、文档和 PDF。构建目录、输出目录和本机配置均已写入 `.gitignore`，不会随提交上传。

## 7. 建议的下一步

1. 两位开发者先确定模块分工，约定修改 `KnightContracts.h` 或公开接口前要同步讨论；各自从最新 `main` 建分支开发。
2. 先实现一个最小可玩闭环所需的关卡加载、世界初始化、地图碰撞、运动、业务协调与界面输入；再接入攻击、陷阱、存档。实现时逐项对照接口文档和业务流程文档。
3. 每增加一个 `.cpp` 模块，就补对应 `CMakeLists.txt`、导出宏和有意义的模块测试。存档模块单独链接 Qt Sql；界面模块再加入所需的 Qt 图形组件。
4. 做联调时重点验证“按住移动键后连续按 J 攻击”、触发区/伤害/死亡/复活，以及存档关闭后重新打开的结果。

接手会话应先运行 `git status` 和 `git fetch` 核对新提交，再确认本文件中的状态是否已经过时；随后从用户当前指定的模块继续开发。不要把尚未实现的接口当成已完成的功能。
