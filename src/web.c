#include "wordle.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

EM_JS(void, put_color, (int col, int line, int status), {
        let row = document.getElementsByClassName("letter-row")[line];
        let box = row.children[col];

        if (status === 1) {
            color = "green";
        }
        else if (status === 2) {
            color = "yellow";
        }
        else if (status === 3) {
            color = "grey";
        }
        let delay = 50 * col;
        box.style.backgroundColor = color;
        // color keyboard:
        for (const elem of document.getElementsByClassName("keyboard-button")) {
            if (elem.textContent === box.textContent) {
                let oldColor = elem.style.backgroundColor;
                if (oldColor === 'green') {
                    return;
                } 

                if (oldColor === 'yellow' && color !== 'green') {
                    return;
                }
                elem.style.backgroundColor = color;
                break;
            }
        }
});

static void insert_key(t_web_state *s, char key) {
    if (!s->state)
        return;
    if (s->cursor == 5) {
        return;
    }
    EM_ASM({
        let pressedKey = $0;
        let box = $1;
        let row = $2;
        
        let r = document.getElementsByClassName("letter-row")[row];
        let b = r.children[box];
        b.textContent = String.fromCharCode(pressedKey);
        b.classList.add("filled-box");
    }, key, s->cursor, s->guess_count);
    s->word[s->cursor] = key;
    s->cursor += 1;
}

static void delete_key(t_web_state *s) {
    if (!s->state)
        return;
    if (s->cursor == 0)
        return;
    EM_ASM({
        let box = $0;
        let row = $1;
        
        let r = document.getElementsByClassName("letter-row")[row];
        let b = r.children[box - 1];
        b.textContent = "";
        b.classList.remove("filled-box");
    }, s->cursor, s->guess_count);
    s->word[s->cursor] = 0;
    s->cursor -= 1;
}

static void match_wordle(t_web_state *s) {
    if (!s->state)
        return;
    if (strlen(s->word) < 5)
        return;
    if (s->guess_count == 6) {
        s->state = false;
    }

    bool ret = check_word_non_existent(s->word, s->fd);
    if (ret == true) {
        EM_ASM({
            var button = document.getElementById("clickable");
            var snackbar = document.getElementById("snackbar");
            snackbar.textContent = "This word is not in the dictionary!";
            button.click();
        });
        return;
    }

    t_letter_status letters[MAX_WORD];
    int r = check_word_status(s->wordle, s->word, letters);
    if (r == 42) {
        EM_ASM({
            var button = document.getElementById("clickable");
            var snackbar = document.getElementById("snackbar");
            snackbar.textContent = "Congratulations, you won!!!";
            button.click();
        });
        s->state = false;
    }
    else {
        if (s->guess_count == 5) {
            EM_ASM({
                var button = document.getElementById("clickable");
                var snackbar = document.getElementById("snackbar");
                snackbar.textContent = "GAME OVER ! The word was : " + AsciiToString($0) + " !";
                button.click();
                console.log ("Word was : ", AsciiToString($0));
            }, s->wordle);
        }
        else {
            EM_ASM({
                var button = document.getElementById("clickable");
                var snackbar = document.getElementById("snackbar");
                if ($0 === 1) {
                    snackbar.textContent = "One more time !!! " + $0 + "guess left";
                }
                else {
                    snackbar.textContent = "Nice try, you can try again !!! " + $0 + "guesses left";
                }

            button.click();
            }, 6 - s->guess_count - 1);
        }
    }
    for (int i = 0; i < WORD_SIZE; i++) {
        put_color(i, s->guess_count, letters[i]);
    }
    s->cursor = 0;
    memset(s->word, 0, WORD_SIZE + 1);
    s->guess_count += 1;
}

static EM_BOOL handle_keyup(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
    t_web_state *s = (t_web_state *)userData; (void)s;
    if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
        if (strcmp(keyEvent->key, "Enter") == 0) {
            match_wordle(s);
        }
        else if (strcmp(keyEvent->key, "Backspace") == 0) {
            delete_key(s);
        }
        else {
            if (!((strlen(keyEvent->key) > 1) || !isalpha(keyEvent->key[0])))
                insert_key(s, tolower(keyEvent->key[0]));
        }
    }   
    return 0;
}

static void loop(void* userData) {
    t_web_state *s = (t_web_state *)userData;
    if (!s->state) {
        // pop up box with number of guesses
        EM_ASM( console.error("pressed enter"));
        emscripten_cancel_main_loop();
    }
}

void web_game_loop(const char *wordle, int fd) {
    t_web_state s = { .cursor=0, .guess_count=0, .state=true, .wordle=wordle, .fd=fd };
    memset(s.word, 0, WORD_SIZE+1);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, &s, 1, handle_keyup);
    emscripten_set_main_loop_arg(loop, &s, 0, 1);
    // FIXME: cette fonction ne marche pas comme prévu, pe à cause de useCapture? (cf. doc mdn)
    // because of that, on doit verifier s->state pour ne pas que le callback continue d'etre appele
    emscripten_html5_remove_all_event_listeners();
    emscripten_force_exit(0);
}
#endif