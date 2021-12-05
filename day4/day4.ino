/*
 Name:		day4.ino
 Created:	12/4/2021 11:36:17 pm
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

/**
 * Given the input of a list of values, this function will loop through while filtering out values
 * that don't match our pattern. These patterns are generated by counting the occurance of 1's in that position.
 * Two patterns are used, one where 1's are more common, and 1 where 0's are more common.
 * If only 1 value remains then the count is ignored and that value is chosen for that bit position instead.
 * At the end the two patterns will match a single value in the array, and we return these values multiple together.
 * This function uses branchless techniques to speed up processing.
 * @param input A pointer to our array of values we're processing
 * @param size The length of the array
 * @param bit_size the number of bits in the inputs
 * @return The two patterns multiplied together.
 */
uint8_t play_bingo(
    const int8_t game_input[], uint8_t input_size,
    int8_t game_board[], uint8_t game_board_size
) {
    uint16_t lower = (uint16_t)game_input;
    uint16_t upper = lower + input_size;
    uint8_t board_max = game_board_size * game_board_size;

    for (uint16_t i = lower; i < upper; i++)
    {
        int8_t input = pgm_read_byte_near(i);
        int8_t position = -1;
        while (++position < board_max && game_board[position] != input);

        if (position < board_max) {

            int8_t value = -1;
            game_board[position] = -1;
            int8_t index = position;

            uint8_t left, right, up, down;

            while ((index = index - game_board_size) >= 0 && (value = game_board[index]) == -1);
            up = (value == -1);

            index = position;
            while ((index = index + game_board_size) < board_max && (value = game_board[index]) == -1);
            down = (value == -1);

            if (up && down) return (i - lower);

            int8_t row_min = (position / game_board_size) * game_board_size;
            int8_t row_max = row_min + game_board_size;

            value = -1;
            index = position;
            while (--index >= row_min && (value = game_board[index]) == -1);
            left = (value == -1);

            index = position;
            while (++index < row_max && (value = game_board[index]) == -1);
            right = (value == -1);

            if (left && right) return (i - lower);
        }
    }
    return input_size + 1;
}

/**
 * Given the input of a list of values, this function will loop through and count the number
 * 1's in each bit position. This count is used to then calculate a new value by 
 * placing 1's in each bit position where the count is greater than half than the total
 * number of inputs, aka more 1's than 0's in that position.
 * This value is then multiplied by it's inverse.
 * @param input A pointer to our array of values we're processing
 * @param size The length of the array
 * @param bit_size the number of bits in the inputs
 * @return The calculated value multiplied by it's inverse
 */
uint32_t bingo_bango(
    const int8_t game_input[], uint8_t input_size,
    const int8_t game_boards[], uint16_t boards_size,
    uint8_t game_board_size
)
{
    uint16_t lower = (uint16_t)game_boards;
    uint16_t upper = lower + boards_size;
    uint16_t game_size = game_board_size * game_board_size;

    int8_t active_game[game_size];
    uint16_t score;

    for (uint16_t i = lower; i < upper; i += game_size)
    {
        memcpy_P(active_game, (void *)i, game_size);

        uint8_t result = play_bingo(game_input, input_size, active_game, game_board_size);

        if (result < input_size) {
            input_size = result;
            score = 0;
            for (uint8_t index = 0; index < game_size; index++)
            {
                score += (active_game[index] != -1) * active_game[index];
            }

            score *= pgm_read_byte_near((uint16_t)game_input + result);
        }
    }

    return score;
}

/**
 * Given the input of a list of values, this function will loop through and count the number
 * 1's in each bit position. This count is used to then calculate a new value by 
 * placing 1's in each bit position where the count is greater than half than the total
 * number of inputs, aka more 1's than 0's in that position.
 * This value is then multiplied by it's inverse.
 * @param input A pointer to our array of values we're processing
 * @param size The length of the array
 * @param bit_size the number of bits in the inputs
 * @return The calculated value multiplied by it's inverse
 */
uint32_t bingo_bongo(
    const int8_t game_input[], uint8_t input_size,
    const int8_t game_boards[], uint16_t boards_size,
    uint8_t game_board_size
)
{
    uint16_t lower = (uint16_t)game_boards;
    uint16_t upper = lower + boards_size;
    uint16_t game_size = game_board_size * game_board_size;

    int8_t active_game[game_size];
    uint16_t score;
    uint8_t loser = 0;

    for (uint16_t i = lower; i < upper; i += game_size)
    {
        memcpy_P(active_game, (void *)i, game_size);

        uint8_t result = play_bingo(game_input, input_size, active_game, game_board_size);

        if (result > loser) {
            loser = result;
            score = 0;
            for (uint8_t index = 0; index < game_size; index++)
            {
                score += (active_game[index] != -1) * active_game[index];
            }

            score *= pgm_read_byte_near((uint16_t)game_input + result);
        }
    }

    return score;
}

void setup()
{
    Serial.begin(115200);

    // Waiting for the serial to start
    while (!Serial) {}

    // Small delay because I was having Serial Print issues
    delay(1500);

    uint16_t startTime;
    uint16_t endTime;
    uint16_t duration;
    uint32_t value = 0;

    Serial.println(F("Beginning Day 4 - Part 1"));

    startTime = micros();
    value = bingo_bango(
        GAME_INPUT, sizeof(GAME_INPUT),
        GAME_BOARDS, sizeof(GAME_BOARDS),
        BOARD_SIZE
    );
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("bingo_bango time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("bingo_bango value: "));
    Serial.println(value);

    Serial.println(F("Beginning Day 4 - Part 2"));

    startTime = micros();
    value = bingo_bongo(
        GAME_INPUT, sizeof(GAME_INPUT),
        GAME_BOARDS, sizeof(GAME_BOARDS),
        BOARD_SIZE
    );
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("bingo_bongo time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("bingo_bongo value: "));
    Serial.println(value);
}

void loop() { }