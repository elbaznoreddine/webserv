CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserv

SRC = main.cpp Request.cpp Response.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ) Request.hpp Response.hpp
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp Request.hpp Response.hpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

srv:
	make -C nginx_test

srv-exec:
	docker exec -it nginx_webserv sh

srv-down:
	make down -C nginx_test

srv-clean:
	make clean -C nginx_test

srv-fclean: srv-clean
	make fclean -C nginx_test

srv-re: srv-fclean srv