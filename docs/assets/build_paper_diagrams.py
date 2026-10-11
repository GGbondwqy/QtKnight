"""Regenerate the paper-style diagrams and editable Visio documents.

The 14 flow diagrams use the checked-in DOT files for their topology and
Graphviz only for coordinates. SVG and VSDX are built from native primitives;
the VSDX files contain editable Visio shapes and lines, not embedded pictures.
"""

from __future__ import annotations

import math
import re
import shlex
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

from PySide6.QtGui import QFontDatabase, QGuiApplication, QImage, QPainter
from PySide6.QtSvg import QSvgRenderer
import win32com.client


ASSETS = Path(__file__).resolve().parent
DOT = Path(r"E:\Anaconda3\envs\deepsc_s\Scripts\dot.bat")
STENCIL = Path(r"C:\Program Files\Microsoft Office\root\Office16\Visio Content\2052\BASIC_M.VSSX")
SVG_NS = "http://www.w3.org/2000/svg"
ET.register_namespace("", SVG_NS)

INK = "#25323b"
BLUE = "#52728a"
MID = "#6f899a"
MUTED = "#566873"
PALE = "#edf3f6"
PAPER = "#f8fafb"
RULE = "#d7e0e5"

FLOW_NAMES = [
    "flow_overview", "flow_startup", "flow_new_game", "flow_continue",
    "flow_movement", "flow_jump", "flow_moving_attack", "flow_enemy_ai",
    "flow_trap_damage", "flow_pickup_checkpoint", "flow_next_level",
    "flow_pause_focus", "flow_respawn", "flow_save_exit",
]

ARCHITECTURE = ASSETS / "architecture-overview-paper.svg"
CLASS_STYLE = {
    "title": (32, INK, True), "sub": (17, "#667985", False),
    "layer": (20, "#345c73", True), "num": (14, "#67869a", True),
    "main": (24, INK, True), "detail": (17, "#52636e", False),
    "module": (18, INK, True), "dll": (15, "#4e6878", False),
    "note": (14, "#687782", False), "caption": (16, MUTED, False),
}


def tag(name: str) -> str:
    return f"{{{SVG_NS}}}{name}"


def el(parent: ET.Element, name: str, **attrs: object) -> ET.Element:
    return ET.SubElement(parent, tag(name), {k.replace("_", "-"): str(v) for k, v in attrs.items()})


def rect(parent: ET.Element, x: float, y: float, w: float, h: float,
         fill: str = "#ffffff", stroke: str = RULE, radius: int = 5,
         stroke_width: float = 1.5) -> None:
    el(parent, "rect", x=f"{x:.2f}", y=f"{y:.2f}", width=f"{w:.2f}",
       height=f"{h:.2f}", rx=radius, fill=fill, stroke=stroke,
       stroke_width=stroke_width)


def line(parent: ET.Element, x1: float, y1: float, x2: float, y2: float,
         color: str = MID, width: float = 1.7, arrow: bool = False) -> None:
    attrs = {"x1": f"{x1:.2f}", "y1": f"{y1:.2f}",
             "x2": f"{x2:.2f}", "y2": f"{y2:.2f}",
             "stroke": color, "stroke_width": width}
    if arrow:
        attrs["marker_end"] = "url(#arrow)"
    el(parent, "line", **attrs)


def text(parent: ET.Element, x: float, y: float, value: str, size: int = 18,
         color: str = INK, bold: bool = False, anchor: str = "start") -> None:
    node = el(parent, "text", x=f"{x:.2f}", y=f"{y:.2f}",
              fill=color, font_size=size, font_weight="700" if bold else "400",
              font_family="Microsoft YaHei", text_anchor=anchor)
    node.text = value


