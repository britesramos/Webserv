NAME = webserv

CPPFLAGS = -Wall -Werror -Wextra -std=c++17

WHITE_B = \033[1;37m
PINK = \033[35m
B_GREEN = \033[0;102m
CYAN = \033[36m
BOLD = \033[1m
RESET = \033[0m

OBJ_DIR = obj
SRC_DIR = src
UPLOAD_DIR = www/uploads

SOURCE = main.cpp \
		ConfigParser.cpp \
		ConfigParser_utils.cpp \
		ServerConfig.cpp \
		Location.cpp \
		Client.cpp \
		Server.cpp \
		Webserver.cpp \
		Cgi.cpp \


SRC = $(addprefix $(SRC_DIR)/, $(SOURCE))
OBJECTS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)


$(NAME): $(OBJECTS)
	@c++ $(CPPFLAGS) $(OBJECTS) -o $(NAME)
	@echo "$(CYAN)$(BOLD)\n	$(NAME) is ready$(RESET)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@cc $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "✅"

fclean: clean
	@rm -f $(NAME)
	@if [ -d "$(UPLOAD_DIR)" ]; then \
		echo "$(PINK)$(BOLD) Removing uploaded files...$(RESET)"; \
		rm -rf "$(UPLOAD_DIR)"; \
	fi
	@echo "$(B_GREEN)$(WHITE_B) ALL Cleansed! $(RESET)"

re: fclean all

.PHONY: all clean fclean re

