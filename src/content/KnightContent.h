/**
 * @file KnightContent.h
 * @brief 读取并验证完整关卡定义的公开接口。
 *
 * 当前函数仅有声明；公开结果不包含解析草稿、私有指针或运行世界。
 * 内容模块只解析静态定义，不创建运行实体或管理玩家进度。
 * Result 的 value 仅在状态为 Ok 时有效；异常须在本 DLL 内转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTCONTENT_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTCONTENT_BUILD)
#    define KNIGHTCONTENT_API __declspec(dllexport)
#  else
#    define KNIGHTCONTENT_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTCONTENT_API
#endif

namespace knight {

/**
 * @brief 解析关卡并完成原型与资源引用校验。
 * @details 不向调用方返回未验证草稿，不创建世界或地图句柄。
 * @param root 内容根目录的 UTF-8 路径。
 * @param id 稳定关卡编号，不是任意文件路径。
 * @return 成功返回完整关卡值对象；失败状态应指出文件、对象或字段问题。
 */
KNIGHTCONTENT_API Result<LevelDefinition> loadLevel(
    const ContentRoot& root, const LevelId& id) noexcept;

} // namespace knight