def new_canvas(width: float, height: float, title: str, subtitle: str = "") -> ET.Element:
    root = ET.Element(tag("svg"), {"width": str(int(math.ceil(width))),
                                   "height": str(int(math.ceil(height))),
                                   "viewBox": f"0 0 {int(math.ceil(width))} {int(math.ceil(height))}"})
    defs = el(root, "defs")
    marker = el(defs, "marker", id="arrow", markerWidth="8", markerHeight="8",
                refX="7", refY="4", orient="auto", markerUnits="strokeWidth")
    el(marker, "path", d="M0,0 L8,4 L0,8 Z", fill=BLUE)
    rect(root, 0, 0, width, height, fill="#ffffff", stroke="none", radius=0, stroke_width=0)
    text(root, 82, 63, title, 31, INK, True)
    if subtitle:
        text(root, width - 82, 61, subtitle, 16, MUTED, anchor="end")
    line(root, 82, 88, width - 82, 88, "#b7c6cf", 1.5)
    return root


def decode_label(value: str) -> str:
    return value.replace("\\n", "\n")


def dot_plain(path: Path) -> tuple[float, float, list[dict], list[dict]]:
    run = subprocess.run([str(DOT), "-Tplain", str(path)], check=True,
                         capture_output=True, text=True, encoding="utf-8")
    graph_width = graph_height = 0.0
    nodes: list[dict] = []
    edges: list[dict] = []
    for raw in run.stdout.splitlines():
        parts = shlex.split(raw)
        if not parts:
            continue
        if parts[0] == "graph":
            graph_width, graph_height = float(parts[2]), float(parts[3])
        elif parts[0] == "node":
            nodes.append({"id": parts[1], "x": float(parts[2]), "y": float(parts[3]),
                          "w": float(parts[4]), "h": float(parts[5]),
                          "label": decode_label(parts[6]), "shape": parts[8]})
        elif parts[0] == "edge":
            count = int(parts[3])
            at = 4
            points = [(float(parts[at + 2 * i]), float(parts[at + 2 * i + 1]))
                      for i in range(count)]
            at += 2 * count
            label = None
            if len(parts) - at > 2 and parts[at] not in {"solid", "dashed", "dotted"}:
                label = (decode_label(parts[at]), float(parts[at + 1]), float(parts[at + 2]))
            edges.append({"source": parts[1], "target": parts[2],
                          "points": points, "label": label})
    return graph_width, graph_height, nodes, edges


def flow_title(path: Path) -> str:
    source = path.read_text(encoding="utf-8")
    match = re.search(r'graph\s*\[label="([^"]+)"', source)
    if not match:
        raise ValueError(f"Missing title in {path.name}")
    return match.group(1)


def make_flow(path: Path) -> ET.Element:
    gw, gh, nodes, edges = dot_plain(path)
    scale = 130
    width = max(900, gw * scale + 170)
    height = 120 + gh * scale + 78
    offset = (width - gw * scale) / 2
    root = new_canvas(width, height, flow_title(path), "业务流程 · 目标接口")

    def xy(x: float, y: float) -> tuple[float, float]:
        return offset + x * scale, 120 + (gh - y) * scale

    # Connectors go behind nodes. The shape positions come from Graphviz;
    # individual lines and labels remain native, editable Visio objects.
    for edge in edges:
        pts = [xy(x, y) for x, y in edge["points"]]
        path_d = " ".join(("M" if i == 0 else "L") + f"{x:.2f},{y:.2f}"
                          for i, (x, y) in enumerate(pts))
        el(root, "path", d=path_d, fill="none", stroke=MID,
           stroke_width="1.7", marker_end="url(#arrow)")
    for edge in edges:
        if edge["label"]:
            label, x, y = edge["label"]
            px, py = xy(x, y)
            estimated = max(88, min(360, len(label) * 14))
            rect(root, px - estimated / 2 - 5, py - 18, estimated + 10, 26,
                 fill="#ffffff", stroke="none", radius=0, stroke_width=0)
            text(root, px, py, label, 14, MUTED, anchor="middle")

    for node in nodes:
        cx, cy = xy(node["x"], node["y"])
        w, h = node["w"] * scale, node["h"] * scale
        label = node["label"]
        if node["shape"] == "diamond":
            pts = [(cx, cy - h / 2), (cx + w / 2, cy),
                   (cx, cy + h / 2), (cx - w / 2, cy)]
            el(root, "polygon", points=" ".join(f"{x:.2f},{y:.2f}" for x, y in pts),
               fill=PALE, stroke=BLUE, stroke_width="1.7")
        else:
            fill = PAPER if label.startswith("界面") else PALE if label.startswith("业务层") else "#ffffff"
            border = BLUE if label.startswith("业务层") else "#8ca2af"
            rect(root, cx - w / 2, cy - h / 2, w, h, fill, border, 5, 1.5)
        lines = label.split("\n")
        font = 16 if max(map(len, lines)) < 33 else 15
        line_height = font * 1.34
        first = cy - (len(lines) - 1) * line_height / 2 + font * 0.34
        for idx, value in enumerate(lines):
            text(root, cx, first + idx * line_height, value, font,
                 INK if idx == 0 else MUTED, bold=idx == 0, anchor="middle")

    text(root, 82, height - 28, "箭头表示处理顺序；底层模块的结果先返回业务层，再由协调器传给下一步。",
         15, MUTED)
    return root


