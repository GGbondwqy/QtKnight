#pragma once

#include "servicelib/GameSession.h"

#include <QtCore/QList>
#include <QtCore/QRectF>

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

namespace QtKnight
{

/// 组织图层、实体排序、相机与 HUD 的只读绘制，不推进会话。
class SceneRenderer
{
public:
    /// 按固定图层绘制，恢复相机变换后再用屏幕坐标绘制 HUD。
    void paint(QPainter& painter, const GameSession::World& world,
               const QRectF& viewport) const;

private:
    /// 世界图层由后向前排列；HUD 独立使用屏幕坐标，不属于实体图层。
    enum class Layer
    {
        Background,
        Terrain,
        BackObjects,
        Characters,
        FrontObjects,
        Effects,
        Foreground
    };

    enum class EntityKind
    {
        Player,
        Enemy,
        Trap,
        Pickup,
        Checkpoint,
        Door,
        Exit
    };

    struct DrawItem
    {
        const EntityObject* entity = nullptr;
        EntityKind kind = EntityKind::Player;
        Layer layer = Layer::Characters;
    };

    /// 按 layer、kind、id 排序。陷阱和交互物在角色后面，门和出口在前面。
    QList<DrawItem> buildDrawQueue(const GameSession::World& world) const;
    void drawEntity(QPainter& painter, const DrawItem& item) const;

    void drawHud(QPainter& painter, const GameSession::World& world,
                 const QRectF& viewport) const;
};

} // namespace QtKnight
