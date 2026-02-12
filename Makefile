CXX = c++
PORT = 8080
CFLAGS = -Wall -Werror -Wextra -g3 -std=c++98

NAME = serv

SRC =	src/get_response.cpp \
		src/parse_request.cpp \
		src/Request.cpp \
		src/CGI.cpp \
		src/multi_srv.cpp \
		src/client.cpp \
		src/get_response_code.cpp \
		src/response.cpp \
		src/post_response.cpp \
		src/delete_response.cpp \
		src/html_path.cpp \
		src/autoindex.cpp \
		src/Request.cpp \
		src/main.cpp \
		src/Config.cpp
		
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

OBJ_DIR = build/

OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)%.o)

$(OBJ_DIR)%.o : src/%.cpp
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