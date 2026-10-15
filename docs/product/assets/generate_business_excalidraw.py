"""Generate the complete Qt Knight BRD diagram set in Excalidraw format."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path


OUTPUT = Path(__file__).resolve().parent / "excalidraw"

COLORS = {
    "blue": ("#a5d8ff", "#1971c2"),
    "green": ("#b2f2bb", "#2b8a3e"),
    "yellow": ("#ffec99", "#e67700"),
    "red": ("#ffc9c9", "#c92a2a"),
    "purple": ("#eebefa", "#862e9c"),
    "gray": ("#e9ecef", "#495057"),
}


@dataclass
class Node:
    key: str
    x: float
    y: float
    w: float
    h: float


class ExcalidrawDiagram:
    def __init__(self, slug: str, title: str):
        self.slug = slug
        self.elements: list[dict] = []
        self.nodes: dict[str, Node] = {}
        self.serial = 1
        self.text("title", 380, 25, 440, 45, title, 28, bold=False)

    def _base(self, element_type: str, key: str, x: float, y: float, w: float, h: float) -> dict:
        serial = self.serial
        self.serial += 1
        return {
            "id": f"{self.slug}-{key}-{serial}",
            "type": element_type,
            "x": x,
            "y": y,
            "width": w,
            "height": h,
            "angle": 0,
            "strokeColor": "#1e1e1e",
            "backgroundColor": "transparent",
            "fillStyle": "solid",
            "strokeWidth": 2,
            "strokeStyle": "solid",
            "roughness": 1,
            "opacity": 100,
            "groupIds": [],
            "frameId": None,
            "roundness": {"type": 3} if element_type == "rectangle" else None,
            "seed": 1000 + serial,
            "version": 1,
            "versionNonce": 5000 + serial,
            "isDeleted": False,
            "boundElements": [],
            "updated": 1,
            "link": None,
            "locked": False,
        }

    def node(
        self,
        key: str,
        x: float,
        y: float,
        w: float,
        h: float,
        label: str,
        color: str = "blue",
        shape: str = "rectangle",
        font_size: int = 20,
    ) -> None:
        fill, stroke = COLORS[color]
        element = self._base(shape, key, x, y, w, h)
        element["backgroundColor"] = fill
        element["strokeColor"] = stroke
        if shape == "diamond":
            element["roundness"] = {"type": 2}
        self.elements.append(element)
        self.nodes[key] = Node(key, x, y, w, h)
        line_count = label.count("\n") + 1
        text_h = line_count * font_size * 1.25
        self.text(
            f"{key}-label",
            x + 12,
            y + (h - text_h) / 2,
            w - 24,
            text_h,
            label,
            font_size,
        )

    def text(
        self,
        key: str,
        x: float,
        y: float,
        w: float,
        h: float,
        label: str,
        font_size: int,
        bold: bool = False,
    ) -> None:
        element = self._base("text", key, x, y, w, h)
        element.update(
            {
                "strokeWidth": 1,
                "roundness": None,
                "boundElements": None,
                "fontSize": font_size,
                "fontFamily": 5,
                "text": label,
                "textAlign": "center",
                "verticalAlign": "middle",
                "containerId": None,
                "originalText": label,
                "lineHeight": 1.25,
                "autoResize": False,
            }
        )
        self.elements.append(element)

    def edge(self, key: str, source: str, target: str, dashed: bool = False) -> None:
        a = self.nodes[source]
        b = self.nodes[target]
        acx, acy = a.x + a.w / 2, a.y + a.h / 2
        bcx, bcy = b.x + b.w / 2, b.y + b.h / 2
        dx, dy = bcx - acx, bcy - acy
        if abs(dx) >= abs(dy):
            sx = a.x + a.w if dx >= 0 else a.x
            sy = acy
            ex = b.x if dx >= 0 else b.x + b.w
            ey = bcy
        else:
            sx = acx
            sy = a.y + a.h if dy >= 0 else a.y
            ex = bcx
            ey = b.y if dy >= 0 else b.y + b.h
        element = self._base("arrow", key, sx, sy, ex - sx, ey - sy)
        element.update(
            {
                "strokeColor": "#495057",
                "backgroundColor": "transparent",
                "strokeStyle": "dashed" if dashed else "solid",
                "roundness": {"type": 2},
                "boundElements": None,
                "points": [[0, 0], [ex - sx, ey - sy]],
                "lastCommittedPoint": None,
                "startBinding": None,
                "endBinding": None,
                "startArrowhead": None,
                "endArrowhead": "arrow",
                "elbowed": False,
            }
        )
        self.elements.append(element)

    def save(self) -> None:
        data = {
            "type": "excalidraw",
            "version": 2,
            "source": "https://excalidraw.com",
            "elements": self.elements,
            "appState": {"viewBackgroundColor": "#ffffff", "gridSize": 20},
            "files": {},
        }
        (OUTPUT / f"{self.slug}.excalidraw").write_text(
            json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
        )


def product_scope() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("product-scope-mindmap", "Qt Knight 首版产品范围")
    d.node("root", 480, 290, 240, 100, "Qt Knight\n首个可玩版本", "yellow", font_size=24)
    d.node("entry", 60, 90, 290, 100, "进入游戏\n新游戏 · 继续 · 设置 · 退出", "blue", font_size=18)
    d.node("control", 850, 90, 290, 100, "核心操作\n移动 · 跳跃 · 攻击 · 交互", "green", font_size=18)
    d.node("content", 40, 500, 320, 100, "关卡内容\n平台 · 敌人 · 陷阱 · 拾取物", "purple", font_size=18)
    d.node("progress", 840, 500, 320, 100, "进度循环\n检查点 · 复活 · 过关 · 存档", "yellow", font_size=18)
    d.node("feedback", 450, 650, 300, 90, "玩家反馈\n画面 · 状态栏 · 音效 · 提示", "red", font_size=18)
    for key in ("entry", "control", "content", "progress", "feedback"):
        d.edge(f"root-{key}", "root", key)
    return d


def business_structure() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("business-structure-tree", "Qt Knight 产品业务结构")
    d.node("root", 480, 90, 240, 80, "Qt Knight", "yellow", font_size=25)
    items = [
        (40, "session", "会话与菜单\n主菜单 · 加载 · 暂停", "blue"),
        (330, "gameplay", "关卡游玩\n操作 · 敌人 · 陷阱", "green"),
        (620, "progress", "进度与存档\n槽位 · 检查点 · 解锁", "purple"),
        (910, "feedback", "显示与反馈\n场景 · 音效 · 提示", "yellow"),
    ]
    for x, key, text, color in items:
        d.node(key, x, 360, 250, 120, text, color, font_size=18)
        d.edge(f"root-{key}", "root", key)
    return d


def player_journey() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("player-journey", "Qt Knight 首版玩家旅程")
    stages = [
        (40, "entry", "1 进入游戏\n启动主菜单\n新建或选择存档\n等待关卡载入", "blue"),
        (330, "explore", "2 学习与探索\n移动和跳跃\n躲避陷阱\n攻击敌人", "green"),
        (620, "risk", "3 风险与恢复\n受到伤害\n激活检查点\n死亡后复活", "red"),
        (910, "goal", "4 达成目标\n取得关键物品\n进入出口\n保存并进入下一关", "yellow"),
    ]
    for x, key, text, color in stages:
        d.node(key, x, 240, 250, 230, text, color, font_size=18)
    for a, b in (("entry", "explore"), ("explore", "risk"), ("risk", "goal")):
        d.edge(f"{a}-{b}", a, b)
    return d


def core_loop() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("core-gameplay-loop", "核心玩法循环")
    d.node("observe", 60, 280, 220, 90, "观察环境", "blue", font_size=22)
    d.node("act", 360, 90, 240, 100, "移动 · 跳跃\n攻击 · 交互", "green", font_size=20)
    d.node("result", 760, 90, 220, 100, "获得操作结果", "yellow", font_size=20)
    d.node("goal", 900, 390, 240, 100, "完成目标并过关", "purple", font_size=20)
    d.node("recover", 360, 500, 270, 100, "受伤调整 / 死亡复活", "red", font_size=19)
    for a, b in (("observe", "act"), ("act", "result"), ("result", "goal"), ("goal", "recover"), ("recover", "observe")):
        d.edge(f"{a}-{b}", a, b)
    return d


def page_navigation() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("page-navigation", "页面导航关系")
    d.node("start", 30, 300, 170, 80, "启动页", "blue", font_size=22)
    d.node("menu", 250, 300, 180, 80, "主菜单", "blue", font_size=22)
    d.node("loading", 490, 300, 180, 80, "加载页", "yellow", font_size=22)
    d.node("game", 730, 300, 200, 80, "游戏主页面", "green", font_size=22)
    d.node("pause", 990, 90, 190, 80, "暂停菜单", "purple", font_size=21)
    d.node("outcome", 990, 420, 210, 120, "死亡页面\n或\n关卡完成页面", "red", font_size=19)
    for a, b in (("start", "menu"), ("menu", "loading"), ("loading", "game"), ("game", "pause"), ("game", "outcome")):
        d.edge(f"{a}-{b}", a, b)
    return d


def attack_flow() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("attack-flow", "玩家攻击业务流程")
    d.node("press", 30, 260, 170, 80, "按下攻击键", "blue", font_size=19)
    d.node("check", 260, 230, 210, 140, "状态、冷却、资源\n是否允许？", "yellow", "diamond", 17)
    d.node("reject", 275, 470, 180, 75, "拒绝本次攻击", "red", font_size=18)
    d.node("windup", 540, 260, 150, 80, "起手", "green", font_size=22)
    d.node("active", 750, 260, 180, 80, "有效攻击阶段", "red", font_size=19)
    d.node("recovery", 990, 260, 150, 80, "收招", "purple", font_size=22)
    for a, b in (("press", "check"), ("check", "reject"), ("check", "windup"), ("windup", "active"), ("active", "recovery")):
        d.edge(f"{a}-{b}", a, b)
    return d


def enemy_state() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("enemy-state", "敌人行为状态")
    d.node("patrol", 60, 280, 170, 80, "巡逻", "blue", font_size=23)
    d.node("alert", 320, 90, 170, 80, "警戒", "yellow", font_size=23)
    d.node("chase", 600, 90, 170, 80, "追击", "green", font_size=23)
    d.node("attack", 880, 280, 170, 80, "攻击", "red", font_size=23)
    d.node("return", 600, 500, 170, 80, "返回", "purple", font_size=23)
    d.node("dead", 320, 500, 170, 80, "死亡", "gray", font_size=23)
    for a, b in (("patrol", "alert"), ("alert", "chase"), ("chase", "attack"), ("attack", "return"), ("return", "patrol"), ("chase", "dead")):
        d.edge(f"{a}-{b}", a, b, dashed=b == "dead")
    return d


def respawn_flow() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("respawn-flow", "死亡、复活与重试流程")
    d.node("damage", 30, 250, 180, 80, "受到有效伤害", "red", font_size=19)
    d.node("health", 270, 220, 200, 140, "生命是否归零？\n否：继续游玩", "yellow", "diamond", 17)
    d.node("dead", 540, 250, 170, 80, "进入死亡状态", "red", font_size=19)
    d.node("choice", 770, 220, 200, 140, "复活、重试\n或返回菜单", "purple", "diamond", 17)
    d.node("rebuild", 1030, 110, 180, 90, "验证并重建\n本关状态", "blue", font_size=18)
    d.node("menu", 1030, 420, 180, 90, "返回主菜单", "gray", font_size=19)
    for a, b in (("damage", "health"), ("health", "dead"), ("dead", "choice"), ("choice", "rebuild"), ("choice", "menu")):
        d.edge(f"{a}-{b}", a, b)
    return d


def save_flow() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("save-flow", "进度保存流程")
    d.node("trigger", 30, 250, 190, 90, "检查点 / 通关\n主动保存", "blue", font_size=18)
    d.node("confirmed", 280, 220, 210, 140, "玩法结果确认？\n否：等待确认", "yellow", "diamond", 17)
    d.node("snapshot", 550, 250, 190, 90, "制作进度副本", "green", font_size=19)
    d.node("write", 800, 250, 170, 90, "写入存档", "purple", font_size=20)
    d.node("success", 1030, 110, 180, 80, "保存成功", "green", font_size=20)
    d.node("failure", 1030, 410, 190, 100, "保留旧存档\n提示并允许重试", "red", font_size=18)
    for a, b in (("trigger", "confirmed"), ("confirmed", "snapshot"), ("snapshot", "write"), ("write", "success"), ("write", "failure")):
        d.edge(f"{a}-{b}", a, b)
    return d


def session_state() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("session-state", "游戏会话状态")
    d.node("init", 30, 300, 160, 80, "初始化", "blue", font_size=21)
    d.node("menu", 250, 300, 170, 80, "主菜单", "blue", font_size=21)
    d.node("loading", 490, 300, 170, 80, "加载中", "yellow", font_size=21)
    d.node("playing", 730, 300, 170, 80, "游玩中", "green", font_size=21)
    d.node("paused", 980, 90, 180, 80, "已暂停", "purple", font_size=21)
    d.node("result", 980, 430, 200, 110, "死亡\n或关卡完成", "red", font_size=20)
    for a, b in (("init", "menu"), ("menu", "loading"), ("loading", "playing"), ("playing", "paused"), ("playing", "result"), ("result", "loading")):
        d.edge(f"{a}-{b}", a, b)
    return d


def safety_boundaries() -> ExcalidrawDiagram:
    d = ExcalidrawDiagram("safety-boundaries", "异常与安全边界")
    d.node("root", 470, 290, 260, 100, "保护玩家安全状态", "red", font_size=22)
    d.node("load", 50, 90, 300, 100, "加载失败\n不替换当前状态", "blue", font_size=19)
    d.node("save", 850, 90, 300, 100, "保存失败\n保留旧存档", "green", font_size=19)
    d.node("focus", 40, 500, 320, 100, "失焦释放输入\n暂停停止世界时间", "purple", font_size=18)
    d.node("death", 840, 500, 320, 100, "致命伤害优先\n取消同帧交互", "yellow", font_size=18)
    d.node("error", 450, 650, 300, 90, "错误可见、可理解、可重试", "gray", font_size=18)
    for key in ("load", "save", "focus", "death", "error"):
        d.edge(f"root-{key}", "root", key)
    return d


FACTORIES = [
    product_scope,
    business_structure,
    player_journey,
    core_loop,
    page_navigation,
    attack_flow,
    enemy_state,
    respawn_flow,
    save_flow,
    session_state,
    safety_boundaries,
]


def main() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for factory in FACTORIES:
        factory().save()
    print(f"Generated {len(FACTORIES)} Excalidraw diagrams in {OUTPUT}")


if __name__ == "__main__":
    main()
