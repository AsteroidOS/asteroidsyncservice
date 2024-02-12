cmake_minimum_required(VERSION 3.15)
find_package(Qt6 COMPONENTS DBus Qml)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.12 COMPONENTS DBus Qml REQUIRED)
    add_library(Qt::DBus ALIAS Qt5::DBus)
    add_library(Qt::Qml ALIAS Qt5::Qml)
endif()
add_library(asteroidsyncserviceplugin SHARED syncservice_plugin.cpp watch.cpp watches.cpp ${PLATFORM_SOURCE_DIR}/servicecontrol.cpp)
set_target_properties(asteroidsyncserviceplugin PROPERTIES AUTOMOC ON)
target_include_directories(asteroidsyncserviceplugin PRIVATE ${PROJECT_BINARY_DIR} ${PLATFORM_SOURCE_DIR})
target_compile_features(asteroidsyncserviceplugin PUBLIC cxx_std_17)
target_link_libraries(asteroidsyncserviceplugin PRIVATE Qt::DBus Qt::Qml)


# Installation paths
#get_target_property(REAL_QMAKE_EXECUTABLE Qt::qmake IMPORTED_LOCATION)
find_program(REAL_QMAKE_EXECUTABLE NAMES qmake6 qmake)

if (NOT QT_INSTALL_QML)
    execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_QML
                    OUTPUT_VARIABLE QT_INSTALL_QML
                    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

install(TARGETS asteroidsyncserviceplugin DESTINATION ${QT_INSTALL_QML}/org/asteroid/syncservice)
install(FILES qmldir DESTINATION ${QT_INSTALL_QML}/org/asteroid/syncservice)
