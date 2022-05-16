#include "wordle.h"
#ifndef __EMSCRIPTEN__
/* =========================================================================*/
/*                                   PRINT                                  */
/* =========================================================================*/

static void print_letter_status(char buff, t_letter_status status) {
    switch (status) {
        case UNDEFINED:
            printf(" \033[1;37m_\033[0m ");
            break;
        case CORRECT:
            printf(" \033[1;32m%c\033[0m ", buff);
            break;
        case WRONG_PLACE:
            printf(" \033[1;33m%c\033[0m ", buff);
            break;
        case INCORRECT:
            printf(" \033[1;37m%c\033[0m ", buff);
            break;
    }
}

/**
 * @brief		Print the letters according to the status
 *
 * @param		buff[MAX_WORD][WORD_SIZE] array of words given by the user
 * @param		status[MAX_WORD][WORD_SIZE] array of status of the words
 */
static void print_term(char buff[MAX_WORD][BUFF_SIZE], t_letter_status status[MAX_WORD][WORD_SIZE]) {
    for(int i = 0; i < MAX_WORD; i++) {
        printf("\t\t\t");
        for (int j = 0; j < WORD_SIZE; j++) {
            print_letter_status(buff[i][j], status[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * @brief		Print the board game 
 *
 * @param		buff[MAX_WORD][WORD_SIZE] array of words given by the user
 * @param		status[MAX_WORD][WORD_SIZE] array of status of the words
 */
static void print_title(char buff[MAX_WORD][BUFF_SIZE], t_letter_status status[MAX_WORD][WORD_SIZE]) {
    printf("\033[2J\033[1;1H\n\n\033[1;33m");
    printf("\t██╗    ██╗ ██████╗ ██████╗ ██████╗ ██╗     ███████╗\n");
    printf("\t██║    ██║██╔═══██╗██╔══██╗██╔══██╗██║     ██╔════╝\n");
    printf("\t██║ █╗ ██║██║   ██║██████╔╝██║  ██║██║     █████╗  \n");
    printf("\t██║███╗██║██║   ██║██╔══██╗██║  ██║██║     ██╔══╝  \n");
    printf("\t╚███╔███╔╝╚██████╔╝██║  ██║██████╔╝███████╗███████╗\n");
    printf("\t ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝\033[0m\n");
    printf("\t===================================================\n\n");
    print_term(buff, status);
}

/**
 * @brief		Print the prompt and check is word is valid
 *
 * @param		buff pointer to the word array
 * @param		fd file descriptor of the dictionary
 * @return		true if success, false if error
 */
static bool print_prompt(char *buff, int fd) {
    printf("\n\tYour word > ");
    if (scanf("%s", buff) < 0) {
        if (fd != -1) close (fd);
        exit(0);
    }
    for (int l = 0; l < WORD_SIZE; l++) 
        buff[l] = tolower(buff[l]);
    if (strlen(buff) != WORD_SIZE)
        return false;
    if (check_word_non_existent(buff, fd) == true)
        return false;
    return true;
}

/* =========================================================================*/
/*                                GAME  LOOP                                */
/* =========================================================================*/

/**
 * @brief		Playable loop 
 *
 * @param		word word to find
 * @param		fd file descriptor of the dictionnary
 * @param		nb_try number of try
 * @return		game status (0 if LOSE, 42 if WIN)
 */
int shell_game_loop(char *word, int fd, unsigned int *nb_try) {
    t_letter_status result[MAX_WORD][WORD_SIZE];
    char    buff[MAX_WORD][BUFF_SIZE];
    bool    valid = true;
    int     ret = 0;


    memset(result, 0, sizeof(t_letter_status) * MAX_WORD * WORD_SIZE);
    memset(buff, 0, MAX_WORD * WORD_SIZE);
    for ((*nb_try) = 0; *nb_try < MAX_WORD; (*nb_try)++) {
        memset(buff[*nb_try], 0, WORD_SIZE);
        print_title(buff, result);
        valid = print_prompt(buff[*nb_try], fd);
        while (valid == false) {
            printf("\033[A\033[2K\033[A\033[2K\t\t     \033[1;31mInvalid word: %s\033[0m", buff[*nb_try]);
            valid = print_prompt(buff[*nb_try], fd);
        }
        ret = check_word_status(word, buff[*nb_try], result[*nb_try]);
        if (ret == 42)
            break ;
    }
    print_title(buff, result);
    return ret;
}
#endif

