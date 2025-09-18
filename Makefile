CFLAGS = -Werror -Wextra -Wall -MMD -MP -g3 -fPIC

INCLUDES = -Iincludes -I./lib/ft_printf/includes -I./lib/libft/includes -I./lib/get_next_line/includes

LIBPRINTF = lib/ft_printf/lib/libprintf.a
LIBFT = lib/libft/lib/libft.a
GNL = lib/get_next_line/lib/getnextline.a

LDFLAGS = -lreadline

SRC_DIR = src
OBJ_DIR = objs

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPENDS = $(OBJS:.o=.d)

NAME = minishell


.PHONY: all
all: $(NAME)

-include $(DEPENDS)

$(NAME): $(OBJS) $(LIBPRINTF) $(LIBFT) $(GNL)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBPRINTF):
	$(MAKE) -C lib/ft_printf

$(LIBFT):
	$(MAKE) -C lib/libft

$(GNL):
	$(MAKE) -C lib/get_next_line

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C lib/ft_printf clean
	$(MAKE) -C lib/libft clean
	$(MAKE) -C lib/get_next_line clean

.PHONY: fclean
fclean: clean
	rm -f $(NAME)
	$(MAKE) -C lib/ft_printf fclean
	$(MAKE) -C lib/libft fclean
	$(MAKE) -C lib/get_next_line fclean

.PHONY: re
re: fclean all
