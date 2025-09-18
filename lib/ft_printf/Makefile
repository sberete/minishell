CFLAGS := -Wall -Wextra -Werror -MMD -g3

INCLUDE_DIR := includes

INCLUDES := -I$(INCLUDE_DIR)

OBJ_DIR := objs

SRC_DIR := srcs

SRCS := $(shell find $(SRC_DIR) -name '*.c')

OBJS := $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(notdir $(basename $(SRCS)))))
DEPEND = $(OBJS:.o=.d)

LIB_FOLDER := lib

LIB_NAME := libprintf.a

LIB := $(LIB_FOLDER)/$(LIB_NAME)

all : $(LIB)

$(LIB) : $(OBJS)
		@mkdir -p lib
		ar rcs $@ $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPEND)

$(OBJ_DIR): ; @mkdir -p $@

.PHONY : re
re : fclean $(LIB)

.PHONY : fclean
fclean : clean
		rm -rf $(LIB_FOLDER) $(OBJ_DIR)

.PHONY : clean
clean :
		rm -rf $(OBJ_DIR)