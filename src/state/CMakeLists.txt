# SHARED 表示动态库；KnightState 是构建目标名，后续 target_* 命令必须使用同一名称。
# .cpp 会被编译，.h 列在这里是为了让工程视图显示这个公开头文件。
add_library(KnightState SHARED
    KnightState.h
    KnightState.cpp)

# 仅在编译本 DLL 时定义 KNIGHTSTATE_BUILD，使头文件选择 dllexport。
# 其他使用这个 DLL 的目标没有该定义，头文件会选择 dllimport。
target_compile_definitions(KnightState PRIVATE KNIGHTSTATE_BUILD)

# 本 DLL 使用共享头文件目录和 Qt 的基础模块；PRIVATE 表示不向使用者传播这两项链接依赖。
target_link_libraries(KnightState PRIVATE
    KnightContracts
    Qt6::Core)

# 把当前 state 目录加入头文件搜索路径；PUBLIC 表示本 DLL 和使用者都能使用该目录。
target_include_directories(KnightState PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}"
)
