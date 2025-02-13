BUILD_DIR     = build
PROGRAM       = output.exe
DEBUG_ENABLED = 1

CC_PATH = gcc

C_SOURCE_LIST = \
	lib/glad.c \
	src/main.c \
	src/game/wof.c \
	src/gfx/camera.c \
	src/gfx/mesh2d.c \
	src/gfx/shader.c \
	src/gfx/tex_array.c \
	src/gfx/texture.c \
	src/input/callbacks.c \
	src/input/window.c \
	src/util/error_handling.c \
	src/util/generic_impl.c \
	src/util/memory_safety.c

C_INCLUDE_LIST = 

C_OBJECT_LIST =
C_OBJECT_LIST += $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCE_LIST:.c=.o)))

vpath %.c $(sort $(dir $(C_SOURCE_LIST)))

LIB_OBJECT_LIST = \
	lib/libglfw3.a \
	lib/OpenAL32.lib \
	lib/libsndfile.a \
	-lopengl32 \
	-lglu32 \
	-lgdi32 \
	-lkernel32 \
	-luser32

LIB_INCLUDE_LIST = -I include -I src

WARNINGS     = -Wall
FLAGS_DEBUG  = -g -D MEMORY_DEBUG_ON
FLAGS_OPT	 	 = -O3
FLAGS_DEPS   = -MMD -MP
FLAGS_OPENAL = 

FLAGS =
FLAGS += $(C_INCLUDE_LIST)
FLAGS += $(LIB_INCLUDE_LIST)
FLAGS += $(FLAGS_DEPS)
FLAGS += $(WARNINGS)
FLAGS += $(FLAGS_OPENAL)

ifneq ($(DEBUG_ENABLED), 0)
FLAGS += $(FLAGS_DEBUG)
else 
FLAGS += $(FLAGS_OPT)
endif

$(BUILD_DIR)/$(PROGRAM) : $(C_OBJECT_LIST) makefile | $(BUILD_DIR)
	$(CC_PATH) -o $(@) $(C_OBJECT_LIST) $(LIB_OBJECT_LIST)

$(BUILD_DIR)/%.o : %.c makefile | $(BUILD_DIR)
	$(CC_PATH) -c -o $(@) $(<) $(FLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
	copy lib\\OpenAL32.lib $(BUILD_DIR)\\OpenAL32.lib
	copy lib\\OpenAL32.dll $(BUILD_DIR)\\OpenAL32.dll

clean:
	rmdir /s /q $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)