def make_business_flow() -> ET.Element:
    width, height = 1600, 818
    root = new_canvas(width, height, "单帧业务逻辑数据流", "固定逻辑帧 · 目标流程")
    rect(root, 82, 118, 1436, 52, PAPER, RULE)
    text(root, 800, 151, "所有底层调用由游戏协调器发起", 20, BLUE, True, "middle")
    stages = [
        ("01", "接收与校验", ["输入命令", "会话状态"], "业务层"),
        ("02", "生成行动意图", ["玩家操作", "敌人决策"], "KnightAI.dll"),
        ("03", "运动与碰撞", ["运动提案 / 地图裁决", "运动收尾"], "KnightMotion.dll / KnightMap.dll"),
        ("04", "伤害与交互", ["攻击 / 陷阱", "拾取 / 检查点"], "KnightCombat.dll"),
        ("05", "提交与呈现", ["世界状态 / 关卡存档", "画面快照"], "KnightState.dll / KnightContent.dll / KnightSave.dll"),
    ]
    card_x, card_y, card_w, card_h, gap = 83, 226, 274, 260, 18
    for i in range(4):
        x = card_x + i * (card_w + gap)
        line(root, x + card_w + 2, card_y + 130,
             x + card_w + gap - 4, card_y + 130, BLUE, 2, True)
    for i, (num, title, details, modules) in enumerate(stages):
        x = card_x + i * (card_w + gap)
        rect(root, x, card_y, card_w, card_h, PALE if i == 4 else "#ffffff",
             BLUE if i == 4 else "#8ca2af", 5, 1.7)
        text(root, x + 22, card_y + 39, num, 16, BLUE, True)
        text(root, x + card_w / 2, card_y + 98, title, 22, INK, True, "middle")
        line(root, x + 32, card_y + 120, x + card_w - 32, card_y + 120, RULE, 1)
        text(root, x + card_w / 2, card_y + 166, details[0], 17, MUTED, anchor="middle")
        text(root, x + card_w / 2, card_y + 202, details[1], 17, MUTED, anchor="middle")
        rect(root, x, 510, card_w, 92, PAPER, RULE)
        text(root, x + card_w / 2, 541, "调用模块", 15, BLUE, True, "middle")
        if i == 4:
            text(root, x + card_w / 2, 566, "KnightState.dll / KnightContent.dll", 13, MUTED, anchor="middle")
            text(root, x + card_w / 2, 587, "KnightSave.dll", 13, MUTED, anchor="middle")
        else:
            text(root, x + card_w / 2, 575, modules, 14, MUTED, anchor="middle")
    rect(root, 82, 651, 1436, 98, PAPER, RULE)
    text(root, 111, 683, "结果回传", 16, BLUE, True)
    text(root, 800, 719, "模块结果  →  状态变更单  →  领域事件与只读快照  →  界面呈现",
         20, INK, anchor="middle")
    return root


