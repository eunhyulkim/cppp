NAME = philosopher
CC = clang++
RM = rm -rf
INCLUDES = -I./includes/
CFLAGS = -Wall -Wextra -Werror

MAIN_FILES = inherit start exception error help cmd_main strong_getter function
			 cppp get make
TEST_FILES = Caos main game

SRCS = $(addsuffix .cpp, $(MAIN_FILES))
SRCS += $(addsuffix .cpp, $(TEST_FILES))

OBJS = $(addsuffix .o, $(MAIN_FILES))
OBJS += $(addsuffix .o, $(TEST_FILES))

GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

${NAME}: ${OBJS}
	@echo "$(GREEN)$(NAME) START$(RESET)"
	@echo "$(RESET)Compiling ${NAME} to root."
	@${CC} ${CFLAGS} ${INCLUDES} ${OBJS} -o ${NAME}
	@echo "$(GREEN)DONE$(RESET)"

${OBJS}: ${SRCS}
	@${CC} ${CFLAGS} ${INCLUDES} -c ${SRCS}

clean:
	@echo "$(RESET)Cleaning your .o files$(RESET)"
	@${RM} $(OBJS)
	@echo "$(RED)clean DONE$(RESET)"

fclean: clean
	@echo "$(RESET)Removing $(NAME)"
	@${RM} $(NAME)
	@echo "$(RED)fclean DONE$(RESET)"

re: fclean all

.PHONY:	all clean fclean re bonus
