/**
 * @file ContentPrivate.h
 * @brief 内容模块解析文件期间使用的未验证草稿。
 *
 * 仅供 KnightContent.dll 内部使用；验证并解析资源引用后才能生成公开关卡结果。
 */

#pragma once

#include "KnightContracts.h"

namespace knight {

/// @brief 解析期间的未验证数据；不得作为公开关卡结果返回。
struct LevelDraft {
    LevelDefinition partial; ///< 尚未完成完整验证的关卡数据。
    std::vector<ResourceId> unresolvedRefs; ///< 等待解析的资源编号。
    std::string sourcePathUtf8; ///< 原始关卡文件的 UTF-8 路径，用于诊断。
    int errorLine = 0; ///< 解析器记录的出错行位置。
    int errorColumn = 0; ///< 解析器记录的出错列位置。
};

} // namespace knight
