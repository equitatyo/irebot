CC = x86_64-w64-mingw32-gcc-win32
CFLAGS = -Ofast -flto -fno-fat-lto-objects -ffast-math -fPIC

INCLUDES = -I./libraries/GLFW/include -I./libraries/fontstash -I./src/ -I./src/gui/ -I./src/audio/
LFLAGS = -L./libraries/GLFW
LIBS = -lglfw3 -lopengl32 -lwsock32 -lws2_32 -lcomdlg32 -lurlmon

SRCS = $(shell find src/ -type f -name '*.c')
OBJS = $(SRCS:.c=.o)
TARGET = "build/irebot.exe"

.PHONY: clean

all: setup ${TARGET}

setup:
	@[ -d "build" ] || mkdir -p build

${TARGET}: ${OBJS}
	@echo "Compiling"
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)
	@strip -s ${TARGET}

.c.o:
	@echo "Building" $@
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean:
	@echo "Cleaning"
	@${RM} ${OBJS} ${TARGET}