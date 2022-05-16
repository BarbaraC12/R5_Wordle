#include "wordle.h"

static void print_win() {
    printf("\033[1;36m");
    printf("      __   __   ___    _   _          __      __ ___    _  _  \n"); 
    printf("      \\ \\ / /  / _ \\  | | | |    o O O\\ \\    / /|_ _|  | \\| | \n"); 
    printf("       \\ V /  | (_) | | |_| |   o      \\ \\/\\/ /  | |   | .` | \n"); 
    printf("       _|_|_   \\___/   \\___/   TS__[O]  \\_/\\_/  |___|  |_|\\_| \n"); 
    printf("     _| *** |_|*****|_|*****| {======|_|*****|_|*****|_|*****| \n");
    printf("     *`-0-0-'*`-0-0-'*`-0-0-'./o--000'*`-0-0-'*`-0-0-'*`-0-0-'\033[0m\n\n");
}

static void print_lose() {
    printf("\033[1;31m");
    printf(" __   __   ___    _   _             _       ___     ___     ___   \n");
    printf(" \\ \\ / /  / _ \\  | | | |    o O O  | |     / _ \\   / __|   | __|  \n");
    printf("  \\ V /  | (_) | | |_| |   o       | |__  | (_) |  \\__ \\   | _|   \n");
    printf("  _|_|_   \\___/   \\___/   TS__[O]  |____|  \\___/   |___/   |___|  \n");
    printf("_| *** |_|*****|_|*****| {======|_|*****|_|*****|_|*****|_|*****| \n");
    printf("*`-0-0-'*`-0-0-'*`-0-0-'./o--000'*`-0-0-'*`-0-0-'*`-0-0-'*`-0-0-' \033[0m\n\n");
}

/* =========================================================================*/
/*                                   MAIN                                   */
/* =========================================================================*/

int main (int ac, char **av) {
    assert(WORD_SIZE == 6);
    (void)ac,(void)av;
    int fd = open(FILEPATH, O_RDONLY);
    if (!fd) return 1;
    char word[WORD_SIZE + 1] = {0};
    int ret = get_rand_word_from_file(fd, (char **)&word);
    if(ret != 0) return 1;
#ifndef __EMSCRIPTEN__
    unsigned int try = 0;
    ret = shell_game_loop(word, fd, &try);
    if (ret == 42) {
        print_win();
        printf("\t    >>> Nice job, you found %s in %u try <<<\n", word, try+1);
    }
    else {
        print_lose();
        printf("\t\t>>> Too bad, the word was %s <<<\n", word);
    }
#else
    (void)print_lose, (void)print_win;
    web_game_loop(word, fd);
#endif
    if (fd != -1) close (fd);
    return 0;
}
