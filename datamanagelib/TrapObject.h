#pragma once

#include "MapObject.h"

#include <QtCore/QtGlobal>

namespace QtKnight
{

/// 危险区域与伤害时机；玩家扣血、无敌和击退由会话结算。
class TrapObject : public MapObject
{
public:
    int damage = 0;
    int periodFrames = 0; ///< 0 为持续危险区，大于 0 为周期长度。
    int activeFrames = 0; ///< 每个周期的危险帧数。
    int offsetFrames = 0; ///< 周期相位偏移，单位逻辑帧。
    int damageIntervalFrames = 0; ///< 同一玩家再次受伤的最小逻辑帧间隔。
    int knockback = 0; ///< 配置的击退强度，方向与应用方式由移动流程确定。

    bool isActiveAt(quint64 frameNumber) const; ///< 按逻辑帧判断，不依赖动画。

};

} // namespace QtKnight
