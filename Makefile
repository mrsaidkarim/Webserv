NAME = webserv

CPP = c++

FLAGS =  -std=c++11   #-Wall -Wextra -Werror

RM = rm -f

SRCS = 	Configuration/Location.cpp\
		Configuration/Server.cpp\
		Request/HttpRequest.cpp\
		Response/HttpResponse.cpp\
		WebServ.cpp\
		main.cpp

HEADERS = 	Configuration/Location.hpp\
			Configuration/Server.hpp\
			Request/HttpRequest.hpp\
			Response/HttpResponse.hpp\
			WebServ.hpp\
			const.hpp

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