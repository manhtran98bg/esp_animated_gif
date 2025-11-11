cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES src/AnimatedGIF/src/*.c)
list(APPEND SOURCES src/esp_animated_gif.c)

list(APPEND pub_requires esp_partition)
list(APPEND priv_requires esptool_py spi_flash vfs esp_lcd)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS include
    PRIV_INCLUDE_DIRS src/AnimatedGIF/src
    REQUIRES ${pub_requires}
    PRIV_REQUIRES ${priv_requires}
)

