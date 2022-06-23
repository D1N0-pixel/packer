CC = gcc

CXXFLAGS = 

SRC_DIR = ./src
OBJ_DIR = ./obj

TARGET = packer

SRCS := $(notdir $(wildcard $(SRC_DIR)/*.c))
# SRCS:= main.c parse.c
OBJS := $(SRCS:.c=.o)

OBJECTS = $(patsubst %.o, $(OBJ_DIR)/%.o, $(OBJS))
DEPS = $(OBJECTS:.o=.d)

$(info ${OBJECTS})

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
		$(CC) $(CXXFLAGS) -c $< -o $@ -MD

$(TARGET): $(OBJECTS)
		$(CC) $(CXXFLAGS) $^ -o $@

.PHONY: clean all
clean:
		rm -f $(OBJECTS) $(DEPS) $(TARGET)

-include $(DEPS)