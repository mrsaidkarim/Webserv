NAME = webserv

CPP = c++

FLAGS =  -std=c++17 -g -fsanitize=address  #-Wall -Wextra -Werror

RM = rm -f

SRCS = 	ParseConfigFile/ParserConfig.cpp\
		Configuration/Location.cpp\
		Configuration/Server.cpp\
		Request/HttpRequest.cpp\
		Response/HttpResponse.cpp\
		Response/get_method.cpp\
		Response/post_method.cpp\
		Response/delete_method.cpp\
		Response/error_response.cpp\
		Response/cgi.cpp\
		WebServ.cpp\
		main.cpp

HEADERS = 	ParseConfigFile/ParserConfig.hpp\
			Configuration/Location.hpp\
			Configuration/Server.hpp\
			Request/HttpRequest.hpp\
			Response/HttpResponse.hpp\
			WebServ.hpp\
			const.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)
	@mkdir -p ./Session_Management/

%.o: %.cpp $(HEADERS)
	$(CPP) $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(CPP) $(FLAGS) $^ -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)
	rm -rf ./Session_Management/

re: fclean all

.PHONY: clean fclean re