def make_data_ownership() -> ET.Element:
    width, height = 1600, 866
    root = new_canvas(width, height, "关卡配置、运行状态与存档", "数据归属 · 非调用图")
    text(root, 800, 148, "游戏协调器负责读取、汇总与传递值对象", 20, MUTED, anchor="middle")
    cards = [
        ("静态关卡配置", "level_01.json", ["地图图块", "敌人与陷阱原型", "关卡目标"], "读取并校验：KnightContent.dll"),
        ("运行中的世界状态", "KnightState.dll", ["实体位置与生命", "攻击与陷阱相位", "当前检查点"], "唯一可写的权威世界"),
        ("持久化玩家进度", "KnightSave.dll", ["已解锁关卡", "关键物品与能力", "最近检查点"], "事务写入与读取"),
    ]
    for i, (title, filename, items, owner) in enumerate(cards):
        x = 82 + i * 486
        fill = PALE if i == 1 else "#ffffff"
        rect(root, x, 204, 464, 454, fill, BLUE if i == 1 else "#8ca2af", 5, 1.7)
        text(root, x + 30, 248, f"0{i+1}", 16, BLUE, True)
        text(root, x + 232, 304, title, 25, INK, True, "middle")
        text(root, x + 232, 345, filename, 18, BLUE, anchor="middle")
        line(root, x + 40, 371, x + 424, 371, RULE, 1)
        for j, item in enumerate(items):
            yy = 426 + j * 65
            rect(root, x + 44, yy - 12, 8, 8, BLUE, "none", 0, 0)
            text(root, x + 70, yy, item, 20, INK)
        text(root, x + 232, 623, owner, 16, MUTED, anchor="middle")
    rect(root, 82, 707, 1436, 92, PAPER, RULE)
    text(root, 800, 763, "实时坐标与速度不写入存档", 23, BLUE, True, "middle")
    return root


def save_svg(root: ET.Element, path: Path) -> None:
    ET.ElementTree(root).write(path, encoding="utf-8", xml_declaration=True)


def render_png(svg_path: Path, app: QGuiApplication) -> Path:
    renderer = QSvgRenderer(str(svg_path))
    if not renderer.isValid():
        raise RuntimeError(f"Invalid SVG: {svg_path}")
    width, height = renderer.defaultSize().width(), renderer.defaultSize().height()
    image = QImage(width, height, QImage.Format_ARGB32)
    image.fill(0xffffffff)
    painter = QPainter(image)
    renderer.render(painter)
    painter.end()
    result = svg_path.with_suffix(".png")
    if not image.save(str(result)):
        raise RuntimeError(f"Could not save {result}")
    return result


def rgb_formula(value: str) -> str:
    value = value.lstrip("#")
    return f"RGB({int(value[0:2],16)},{int(value[2:4],16)},{int(value[4:6],16)})"


def to_inches_x(value: float) -> float:
    return value / 100


def to_inches_y(value: float, height: float) -> float:
    return (height - value) / 100


def set_style(shape, fill: str | None, stroke: str | None, width: float = 1.5,
              dashed: bool = False) -> None:
    if fill and fill != "none":
        shape.CellsU("FillPattern").FormulaU = "1"
        shape.CellsU("FillForegnd").FormulaU = rgb_formula(fill)
    else:
        shape.CellsU("FillPattern").FormulaU = "0"
    if stroke and stroke != "none":
        shape.CellsU("LinePattern").FormulaU = "2" if dashed else "1"
        shape.CellsU("LineColor").FormulaU = rgb_formula(stroke)
        shape.CellsU("LineWeight").FormulaU = f"{max(0.7, width * 0.75):.2f} pt"
    else:
        shape.CellsU("LinePattern").FormulaU = "0"


