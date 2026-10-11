"""Render business calls with module, header and function labels inside each figure."""

from __future__ import annotations

import argparse
import os
import re
from pathlib import Path

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtGui import QFontDatabase, QGuiApplication
import win32com.client

import build_paper_diagrams as p
from business_flow_specs import FIGURES, MODULES, SCENARIOS

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent


def wrap(value: str, size: int, width: float) -> list[str]:
    lines, current = [], ""
    for char in value:
        if char == "\n":
            lines.append(current)
            current = ""
        elif current and p.approx_text_width(current + char, size) > width:
            lines.append(current)
            current = char
        else:
            current += char
    if current:
        lines.append(current)
    return lines


def validate_specs() -> None:
    for name, figure in FIGURES.items():
        if not 2 <= len(figure.steps) <= 4:
            raise ValueError(f"{name}: each figure needs 2–4 call nodes")
        for step in figure.steps:
            _, _, header_path = MODULES[step.module]
            header = (ROOT / header_path).read_text(encoding="utf-8")
            if len(step.functions) > 2:
                raise ValueError(f"{name}: too many functions in one node")
            if step.module == "internal":
                if step.functions:
                    raise ValueError("Internal logic must not pretend to be a declared function")
            else:
                if not step.functions:
                    raise ValueError(f"{name}: missing function")
                for function in step.functions:
                    if not re.search(r"\b" + re.escape(function) + r"\s*\(", header):
                        raise ValueError(f"{name}: {function} is absent from {header_path}")
        if any(index >= len(figure.steps) for index in figure.failure_at):
            raise ValueError(f"{name}: invalid branch source")


def draw_step(root, index, step, x, y, width):
    owner, header, _ = MODULES[step.module]
    if step.module == "game":
        owner = "UI → " + owner
    fill = p.PALE if step.module == "internal" else "#ffffff"
    p.rect(root, x, y, width, 174, fill, p.BLUE if step.module == "internal" else "#8ca2af", 5, 1.6)
    p.text(root, x + 18, y + 31, f"{index:02d}", 15, p.BLUE, True)
    p.text(root, x + width / 2 + 9, y + 32, step.title, 21, p.INK, True, "middle")
    p.line(root, x + 18, y + 48, x + width - 18, y + 48, p.RULE, 1)
    p.text(root, x + width / 2, y + 75, owner, 17, p.BLUE, True, "middle")
    p.text(root, x + width / 2, y + 101, header, 17, p.MUTED, anchor="middle")
    if step.functions:
        for row, function in enumerate(step.functions):
            p.text(root, x + width / 2, y + 133 + row * 25,
                   function + "()", 18, p.INK, anchor="middle")
    else:
        p.text(root, x + width / 2, y + 133, step.state, 16, p.INK, anchor="middle")
        p.text(root, x + width / 2, y + 157, "内部逻辑 · 非导出函数", 15, p.MUTED, anchor="middle")


def build(figure):
    count = len(figure.steps)
    width = 1200 if count == 4 else 1100
    card_w = 250 if count == 4 else 280 if count == 3 else 330
    gap = 35 if count == 4 else 70 if count == 3 else 120
    left = (width - count * card_w - (count - 1) * gap) / 2
    height = 650 if figure.failure_at else 560
    root = p.new_canvas(width, height, figure.title, "业务与接口调用")
    for row, value in enumerate(wrap(figure.context, 17, width - 164)):
        p.text(root, 82, 135 + row * 23, value, 17, p.INK)
    p.text(root, 82, 183, "底层调用统一由 KnightGame.dll 发起；箭头表示处理顺序，不表示底层模块互调。", 15, p.MUTED)
    y = 210
    xs = [left + i * (card_w + gap) for i in range(count)]
    for i in range(count - 1):
        p.line(root, xs[i] + card_w, y + 87, xs[i + 1] - 3, y + 87, p.BLUE, 1.8, True)
        if i in figure.conditions:
            p.text(root, xs[i] + card_w + gap / 2, y + 72,
                   figure.conditions[i], 15, p.MUTED, anchor="middle")
    if figure.failure_at:
        centers = [xs[i] + card_w / 2 for i in figure.failure_at]
        center = width / 2
        for x in centers:
            p.line(root, x, y + 174, x, 435, p.BLUE, 1.6)
        p.line(root, min([center, *centers]), 435, max([center, *centers]), 435, p.BLUE, 1.6)
        p.line(root, center, 435, center, 463, p.BLUE, 1.6, True)
        p.rect(root, 82, 466, width - 164, 56, p.PAPER, p.RULE, 5, 1.4)
        p.text(root, center, 501, figure.failure, 17, p.INK, anchor="middle")
        p.text(root, min(centers) + 24, 416, "失败／未通过", 14, p.MUTED)
    for i, (step, x) in enumerate(zip(figure.steps, xs), 1):
        draw_step(root, i, step, x, y, card_w)
    p.line(root, 82, height - 100, width - 82, height - 100, p.RULE, 1)
    for row, value in enumerate(wrap(figure.note, 16, width - 164)):
        p.text(root, 82, height - 74 + row * 23, value, 16, p.MUTED)
    p.text(root, 82, height - 24, "共享数据类型：KnightContracts.h；带“内部逻辑”的节点不代表新增函数接口。", 14, p.MUTED)
    return root


def main():
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--png-only", action="store_true")
    group.add_argument("--visio-only", action="store_true")
    args = parser.parse_args()
    validate_specs()
    paths = [HERE / f"{name}-compact.svg" for name in FIGURES]
    if not args.visio_only:
        app = QGuiApplication([])
        for font in (r"C:\Windows\Fonts\msyh.ttc", r"C:\Windows\Fonts\msyhbd.ttc"):
            if QFontDatabase.addApplicationFont(font) < 0:
                raise RuntimeError(f"Could not load font {font}")
        for path, figure in zip(paths, FIGURES.values()):
            p.save_svg(build(figure), path)
            p.render_png(path, app)
            print(f"PNG {path.with_suffix('.png').name}: {len(figure.steps)} call nodes", flush=True)
    if not args.png_only:
        visio = win32com.client.DispatchEx("Visio.Application")
        visio.Visible = False
        stencil = visio.Documents.Open(str(p.STENCIL))
        try:
            font_id = next(stencil.Fonts.Item(i).ID for i in range(1, stencil.Fonts.Count + 1)
                           if stencil.Fonts.Item(i).Name == "Microsoft YaHei UI")
            for path in paths:
                p.export_visio(path, visio, stencil, font_id)
        finally:
            stencil.Close()
            visio.Quit()


if __name__ == "__main__":
    main()
