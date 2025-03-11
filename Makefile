NAME = ircserv

SRCS = 	main.cpp \
	Server.cpp \
	ServerConnection.cpp \
	ServerCommand.cpp \
	Client.cpp \
	Channel.cpp \
	Commands/Nick.cpp \
	Commands/User.cpp \
	Commands/Help.cpp \
	Commands/Pass.cpp \
	Commands/Privmsg.cpp \
	Commands/Join.cpp \
	Commands/Topic.cpp \
	Commands/Kick.cpp \
	Commands/Mode.cpp \
	Commands/Invite.cpp \
	development/utils.cpp

OBJS = $(SRCS:.cpp=.o)
FLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJS)
	c++ $(FLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 
