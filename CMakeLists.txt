# 要求使用 3.25 或更新版本的 CMake；版本不足时停止配置。
cmake_minimum_required(VERSION 3.25)
# 声明工程名称为 QtKnight，并告诉 CMake 本工程使用 C++。
project(QtKnight LANGUAGES CXX)

# 以下两行要求所有目标按 C++17 编译，不允许自动退回更早的标准。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# CMAKE_BINARY_DIR 是本次构建目录，例如 build/Desktop_Qt_..._Debug。
# Windows 下的可执行程序和 DLL 放进该构建目录的 bin 文件夹。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# .lib 文件放进该构建目录的 lib 文件夹；当前生成的是 DLL 的导入库。
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
# 查找至少 6.12 版本的 Qt，并使 Qt6::Core 目标可供后续模块使用。
find_package(Qt6 6.12 REQUIRED COMPONENTS Core)
# 设置 Qt 工程常用的自动处理规则，例如自动处理需要 moc 的头文件。
qt_standard_project_setup()
# 声明只传递编译配置的接口目标；它本身不会生成 DLL 或 LIB。
add_library(KnightContracts INTERFACE)
# 以后链接 KnightContracts 的目标，可以直接包含 src/contracts 中的共享头文件。
# CMAKE_CURRENT_SOURCE_DIR 在此处指根目录，而不是 build 目录。
target_include_directories(KnightContracts INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/contracts"
)
# 继续读取状态模块目录内的 CMakeLists.txt，由它定义 KnightState 动态库。
add_subdirectory(src/state)
