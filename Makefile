CFLAGS = -Werror -Wextra -Wall -MMD -g3 -fPIC

INCLUDES = -Iincludes -I./lib/ft_printf/includes -I./lib/libft/includes -I./lib/get_next_line/includes

LIBPRINTF = lib/ft_printf/lib/libprintf.a
LIBFT = lib/libft/lib/libft.a
GNL = lib/get_next_line/lib/getnextline.a

LDFLAGS = -lreadline
VALGRIND = valgrind

VGFLAGS  = --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --errors-for-leak-kinds=all \
            --track-fds=yes \
            --num-callers=25 \
            --exit-on-first-error=no \
            --log-file=valgrind.log \
			--suppressions=supp.supp

SRC_DIR = src
OBJ_DIR = objs

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPENDS = $(OBJS:.o=.d)

NAME = minishell

-include $(DEPENDS)

.PHONY: all
all: $(NAME)

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


# Arguments passés à l'exécutable: make valgrind ARGS="…"
ARGS ?=

# Pour Valgrind, recompile en debug propre (-O0), garde -g3
VALGRIND_CFLAGS = $(filter-out -O%,$(CFLAGS)) -O0

.PHONY: valgrind vg vg-clean
valgrind: CFLAGS := $(VALGRIND_CFLAGS)
valgrind: re
	@$(VALGRIND) $(VGFLAGS) ./$(NAME) $(ARGS)
	@echo "==> Report: valgrind.log"

vg: valgrind

vg-clean:
	@rm -f valgrind.log
	@echo "valgrind.log removed"