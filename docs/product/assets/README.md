# Qt Knight 业务需求图件

业务需求文档中的图统一使用 Excalidraw 格式，全部位于 `excalidraw/`。这种格式保存节点、文字、颜色和连线等可编辑信息，适合作为产品评审与后续软件设计的图形源文件。

## 图件清单

| 图 | Excalidraw 文件 | 类型 | 元素数 |
|---|---|---|---:|
| 产品范围思维导图 | [打开](excalidraw/product-scope-mindmap.excalidraw) | 思维导图 | 18 |
| 产品业务结构 | [打开](excalidraw/business-structure-tree.excalidraw) | 树状图 | 15 |
| 首版玩家旅程 | [打开](excalidraw/player-journey.excalidraw) | 阶段流程图 | 12 |
| 核心玩法循环 | [打开](excalidraw/core-gameplay-loop.excalidraw) | 循环流程图 | 16 |
| 页面导航关系 | [打开](excalidraw/page-navigation.excalidraw) | 页面流程图 | 18 |
| 玩家攻击流程 | [打开](excalidraw/attack-flow.excalidraw) | 业务流程图 | 18 |
| 敌人行为状态 | [打开](excalidraw/enemy-state.excalidraw) | 状态图 | 19 |
| 死亡、复活与重试 | [打开](excalidraw/respawn-flow.excalidraw) | 异常流程图 | 18 |
| 进度保存流程 | [打开](excalidraw/save-flow.excalidraw) | 业务流程图 | 18 |
| 游戏会话状态 | [打开](excalidraw/session-state.excalidraw) | 状态图 | 19 |
| 异常与安全边界 | [打开](excalidraw/safety-boundaries.excalidraw) | 关系图 | 18 |

## 如何查看和编辑

1. 打开 [Excalidraw](https://excalidraw.com/)。
2. 选择“打开”，或把 `.excalidraw` 文件拖入页面。
3. 也可以安装 VS Code Excalidraw 扩展后直接在项目中编辑。

普通 Markdown 阅读器不能直接把 `.excalidraw` 渲染为内嵌图片，因此业务需求文档提供的是可点击图源链接。评审或发布时如需 PNG、SVG、PDF，可在 Excalidraw 中通过“导出图片”临时生成，不作为仓库内的主图源格式。

## 重新生成全套图件

修改生成器后，在仓库根目录运行：

```powershell
python docs/product/assets/generate_business_excalidraw.py
```

生成器会覆盖 `excalidraw/` 下的 11 份图件。所有文字元素统一使用 Excalifont（`fontFamily: 5`），单图元素数量控制在 20 个以内。
