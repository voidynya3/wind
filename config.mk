X11_DEP = `pkg-config --libs x11` `pkg-config --cflags x11`

SOURCE = src/main.c
FINAL = target/wind

CFLAGS = ${SOURCE} -o ${FINAL} ${X11_DEP}
