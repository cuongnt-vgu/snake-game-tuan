#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "linked_list.h"
#include "mbstrings.h"

/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 2!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // `update` should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (`g_score`)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.
    if (g_game_over == 1) {
        return;
    }

    int previous_pos[2];
    int* position = get_first(snake_p -> position);
    previous_pos[0] = position[0];
    previous_pos[1] = position[1];

    switch (input) {
        case INPUT_UP: position[0]--; snake_p -> direction = 0; break;
        case INPUT_DOWN: position[0]++; snake_p -> direction = 1; break;
        case INPUT_LEFT: position[1]--; snake_p -> direction = 2; break;
        case INPUT_RIGHT: position[1]++; snake_p -> direction = 3; break;
        case INPUT_NONE: {
            switch (snake_p -> direction) {
                case 0: position[0]--; break;
                case 1: position[0]++; break;
                case 2: position[1]--; break;
                case 3: position[1]++; break;
            }
        }
    }

    // stop the game when the step it is about to take is a wall
    if (cells[width * position[0] + position[1]] == FLAG_WALL) {
        g_game_over = 1;
        return;
    }

    if (cells[width * position[0] + position[1]] == FLAG_FOOD) {
        g_score++;
        place_food(cells, width, height);
    }

    cells[width * previous_pos[0] + previous_pos[1]] = FLAG_PLAIN_CELL;
    cells[width * position[0] + position[1]] = FLAG_SNAKE;
}

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    if (*(cells + food_index) == FLAG_PLAIN_CELL) {
        *(cells + food_index) = FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - `write_into`: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    // TODO: implement! (remove the call to strcpy once you begin your
    // implementation)
    strcpy(write_into, "placeholder");
}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 2)
 */
void teardown(int* cells, snake_t* snake_p) {
    free(cells);

    int* temp;
    while (temp != NULL) {
        temp = remove_first(&snake_p -> position);
        free(temp);
    }

    free(snake_p -> position);
}
