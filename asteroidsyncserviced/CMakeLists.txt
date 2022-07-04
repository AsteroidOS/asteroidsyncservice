cmake_minimum_required(VERSION 3.15)
find_package(Qt5 COMPONENTS Core Bluetooth DBus Qml REQUIRED)

add_subdirectory(libasteroid)
if (SAILFISHOS_PLATFORM)
    set(NOTIFICATION_MONITOR ON)
    set(WALL_TIME ON)
    set(MUSIC ON)
    add_subdirectory(libwatchfish)
endif()

if(NOT DEFINED Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR)
    include (CheckCXXSourceCompiles)
    find_package(Qt5 COMPONENTS Core REQUIRED)
    set(CMAKE_REQUIRED_INCLUDES ${Qt5Core_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES Qt5::Core)
    set(Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR 0)
    check_cxx_source_compiles("
#include <QObject>
#include <memory>

class TestObjectPrivate {
    int dummy{42};
};

class TestObject: public QObject
{
    Q_DECLARE_PRIVATE(TestObject);
    std::unique_ptr<TestObjectPrivate> d_ptr;
};

int main() {
    TestObject test_object;
}
"

    Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR)
endif()

add_executable(asteroidsyncserviced 
    main.cpp
    watchesmanager.cpp
    dbusinterface.cpp
    bluez/bluezclient.cpp
    bluez/freedesktop_objectmanager.cpp
    bluez/freedesktop_properties.cpp
    "${DAEMON_PLATFORM_SOURCE_DIR}/platform.cpp"
)
set_target_properties(asteroidsyncserviced PROPERTIES AUTOMOC ON)
target_compile_features(asteroidsyncserviced PUBLIC cxx_std_17)
add_compile_definitions(Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR=${Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR})
target_include_directories(asteroidsyncserviced PRIVATE ${PROJECT_BINARY_DIR} ${DAEMON_PLATFORM_SOURCE_DIR} libasteroid/src bluez)
target_link_libraries(asteroidsyncserviced PRIVATE Qt5::Bluetooth Qt5::Core Qt5::DBus Qt5::Qml asteroid)

install(TARGETS asteroidsyncserviced)
if (DESKTOP_PLATFORM)
    configure_file(
        "asteroidsync.service.in"
        "asteroidsync.service"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/asteroidsync.service DESTINATION /usr/lib/systemd/user) 
endif()

if (SAILFISHOS_PLATFORM)
    target_link_libraries(asteroidsyncserviced PRIVATE watchfish)
    configure_file(
        "asteroidsync.service.in"
        "asteroidsync.service"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/asteroidsync.service DESTINATION /usr/lib/systemd/user)
endif ()
