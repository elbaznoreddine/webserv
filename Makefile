CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPP = c++

SRC = srcs/main.cpp srcs/Server.cpp

OBJ = $(SRC:.cpp=.o)

NAME = webserv

all : $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o:%.cpp headers/Server.hpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all