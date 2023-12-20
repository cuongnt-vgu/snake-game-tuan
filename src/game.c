#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "linked_list.h"
#include "mbstrings.h"

void updateSnake(int** cells, size_t width, node_t* positions) {
    node_t* temp = positions;
    temp = temp -> next;
    int previous_pos[2];
    int* position;

    while (temp) {
        position = temp -> data;

        previous_pos[0] = position[0];
        previous_pos[1] = position[1];

        switch (position[2]) {
            case 0: position[0]--; break;
            case 1: position[0]++; break;
            case 2: position[1]--; break;
            case 3: position[1]++; break;
        }

        (*cells)[width * previous_pos[0] + previous_pos[1]] = FLAG_PLAIN_CELL;
        (*cells)[width * position[0] + position[1]] = FLAG_SNAKE;

        temp = temp -> next;
    }
}

void updatePositionVector(node_t* positions) {
    node_t* temp = positions;
    int* position;
    int* prevPos;

    while (temp -> next) {
        temp = temp -> next;
    }

    while (temp -> prev) {
        position = temp -> data;
        prevPos = temp -> prev -> data;

        position[2] = prevPos[2];

        temp = temp -> prev;
    }
}

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

    //preprocess input
    if (g_score != 0) {
        switch (input) {
            case INPUT_UP: if (position[2] == 1) input = INPUT_DOWN; break;
            case INPUT_DOWN: if (position[2] == 0) input = INPUT_UP; break;
            case INPUT_LEFT: if (position[2] == 3) input = INPUT_RIGHT; break;
            case INPUT_RIGHT: if (position[2] == 2) input = INPUT_LEFT; break;
            default: break;
        } 
    }

    switch (input) {
        case INPUT_UP: position[0]--; position[2] = 0; break;
        case INPUT_DOWN: position[0]++; position[2] = 1; break;
        case INPUT_LEFT: position[1]--; position[2] = 2; break;
        case INPUT_RIGHT: position[1]++; position[2] = 3; break;
        case INPUT_NONE: {
            switch (position[2]) {
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

    if (cells[width * position[0] + position[1]] == FLAG_SNAKE) {
        int* temp = get_last(snake_p -> position);
        if (temp[0] != position[0] || temp[1] != position[1]) {
            g_game_over = 1;
            return;
        }
    }

    if (cells[width * position[0] + position[1]] == FLAG_FOOD) {
        g_score++;
        if (growing) {
            int* lastPos = get_last(snake_p -> position);
            int newSnakeCellPos[3] = {lastPos[0], lastPos[1], lastPos[2]};
            if (g_score <= 1) {
                switch (newSnakeCellPos[2]) {
                    case 0: newSnakeCellPos[0] += 2; break;
                    case 1: newSnakeCellPos[0] -= 2; break;
                    case 2: newSnakeCellPos[1] += 2; break;
                    case 3: newSnakeCellPos[1] -= 2; break;
                }
            }
            else {
                switch (newSnakeCellPos[2]) {
                    case 0: newSnakeCellPos[0] += 1; break;
                    case 1: newSnakeCellPos[0] -= 1; break;
                    case 2: newSnakeCellPos[1] += 1; break;
                    case 3: newSnakeCellPos[1] -= 1; break;
                }
            }

            insert_last(&snake_p -> position, newSnakeCellPos, sizeof(int)*3);
        }
        place_food(cells, width, height);
    }

    cells[width * previous_pos[0] + previous_pos[1]] = FLAG_PLAIN_CELL;
    cells[width * position[0] + position[1]] = FLAG_SNAKE;

    updateSnake(&cells, width, snake_p -> position);
    updatePositionVector(snake_p -> position);
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
    printf("Name > ");
    fflush(0);
    int numRead = read(0, write_into, 1000);
    while (numRead <= 1) {
        printf("Name Invalid: must be longer than 0 characters.\n");
        printf("Name > ");
        fflush(0);
        numRead = read(0, write_into, 1000);
    }

    write_into[strcspn(write_into, "\n")] = 0;
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