def approx_text_width(value: str, font_size: float) -> float:
    units = sum(1.0 if ord(c) > 127 else 0.58 for c in value)
    return max(font_size * 1.8, units * font_size * 1.02 + 16)


def add_visio_text(page, elem: ET.Element, height: float, font_id: int) -> None:
    value = elem.text or ""
    if not value.strip():
        return
    cls = elem.get("class", "")
    default_size, default_color, default_bold = CLASS_STYLE.get(cls, (18, INK, False))
    size = float(elem.get("font-size", default_size))
    bold = elem.get("font-weight", "700" if default_bold else "400") in {"700", "bold"}
    color = elem.get("fill", default_color)
    anchor = elem.get("text-anchor", "start")
    x, y = float(elem.get("x", "0")), float(elem.get("y", "0"))
    box_w = approx_text_width(value, size)
    if anchor == "middle":
        left = x - box_w / 2
    elif anchor == "end":
        left = x - box_w
    else:
        left = x
    top = y - size * 1.13
    bottom = y + size * 0.29
    shape = page.DrawRectangle(to_inches_x(left), to_inches_y(bottom, height),
                               to_inches_x(left + box_w), to_inches_y(top, height))
    set_style(shape, None, None)
    shape.Text = value
    shape.CellsU("Char.Font").FormulaU = str(font_id)
    shape.CellsU("Char.AsianFont").FormulaU = str(font_id)
    shape.CellsU("Char.Size").FormulaU = f"{size * 0.72:.2f} pt"
    shape.CellsU("Char.Color").FormulaU = rgb_formula(color)
    shape.CellsU("Char.Style").FormulaU = "1" if bold else "0"
    shape.CellsU("Para.HorzAlign").FormulaU = "1" if anchor == "middle" else "2" if anchor == "end" else "0"
    shape.CellsU("VerticalAlign").FormulaU = "1"
    for margin in ("LeftMargin", "RightMargin", "TopMargin", "BottomMargin"):
        shape.CellsU(margin).FormulaU = "0 in"


def draw_visio_segment(page, x1: float, y1: float, x2: float, y2: float,
                       height: float, color: str, width: float, arrow: bool) -> None:
    shape = page.DrawLine(to_inches_x(x1), to_inches_y(y1, height),
                          to_inches_x(x2), to_inches_y(y2, height))
    set_style(shape, None, color, width)
    if arrow:
        shape.CellsU("EndArrow").FormulaU = "4"


