#include "wordle.h"

int get_rand_word_from_file(int fd, char **word) {
    if (fd == -1) {
        perror("wordle: get_rand_word_from_file: invalid file descriptor");
        return 1;
    }
    struct stat buf;
    time_t t;
    srand((unsigned) time(&t));
    
    int ret = fstat(fd, &buf);
    if (ret != 0) {
        perror("wordle: get_rand_word_from_file: failed to stat file");
        return 1;
    }
    if (!S_ISREG(buf.st_mode)) {
        perror("wordle: get_rand_word_from_file: is not a regular file");
        return 1;
    }

    off_t offset = 0;
    if (buf.st_size) {
        // 1. On rapporte rand() à un range qui nous intéresse (dans la taille du fichier)
        off_t tmp = (rand() % buf.st_size) / (WORD_SIZE + 1);
        // 2. on utilise tmp pour retrouver des valeurs d'offset valides (début de la ligne)
        offset = tmp - (tmp % 6);
    }
    
    if (offset) {
        off_t new_offset = lseek(fd, offset, SEEK_SET);
        if (new_offset != offset) {
            perror("wordle: get_rand_word_from_file: seek failed to update file offset");
            return 1;
        }
    }

    size_t rd = read(fd, word, WORD_SIZE);
    if (rd != WORD_SIZE) {
        printf("wordle: get_rand_word_from_file: invalid file %zu \n", rd);
        return 1;
    }

    return 0;
}

int check_word_status(const char *wordle, const char *word, t_letter_status *result) {
    assert(wordle);
    assert(word);
    assert(result);

    size_t len_word = strlen(word);
    unsigned var = 0;
    char tmp[WORD_SIZE];
    strncpy(tmp, wordle, WORD_SIZE);
    
    for (unsigned i = 0; i < len_word; i++) {
        if (tmp[i] == word[i]) {
            result[i] = CORRECT;
            tmp[i] = ' ';
            var ++;
        }
        else {
            if (strchr(tmp, word[i])) {
                result[i] = WRONG_PLACE;
                // pop it up to avoid counting it twice
                for (int j = 0; j < WORD_SIZE; j++) {
                    if (tmp[j] == word[i]) {
                        tmp[j] = ' ';
                        break;
                    }
                }
            }
            else {
                result[i] = INCORRECT;
            }
        }
    }

    if (var == WORD_SIZE) {
        return 42;
    }
    return 0;
}

/**
 * @brief		Check if the word given by player is in Dict
 *
 * @param		buff pointer to the word array
 * @param		fd file descriptor of the dictionary
 * @return		true if success, false if error
 */
bool check_word_non_existent(char *buff, int fd) {
    // FIXME: naive implementation.
    // TODO:
    // 1. check if the dictionary is sorted
    // 2. quickly find the first (then sec. etc..) letter until you find the complete word
    char tmp[WORD_SIZE +1];
    
    lseek(fd, 0, SEEK_SET);
    memset(tmp, 0, WORD_SIZE +1);

    while (read(fd, tmp, WORD_SIZE)) {
        lseek(fd, 1, SEEK_CUR);
        if (strncmp(buff, tmp, WORD_SIZE) == 0) {
            return false;
        }
        memset(tmp, 0, WORD_SIZE +1);
    }
    return true;
}