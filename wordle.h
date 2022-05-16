#ifndef WORDLE_H
# define WORDLE_H

# include <assert.h>
# include <ctype.h>
# include <fcntl.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <time.h>
# include <unistd.h>
 
# ifndef WORD_SIZE
#  define WORD_SIZE 5
# endif
# ifndef MAX_WORD
#  define MAX_WORD 6
# endif
# ifndef BUFF_SIZE
#  define BUFF_SIZE 1024
# endif
# ifndef FILEPATH 
#  define FILEPATH "./wordlists/words.txt"
# endif


typedef enum e_letter_status {
    UNDEFINED,
    CORRECT,
    WRONG_PLACE,
    INCORRECT
} t_letter_status;

typedef struct e_web_state {
    unsigned cursor;
    unsigned guess_count;
    bool state;
    const char *wordle;
    char word[WORD_SIZE + 1];
    int fd;
} t_web_state;

int shell_game_loop(char *, int, unsigned int *);
void web_game_loop(const char *, int);
bool check_word_non_existent(char *buff, int fd);
int get_rand_word_from_file(int, char **);
int check_word_status(const char *, const char *, t_letter_status *);

#endif