#pragma once

#include "MapObject.h"

#include <QtCore/QPointF>
#include <QtCore/QtGlobal>

namespace QtKnight
{

/// 接触或交互后激活的节点：拾取物发放奖励，检查点更新复活位置并请求保存。
class InteractionObject : public MapObject
{
public:
    enum class Kind
    {
        Pickup,
        Checkpoint
    };

    enum class ItemType
    {
        Health,
        Energy,
        Key,
        Ability
    };

    Kind kind = Kind::Pickup;
    bool requiresInteraction = false; ///< 拾取物可为接触触发，检查点应配置为主动交互。
    bool activated = false; ///< 拾取物表示已收集；检查点表示已激活，不表示保存成功。

    // 仅拾取物使用：按奖励类别读取数量、物品编号或能力位。
    ItemType itemType = ItemType::Health;
    int amount = 0;
    int keyItemId = 0;
    quint32 abilityMask = 0;

    QPointF respawnPosition; ///< 仅检查点使用：玩家复活左上角坐标，单位像素。

};

} // namespace QtKnight
