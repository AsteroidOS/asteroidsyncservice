cmake_minimum_required(VERSION 3.15)
project(asteroidsyncservice VERSION 2.0.0 
    DESCRIPTION "A synchronization daemon for AsteroidOS watches"
)

# options off-by-default that you can enable
option(SAILFISHOS_PLATFORM "Build the project for the SailfishOS platform" OFF)
option(UBUNTU_TOUCH_PLATFORM "Build the project for the Ubuntu Touch platform" OFF)
option(WITH_DOC "Create project documentation" OFF)

if (SAILFISHOS_PLATFORM)
    set(PLATFORM_SOURCE_DIR "platforms/sailfishos")
    set(DAEMON_PLATFORM_SOURCE_DIR "platforms/sailfishos")
    set(SYSTEMD_SERVICE_NAME "asteroidsync.service")
elseif (UBUNTU_TOUCH_PLATFORM)
    set(PLATFORM_SOURCE_DIR "platforms/ubuntutouch")
    set(DAEMON_PLATFORM_SOURCE_DIR "platforms/ubuntutouch")
    set(SYSTEMD_SERVICE_NAME "")
elseif (DESKTOP_PLATFORM)
    set(PLATFORM_SOURCE_DIR "platforms/sailfishos")
    set(DAEMON_PLATFORM_SOURCE_DIR "platforms/ubuntutouch")
    set(SYSTEMD_SERVICE_NAME "asteroidsync.service")
else()
    message(FATAL_ERROR "no platform selected: you must enable one of SAILFISHOS_PLATFORM or UBUNTU_TOUCH_PLATFORM or DESKTOP_PLATFORM")
endif() 

include(GNUInstallDirs)

configure_file(
    "asteroidsyncservice_config.h.in"
    "asteroidsyncservice_config.h"
)
add_subdirectory(asteroidsyncservice)
add_subdirectory(asteroidsyncserviced)
if (WITH_DOC)
    add_subdirectory(doc)
endif()
# add config file 
# OTHER_FILES += \
 #   README.md \
 #   rpm/asteroidsyncservice.spec
