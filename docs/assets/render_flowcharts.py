"""把业务文档中的 Mermaid 简图转换成可直接预览的 PNG 流程图。"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOCUMENT = ROOT / "业务逻辑与数据流.md"
ASSETS = ROOT / "assets"
DOT = shutil.which("dot") or r"E:\Anaconda3\envs\deepsc_s\Scripts\dot.bat"
NAMES = [
    "flow_overview",
    "flow_startup",
    "flow_new_game",
    "flow_continue",
    "flow_movement",
    "flow_jump",
    "flow_moving_attack",
    "flow_enemy_ai",
    "flow_trap_damage",
    "flow_pickup_checkpoint",
    "flow_next_level",
    "flow_pause_focus",
    "flow_respawn",
    "flow_save_exit",
]


def quoted(value: str) -> str:
    return json.dumps(value, ensure_ascii=False)


def wrap_label(label: str) -> str:
    label = label.replace("<br/>", "\n")
    if "：" in label:
        label = label.replace("：", "：\n", 1)
    parts = []
    for item in label.split("\n"):
        while len(item) > 27:
            cut = max(item.rfind("，", 0, 27), item.rfind("、", 0, 27))
            cut = cut + 1 if cut >= 12 else 27
            parts.append(item[:cut])
            item = item[cut:]
        parts.append(item)
    return "\n".join(parts)


def convert(source: str, title: str) -> str:
    lines = source.strip().splitlines()
    direction = "LR" if "flowchart LR" in lines[0] else "TB"
    nodes: dict[str, tuple[str, bool]] = {}
    edges: list[tuple[str, str, str]] = []
    for line in lines[1:]:
        for match in re.finditer(r'(\w+)\["([^"]+)"\]', line):
            nodes[match.group(1)] = (match.group(2), False)
        for match in re.finditer(r'(\w+)\{"([^"]+)"\}', line):
            nodes[match.group(1)] = (match.group(2), True)
        if "-->" not in line:
            continue
        left, right = line.split("-->", 1)
        source_id = re.match(r"\s*(\w+)", left)
        edge_label = ""
        right = right.strip()
        if right.startswith("|"):
            edge_label, right = right[1:].split("|", 1)
            right = right.strip()
        target_id = re.match(r"(\w+)", right)
        if not source_id or not target_id:
            raise ValueError(f"无法转换的连线：{line}")
        edges.append((source_id.group(1), target_id.group(1), edge_label))

    body = [
        "digraph flow {",
        f'  graph [label={quoted(title)}, labelloc=t, fontsize=18, fontname="Microsoft YaHei", '
        f'rankdir={direction}, bgcolor="#FFFFFF", pad=0.25, nodesep=0.35, ranksep=0.55, '
        'splines=polyline, dpi=140];',
        '  node [fontname="Microsoft YaHei", fontsize=11, margin="0.15,0.09", '
        'penwidth=1.2, color="#64748B"];',
        '  edge [fontname="Microsoft YaHei", fontsize=9, color="#64748B", '
        'fontcolor="#334155", arrowsize=0.7];',
    ]
    for node_id, (label, diamond) in nodes.items():
        if diamond:
            fill = "#FEF3C7"
            shape = "diamond"
            style = "filled"
        elif label.startswith("界面"):
            fill = "#DBEAFE"
            shape = "box"
            style = "rounded,filled"
        elif label.startswith("业务层"):
            fill = "#DCFCE7"
            shape = "box"
            style = "rounded,filled"
        else:
            fill = "#EDE9FE"
            shape = "box"
            style = "rounded,filled"
        body.append(
            f"  {quoted(node_id)} [label={quoted(wrap_label(label))}, shape={shape}, "
            f'style="{style}", fillcolor="{fill}"];'
        )
    for source_id, target_id, edge_label in edges:
        extra = f" [label={quoted(wrap_label(edge_label))}]" if edge_label else ""
        body.append(f"  {quoted(source_id)} -> {quoted(target_id)}{extra};")
    body.append("}")
    return "\n".join(body) + "\n"


def main() -> None:
    original = DOCUMENT.read_text(encoding="utf-8")
    pattern = re.compile(r"```mermaid\s*\n(.*?)```", re.S)
    matches = list(pattern.finditer(original))
    if not matches:
        for name in NAMES:
            dot_path = ASSETS / f"{name}.dot"
            png_path = ASSETS / f"{name}.png"
            if not dot_path.is_file():
                raise FileNotFoundError(dot_path)
            subprocess.run([str(DOT), "-Tpng", str(dot_path), "-o", str(png_path)], check=True)
        return
    if len(matches) != len(NAMES):
        raise RuntimeError(f"需要 {len(NAMES)} 个流程图，实际找到 {len(matches)} 个")
    replacement = original
    for match, name in reversed(list(zip(matches, NAMES))):
        prefix = original[: match.start()]
        headings = re.findall(r"^###?\s+(.+)$", prefix, re.M)
        title = headings[-1] if headings else name
        dot_path = ASSETS / f"{name}.dot"
        png_path = ASSETS / f"{name}.png"
        dot_path.write_text(convert(match.group(1), title), encoding="utf-8")
        subprocess.run([str(DOT), "-Tpng", str(dot_path), "-o", str(png_path)], check=True)
        replacement = (
            replacement[: match.start()]
            + f"![{title}](assets/{name}.png)"
            + replacement[match.end() :]
        )
    DOCUMENT.write_text(replacement, encoding="utf-8")


if __name__ == "__main__":
    main()
