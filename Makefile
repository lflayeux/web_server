CXX = c++
PORT = 8080
CFLAGS = -Wall -Werror -Wextra -g3 -std=c++98

NAME = serv

SRC =	src/get_response.cpp \
		src/webserv.cpp \
		src/main.cpp
		
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

OBJ_DIR = build/

OBJ = $(SRC:src/%.c=$(OBJ_DIR)%.o)

$(OBJ_DIR)%.o : src/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJ)
	@$(CXX) $(CFLAGS) -o $@ $^
	@echo "$(GREEN)Compilation successful ! ✅ $(RESET)"

all : $(NAME)

clean :
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)All object files cleaned. 🧹$(RESET)"

fclean : clean
	@rm -f $(NAME)
	@echo "$(YELLOW)Executable cleaned. 🧹$(RESET)"

re : fclean all

.PHONY: clean fclean re