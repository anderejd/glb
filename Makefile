# Merge windows makefile (?)

ifeq ($(OS),Windows_NT)
else
        UNAME_S := $(shell uname -s)
        ifeq ($(UNAME_S),Linux)
                LDLIBS = -lSDL2 -lGL -lm
                CFLAGS = -Wall -Wextra -Werror -pedantic -O3
                INC = -Ilibs/glad/include
                DEBUGGER = gdb
                DEBUGFLAGS = -ggdb
        endif
        ifeq ($(UNAME_S),Darwin)
                LDLIBS = -framework SDL2 -framework OpenGL 
                CFLAGS = -Wall -Wextra -Werror -pedantic -O3
                INC = -Ilibs/glad/include
                INC += -I/Library/Frameworks/SDL2.framework/Headers
                DEBUGGER = lldb
                DEBUGFLAGS = -g
        endif
        UNAME_P := $(shell uname -p)
        ifeq ($(UNAME_P),x86_64)
        endif
        ifneq ($(filter %86,$(UNAME_P)),)
        endif
        ifneq ($(filter arm%,$(UNAME_P)),)
        endif
endif

LAUNCHER = 

ifeq ($(DEBUG), 1)
        CFLAGS += $(DEBUGFLAGS)
        LAUNCHER = $(DEBUGGER)
endif

all_and_start: bin/glb
	$(LAUNCHER) bin/glb

bin/glb: bin obj/glad.o src/glb.c
	$(CC) $(CFLAGS) $(INC) obj/glad.o src/glb.c -o bin/glb $(LDLIBS)

obj/glad.o: obj libs/glad/src/glad.c libs/glad/include/glad/glad.h libs/glad/include/KHR/khrplatform.h
	$(CC) $(CFLAGS) $(INC) -c libs/glad/src/glad.c -o obj/glad.o

obj:
	mkdir obj

bin:
	mkdir bin

clean:
	-rm -r -f obj
	-rm -r -f bin
	-rm log.txt