def export_visio(svg_path: Path, visio, stencil, font_id: int) -> Path:
    root = ET.parse(svg_path).getroot()
    width, height = float(root.get("width")), float(root.get("height"))
    doc = visio.Documents.Add("")
    page = doc.Pages.Item(1)
    page.PageSheet.CellsU("PageWidth").FormulaU = f"{width / 100:.4f} in"
    page.PageSheet.CellsU("PageHeight").FormulaU = f"{height / 100:.4f} in"
    page.Name = svg_path.stem[:31]

    def visit(parent: ET.Element, inherited: dict[str, str]) -> None:
        for elem in parent:
            kind = elem.tag.rsplit("}", 1)[-1]
            if kind in {"defs", "style", "title", "desc"}:
                continue
            style = inherited.copy()
            style.update(elem.attrib)
            if kind == "g":
                visit(elem, style)
                continue
            if kind == "rect":
                x, y = float(elem.get("x", "0")), float(elem.get("y", "0"))
                w, h = float(elem.get("width", "0")), float(elem.get("height", "0"))
                if x == 0 and y == 0 and w >= width and h >= height:
                    continue
                shape = page.DrawRectangle(to_inches_x(x), to_inches_y(y + h, height),
                                           to_inches_x(x + w), to_inches_y(y, height))
                set_style(shape, style.get("fill"), style.get("stroke"),
                          float(style.get("stroke-width", "1.5")), "stroke-dasharray" in style)
            elif kind == "polygon":
                points = [(float(x), float(y)) for x, y in re.findall(r"([\d.]+),([\d.]+)", elem.get("points", ""))]
                if len(points) != 4:
                    raise ValueError(f"Unsupported polygon in {svg_path.name}")
                xs, ys = [p[0] for p in points], [p[1] for p in points]
                cx, cy = (min(xs) + max(xs)) / 2, (min(ys) + max(ys)) / 2
                shape = page.Drop(stencil.Masters.ItemU("Diamond"),
                                  to_inches_x(cx), to_inches_y(cy, height))
                shape.CellsU("Width").FormulaU = f"{(max(xs) - min(xs)) / 100:.4f} in"
                shape.CellsU("Height").FormulaU = f"{(max(ys) - min(ys)) / 100:.4f} in"
                set_style(shape, style.get("fill"), style.get("stroke"),
                          float(style.get("stroke-width", "1.5")))
            elif kind == "line":
                draw_visio_segment(page, float(elem.get("x1")), float(elem.get("y1")),
                                   float(elem.get("x2")), float(elem.get("y2")),
                                   height, style.get("stroke", MID),
                                   float(style.get("stroke-width", "1.7")),
                                   "marker-end" in style)
            elif kind == "path":
                d = elem.get("d", "")
                points = [(float(x), float(y)) for x, y in re.findall(r"[ML]\s*([\d.]+)[ ,]+([\d.]+)", d)]
                for idx in range(len(points) - 1):
                    (x1, y1), (x2, y2) = points[idx:idx + 2]
                    draw_visio_segment(page, x1, y1, x2, y2, height,
                                       style.get("stroke", MID),
                                       float(style.get("stroke-width", "1.7")),
                                       idx == len(points) - 2 and "marker-end" in style)
            elif kind == "text":
                add_visio_text(page, elem, height, font_id)

    visit(root, {})
    result = svg_path.with_suffix(".vsdx")
    doc.SaveAs(str(result))
    count = page.Shapes.Count
    doc.Close()
    print(f"VSDX {result.name}: {count} editable shapes", flush=True)
    return result


def main() -> None:
    if not ARCHITECTURE.is_file():
        raise FileNotFoundError(ARCHITECTURE)
    graphics = [ARCHITECTURE]
    for name, create in (("business-data-flow-paper", make_business_flow),
                         ("data-ownership-paper", make_data_ownership)):
        path = ASSETS / f"{name}.svg"
        save_svg(create(), path)
        graphics.append(path)
    for name in FLOW_NAMES:
        path = ASSETS / f"{name}-paper.svg"
        save_svg(make_flow(ASSETS / f"{name}.dot"), path)
        graphics.append(path)

    sys.stdout.reconfigure(encoding="utf-8")
    gui = QGuiApplication([])
    for font in (r"C:\Windows\Fonts\msyh.ttc", r"C:\Windows\Fonts\msyhbd.ttc"):
        if QFontDatabase.addApplicationFont(font) < 0:
            raise RuntimeError(f"Could not load font {font}")
    for path in graphics:
        render_png(path, gui)
        print(f"PNG  {path.with_suffix('.png').name}", flush=True)

    visio = win32com.client.DispatchEx("Visio.Application")
    visio.Visible = False
    stencil = visio.Documents.Open(str(STENCIL))
    try:
        font_id = next(stencil.Fonts.Item(i).ID for i in range(1, stencil.Fonts.Count + 1)
                       if stencil.Fonts.Item(i).Name == "Microsoft YaHei UI")
        for path in graphics:
            export_visio(path, visio, stencil, font_id)
    finally:
        stencil.Close()
        visio.Quit()
    print(f"Completed {len(graphics)} diagrams", flush=True)


if __name__ == "__main__":
    main()
