#include "game_setup.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "game.h"

// Some handy dandy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39
#define DELIMITER 0x7C

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 20;
    *height_p = 10;
    int* cells = malloc(20 * 10 * sizeof(int));
    *cells_p = cells;
    for (int i = 0; i < 20 * 10; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 20; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (20 * (10 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 10; ++i) {
        cells[i * 20] = FLAG_WALL;
        cells[i * 20 + 20 - 1] = FLAG_WALL;
    }

    // Add snake
    cells[20 * 2 + 2] = FLAG_SNAKE;
    snake_pos[0] = 2;
    snake_pos[1] = 2;

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 2!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    if (board_rep != NULL) {
        enum board_init_status result = decompress_board_str(cells_p, width_p, height_p, snake_p, board_rep);

        if (result != INIT_SUCCESS) {
            return result;
        }
    } 
    else {
        initialize_default_board(cells_p, width_p, height_p);
    }
    g_game_over = 0;
    g_score = 0;
    snake_direction = 3;
    place_food(*cells_p, *width_p, *height_p);

    return INIT_SUCCESS;
}

/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 2!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p,
                                            size_t* height_p, snake_t* snake_p,
                                            char* compressed) {
    char** main_save_ptr = &compressed;
    char size_delimiter = 'x';
    char delimiter = DELIMITER;
    char* board_argument = strtok_r(compressed, &delimiter, main_save_ptr);
    char** board_save_ptr = &board_argument;
    char temp = board_argument[0];

    int snake_pos_found = 0;
    uint current_height = 1;
    uint current_width;
    int current_flag;
    int current_run;

    board_argument[0] = '0';

    *height_p = atoi(strtok_r(board_argument, &size_delimiter, board_save_ptr));
    *width_p = atoi(strtok_r(NULL, &size_delimiter, board_save_ptr));

    int* cells = malloc(*height_p * *width_p * sizeof(int));
    *cells_p = cells;
    int* current_position = *cells_p;

    if (temp != 'B') {
        return INIT_ERR_BAD_CHAR;
    }

    board_argument = strtok_r(NULL, &delimiter, main_save_ptr);

    while (board_argument != NULL) {
        if (current_height > *height_p) {
            return INIT_ERR_INCORRECT_DIMENSIONS;
        }

        current_width = 0;

        while (*board_argument != '\0') {
            switch (*board_argument) {
                case E_CAP_HEX:
                case E_LOW_HEX: {
                    current_flag = FLAG_PLAIN_CELL;
                    break;
                }

                case W_CAP_HEX:
                case W_LOW_HEX: {
                    current_flag = FLAG_WALL;
                    break;
                }

                case S_CAP_HEX:
                case S_LOW_HEX: {
                    current_flag = FLAG_SNAKE;
                    break;
                }

                default: {
                    return INIT_ERR_BAD_CHAR;
                }
            }

            board_argument++;
            current_run = 0;

            while (*board_argument >= DIGIT_START && *board_argument <= DIGIT_END) {
                current_run *= 10;
                current_run += *board_argument - 48;
                board_argument++;
            }

            current_width += current_run;
            
            if (current_flag == FLAG_SNAKE) {
                snake_pos[0] = current_height - 1;
                snake_pos[1] = current_width - 1;
                snake_pos_found += current_run;
            }

            if (current_width > *width_p) {
                return INIT_ERR_INCORRECT_DIMENSIONS;
            }

            for (int i = 0; i < current_run; i++) {
                *current_position = current_flag;
                current_position++;
            }

        }

        if (current_width != *width_p) {
            return INIT_ERR_INCORRECT_DIMENSIONS;
        }

        current_height += 1;
        board_argument = strtok_r(NULL, &delimiter, main_save_ptr);
    }

    if (current_height-1 < *height_p) {
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    if (snake_pos_found != 1) {
        return INIT_ERR_WRONG_SNAKE_NUM;
    }

    return INIT_SUCCESS;
}
