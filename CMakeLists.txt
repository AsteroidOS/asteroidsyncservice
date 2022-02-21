cmake_minimum_required(VERSION 3.15)
project(asteroidsyncservice VERSION 1.0.0)

# options off-by-default that you can enable
option(SAILFISHOS_PLATFORM "Build the project for the SailfishOS platform" OFF)
option(UBUNTU_TOUCH_PLATFORM "Build the project for the Ubuntu Touch platform" OFF)

if (SAILFISHOS_PLATFORM)
    set(PLATFORM_SOURCE_DIR "platforms/sailfishos")
    set(DAEMON_PLATFORM_SOURCE_DIR "platforms/sailfishos")
elseif (UBUNTU_TOUCH_PLATFORM)
    set(PLATFORM_SOURCE_DIR "platforms/ubuntutouch")
    set(DAEMON_PLATFORM_SOURCE_DIR "platforms/ubuntutouch")
else()
    message(FATAL_ERROR "no platform selected: you must enable either SAILFISHOS_PLATFORM or UBUNTU_TOUCH_PLATFORM")
endif() 

configure_file(
    "asteroidsyncservice_config.h.in"
    "asteroidsyncservice_config.h"
)
add_subdirectory(asteroidsyncservice)
add_subdirectory(asteroidsyncserviced)
# add config file 
# OTHER_FILES += \
 #   README.md \
 #   rpm/asteroidsyncservice.spec
