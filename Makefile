include config.mk

all: ${CONFIG} ${FINAL}

${CONFIG}: ${CONFIG_DEF}
	cp ${CONFIG_DEF} ${CONFIG}

${FINAL}: ${MAIN} ${CONFIG}
	${CC} ${MAIN} -o $@ ${X11} ${CFLAGS} ${DEFINES}