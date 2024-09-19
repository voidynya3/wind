X11 = `pkg-config --libs --cflags x11`

MAIN = wind.c
FINAL = target/wind

CONFIG_DEF = config.def.h
CONFIG = config.h

CFLAGS = -std=c99 -pedantic -Werror

VERSION = "0.1"
GIT_SOURCE = "github.com/voidynya3/wind"
DEFINES = -D'WIND_VERSION=${VERSION}' -D'WIND_GIT_SOURCE=${GIT_SOURCE}'