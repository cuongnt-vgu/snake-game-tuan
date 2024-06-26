#include "mbstrings.h"

/* mbslen - multi-byte string length
 * - Description: returns the number of UTF-8 code points ("characters")
 * in a multibyte string. If the argument is NULL or an invalid UTF-8
 * string is passed, returns -1.
 *
 * - Arguments: A pointer to a character array (`bytes`), consisting of UTF-8
 * variable-length encoded multibyte code points.
 *
 * - Return: returns the actual number of UTF-8 code points in `src`. If an
 * invalid sequence of bytes is encountered, return -1.
 *
 * - Hints:
 * UTF-8 characters are encoded in 1 to 4 bytes. The number of leading 1s in the
 * highest order byte indicates the length (in bytes) of the character. For
 * example, a character with the encoding 1111.... is 4 bytes long, a character
 * with the encoding 1110.... is 3 bytes long, and a character with the encoding
 * 1100.... is 2 bytes long. Single-byte UTF-8 characters were designed to be
 * compatible with ASCII. As such, the first bit of a 1-byte UTF-8 character is
 * 0.......
 *
 * You will need bitwise operations for this part of the assignment!
 */

const int checkBytes[3] = {0xe0, 0xf0, 0xf8};
const int compareBytes[3] = {0xc0, 0xe0, 0xf0};

size_t mbslen(const char* bytes) {
    const char* currentPos = bytes;
    int len = 0;

    while (*currentPos != '\0') {
        int flag = 0;
        for (int i = 0; i < 3; i++) {
            if ((*currentPos & checkBytes[i]) == compareBytes[i]) {
                len++;
                currentPos += i+2;
                flag = 1;
                break;
            }
        }
        if (flag) continue;
        len++;
        currentPos++;
    }

    return len;
}
