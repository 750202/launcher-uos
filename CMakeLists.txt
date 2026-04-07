cmake_minimum_required(VERSION 3.10)
project(Launcher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt 5 自动检测
# 先尝试 Qt5，失败则尝试 Qt6
find_package(Qt5 COMPONENTS Core Quick Qml Sql Network Gui Widgets QUIET)

if (Qt5_FOUND)
    message(STATUS "Using Qt 5")
    set(QT_VERSION_MAJOR 5)
else()
    find_package(Qt6 COMPONENTS Core Quick Qml Sql Network Gui Widgets QUIET)
    if (Qt6_FOUND)
        message(STATUS "Using Qt 6")
        set(QT_VERSION_MAJOR 6)
    else()
        message(FATAL_ERROR "Neither Qt5 nor Qt6 was found. Please install Qt.")
    endif()
endif()

# Qt 5 默认组件（无需额外链接）
# Qt5::QuickControls2 在 Qt5Quick 中包含

# 启用 GraphicalEffects
# QtGraphicalEffects 在 Qt 5 中是独立模块，但通常作为 import 使用
# 不需要在 CMake 中显式链接

# 源文件
set(SOURCES
    main.cpp
    src/data/Database.cpp
    src/data/Settings.cpp
    src/data/ApplicationRepository.cpp
    src/data/FileIndexer.cpp
    src/data/FileRepository.cpp
    src/domain/Application.cpp
    src/domain/FileIndex.cpp
    src/app/SingleInstance.cpp
    src/app/HotkeyManager.cpp
    src/services/SearchEngine.cpp
    src/viewmodel/SearchModel.cpp
    src/viewmodel/SearchViewModel.cpp
)

# 头文件（如需要则添加）
set(HEADERS
    src/data/Database.h
    src/data/Settings.h
    src/data/ApplicationRepository.h
    src/data/FileIndexer.h
    src/data/FileRepository.h
    src/domain/Application.h
    src/domain/FileIndex.h
    src/app/SingleInstance.h
    src/app/HotkeyManager.h
    src/services/SearchEngine.h
    src/viewmodel/SearchModel.h
    src/viewmodel/SearchViewModel.h
)

# 资源文件
qt_add_resources(RESOURCES resources.qrc)

# 可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 添加 include 目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/viewmodel
)

# 链接 Qt 库
if (QT_VERSION_MAJOR EQUAL 5)
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Quick
        Qt5::Qml
        Qt5::Sql
        Qt5::Network
    )
else()
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Sql
        Qt6::Network
    )
endif()

# 安装目标
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 安装资源文件
install(FILES resources/data/launcher.desktop
    DESTINATION share/applications
)

install(FILES resources/icons/launcher.svg
    DESTINATION share/icons/hicolor/scalable/apps
)

# 测试程序
add_executable(backend-test
    src/tests/backend-test.cpp
    src/data/Database.cpp
    src/data/Settings.cpp
    src/data/ApplicationRepository.cpp
    src/data/FileIndexer.cpp
    src/data/FileRepository.cpp
    src/domain/Application.cpp
    src/domain/FileIndex.cpp
)

target_include_directories(backend-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
)

if (QT_VERSION_MAJOR EQUAL 5)
    target_link_libraries(backend-test
        Qt5::Core
        Qt5::Sql
        Qt5::Gui
    )
else()
    target_link_libraries(backend-test
        Qt6::Core
        Qt6::Sql
        Qt6::Gui
    )
endif()
