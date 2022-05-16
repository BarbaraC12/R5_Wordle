NAME			= wordle

FLAGS 			= -Wall -Wextra -Werror

CC				= cc

INC				=-I. 

SRC_NAME		= core.c shell.c web.c

OBJ_NAME		=$(SRC_NAME:.c=.o)

SRC_PATH		=src

OBJ_PATH		=.obj

SRC=$(addprefix $(SRC_PATH)/,$(SRC_NAME))

OBJS=$(addprefix $(OBJ_PATH)/,$(OBJ_NAME)) 

TEST=

all: makedir term #web

makedir:
	@mkdir -p .obj
	@mkdir -p out

test: FLAGS += -g3 -fsanitize=address,undefined
test: TEST = test
test: makedir $(NAME)

term: makedir $(OBJ_PATH)/main.o $(OBJS)
	$(CC) $(FLAGS) $(OBJ_PATH)/main.o  $(OBJS) $(INC) -o $(NAME)

web: FLAGS += --embed-file wordlists
web: CC = emcc
web: makedir
	$(CC) $(FLAGS) $(SRC) main.c $(INC) -o out/$(NAME).js

$(OBJ_PATH)/main.o: main.c
	 $(CC) $(FLAGS)  $(INC) -c main.c -o $(OBJ_PATH)/main.o 
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(FLAGS)  $(INC) -c $< -o $@

clean:
	rm -fr $(OBJ_PATH)

fclean: clean
	rm -fr $(NAME)
	rm -fr out

re: fclean all

