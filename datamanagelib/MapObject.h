#pragma once

#include "EntityObject.h"

namespace QtKnight
{

/// 地图对象的共同基类，特殊伤害、交互及开启条件由具体类型负责。
class MapObject : public EntityObject
{
public:
    virtual bool isBlocking() const; ///< 默认不阻挡移动，需阻挡的类型重写此查询。

protected:
    MapObject();
};

} // namespace QtKnight
