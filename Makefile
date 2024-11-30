NAME = a.out

CPP = c++

FLAGS =  -std=c++11 #-Wall -Wextra -Werror

RM = rm -f

SRCS = Configuration/Location.cpp main.cpp

HEADERS = Configuration/Location.hpp const.hpp

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