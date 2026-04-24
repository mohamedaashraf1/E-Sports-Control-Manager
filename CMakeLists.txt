cmake_minimum_required(VERSION 3.16)

project(EsportsTournamentManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    tournamentsystem.cpp
    queuemanager.cpp
    linkedlist.cpp
)

set(PROJECT_HEADERS
    mainwindow.h
    tournamentsystem.h
    queuemanager.h
    linkedlist.h
    node.h
    player.h
    match.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EsportsTournamentManager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
else()
    add_executable(EsportsTournamentManager
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
endif()

target_link_libraries(EsportsTournamentManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Make sure QSS files are available next to executable at runtime.
add_custom_command(TARGET EsportsTournamentManager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/styles.qss
            $<TARGET_FILE_DIR:EsportsTournamentManager>/styles.qss
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/styles_purple.qss
            $<TARGET_FILE_DIR:EsportsTournamentManager>/styles_purple.qss
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/styles_emerald.qss
            $<TARGET_FILE_DIR:EsportsTournamentManager>/styles_emerald.qss
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(EsportsTournamentManager)
endif()
