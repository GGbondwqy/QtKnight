#pragma once

#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>

namespace QtKnight
{

/// 场景内实体的共同属性，角色和地图对象共用同一套空间数据。
class EntityObject
{
public:
    virtual ~EntityObject();

    int id = 0;
    QPointF position; ///< 碰撞区域左上角的世界坐标，单位像素，右、下为正。
    QSizeF size{0, 0}; ///< 碰撞区域宽高，单位像素。

    QRectF bounds() const; ///< 根据 position 和 size 取得世界坐标碰撞矩形。

protected:
    EntityObject(const QSizeF& size = QSizeF{0, 0});
};

} // namespace QtKnight
