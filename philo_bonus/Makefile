# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/27 18:23:38 by anemet            #+#    #+#              #
#    Updated: 2025/07/30 10:08:59 by anemet           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo
NAME_BONUS = philo_bonus
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g

SRCS = main_bonus.c \
		init_bonus.c \
		routine_bonus.c \
		monitor_bonus.c \
		utils_bonus.c

OBJS = $(SRCS:.c=.o)

all: $(NAME_BONUS)
	cp $(NAME_BONUS) $(NAME)

$(NAME_BONUS): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME_BONUS)

%.o: %.c philo_bonus.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME_BONUS)
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
