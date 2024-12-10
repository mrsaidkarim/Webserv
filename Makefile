NAME = a.out

CPP = c++

FLAGS =  -std=c++11 -fsanitize=address  #-Wall -Wextra -Werror

RM = rm -f

SRCS = Configuration/Location.cpp main.cpp Configuration/Server.cpp Request/HttpRequest.cpp WebServ.cpp

HEADERS = Configuration/Location.hpp Configuration/Server.hpp Request/HttpRequest.hpp const.hpp WebServ.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp $(HEADERS)
	$(CPP) $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(CPP) $(FLAGS) $^ -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean fclean re