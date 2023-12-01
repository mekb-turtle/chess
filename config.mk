TARGET = chess
VERSION = 1.0.0

EXTRA_SRC_FILES =
EXTRA_BINARY_FILES =
CFLAGS += -Wall
LDLIBS += -lSDL2 -lSDL2_image -lm
ifeq ($(shell uname -o),Msys)
CPPFLAGS += -DWINDOWSFONT
else
LDLIBS += -lfontconfig
CPPFLAGS += -DFONTCONFIG
endif
