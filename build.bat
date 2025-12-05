@echo off
chcp 65001 > nul 2>&1  :: 解决中文乱码问题

:: ==================== 第一步：配置项目根目录（需用户修改！）====================
:: 替换为你的项目根目录（例如：D:\projects\kmp_string_matcher）
set "PROJECT_ROOT=."

:: 检查项目根目录是否存在
if not exist "%PROJECT_ROOT%" (
    echo 错误：项目根目录不存在！请修改脚本中的 PROJECT_ROOT 变量。
    echo 当前配置的路径：%PROJECT_ROOT%
    pause
    exit /b 1
)

:: ==================== 第二步：创建并进入 build 目录 ====================
echo 1. 切换到项目根目录：%PROJECT_ROOT%
cd /d "%PROJECT_ROOT%" || (
    echo 错误：无法切换到项目根目录！
    pause
    exit /b 1
)

:: 检查 build 目录，不存在则创建
if not exist "build" (
    echo 2. 创建 build 目录...
    mkdir build || (
        echo 错误：创建 build 目录失败！
        pause
        exit /b 1
    )
) else (
    echo 2. build 目录已存在，跳过创建...
)

echo 3. 进入 build 目录...
cd build || (
    echo 错误：无法进入 build 目录！
    pause
    exit /b 1
)

:: ==================== 第三步：CMake 生成工程文件 ====================
echo 4. 执行 CMake 配置（生成工程文件）...
cmake .. || (
    echo 错误：CMake 配置失败！请检查：
    echo  1. CMake 是否已安装并添加到系统环境变量；
    echo  2. 系统是否安装了编译器（MSVC/MinGW/Clang）；
    echo  3. CMakeLists.txt 是否存在语法错误。
    pause
    exit /b 1
)

:: ==================== 第四步：编译项目 ====================
echo 5. 开始编译项目（并行编译，使用所有CPU核心）...
:: --config Release：编译 Release 版本（可选改为 Debug）
:: -j%NUMBER_OF_PROCESSORS%：使用系统所有CPU核心加速编译
cmake --build . --config Release -j%NUMBER_OF_PROCESSORS% || (
    echo 错误：项目编译失败！
    pause
    exit /b 1
)

:: ==================== 编译完成 ====================
echo.
echo ==============================================
echo          编译成功！可执行文件位于：
echo          %PROJECT_ROOT%\bin\Release\kmp_app.exe
echo ==============================================
pause