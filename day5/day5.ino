/*
 Name:		day3.ino
 Created:	12/3/2021 8:42:20 PM
 Author:	Tosso
*/

#include "input_test.h"
#include <avr/pgmspace.h>

#define SERIAL_BUFFER_SIZE 8

#define FLAGS_SIZE (960 >> 3)
#define set_flag(f,g,b) f[g] & b ? 0 : (f[g] |= b) > 0

const uint8_t DATA_SIZE = sizeof(int16_t);

enum line_direction {
    verticle, horizontal, diagonal_up, diagonal_down
};

static inline int16_t process_range(
    int16_t lower, int16_t upper, uint8_t *flags,
    int16_t line_1_x_1, int16_t line_1_y_1,
    int16_t line_1_x_2, int16_t line_1_y_2,
    uint8_t line_1_direction, int16_t *in_h, int16_t *in_v, int16_t *x_h, int16_t *x_v
)
{
    int16_t result = 0;
    // Comparing line_1_index to all upcoming values
    for (int16_t line_2_index = lower; line_2_index < upper; line_2_index += DATA_SIZE)
    {
        // Reading the 4 16 bit values
        int16_t line_2_x_1 = pgm_read_word_near(line_2_index);
        int16_t line_2_y_1 = pgm_read_word_near(line_2_index += DATA_SIZE);
        int16_t line_2_x_2 = pgm_read_word_near(line_2_index += DATA_SIZE);
        int16_t line_2_y_2 = pgm_read_word_near(line_2_index += DATA_SIZE);

        // Ignore Diagnols
        if ((line_2_x_1 != line_2_x_2) && (line_2_y_1 != line_2_y_2)) continue;

        // Flip so x1/y1 is always lower than x2/y2
        if (line_2_x_1 > line_2_x_2)
        {
            line_2_x_1 ^= line_2_x_2;
            line_2_x_2 ^= line_2_x_1;
            line_2_x_1 ^= line_2_x_2;
        }

        if (line_2_y_1 > line_2_y_2)
        {
            line_2_y_1 ^= line_2_y_2;
            line_2_y_2 ^= line_2_y_1;
            line_2_y_1 ^= line_2_y_2;
        }


        // Getting the line orientation
        line_direction line_2_direction = line_2_x_1 == line_2_x_2 ? verticle : horizontal;

        // Line1/2 Orientation are the same
        if (line_1_direction == line_2_direction)
        {
            if (line_1_direction == horizontal) // Comparing horizontal lines, checking they're on the same y value and overlap on x values
            {
                if (line_1_y_1 == line_2_y_1 && (line_1_x_1 <= line_2_x_2 && line_1_x_2 >= line_2_x_1))
                {
                    int16_t overlap_minimum = max(line_1_x_1, line_2_x_1);
                    int16_t overlap_index = overlap_minimum - line_1_x_1;
                    int16_t overlap_length = (min(line_1_x_2, line_2_x_2) - overlap_minimum) + overlap_index;

                    for (size_t i = overlap_index; i <= overlap_length; i++)
                    {
                        result += set_flag(flags, i >> 3, _BV(i % 8));
                    }
                }
            }
            else  // Comparing verticle lines, checking they're on the same x value and overlap on y values
            {
                if (line_1_x_1 == line_2_x_1 && (line_1_y_1 <= line_2_y_2 && line_1_y_2 >= line_2_y_1))
                {

                    int16_t overlap_minimum = max(line_1_y_1, line_2_y_1);
                    int16_t overlap_index = overlap_minimum - line_1_y_1;
                    int16_t overlap_length = min(line_1_y_2, line_2_y_2) - overlap_minimum + overlap_index;

                    for (size_t i = overlap_index; i <= overlap_length; i++)
                    {
                        result += set_flag(flags, i >> 3, _BV(i % 8));
                    }                    
                }
            }
        }
        else if (line_1_direction == horizontal) // line_1_index is horizontal, line_2_index is verticle. Verifying they cross
        {
            if (line_2_x_1 >= line_1_x_1 && line_2_x_1 <= line_1_x_2 && line_1_y_1 >= line_2_y_1 && line_1_y_1 <= line_2_y_2)
            {
                int16_t difference = (line_2_x_1 - line_1_x_1);
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
        else  // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
        {
            if (line_2_y_1 >= line_1_y_1 && line_2_y_1 <= line_1_y_2 && line_1_x_1 >= line_2_x_1 && line_1_x_1 <= line_2_x_2)
            {
                int16_t difference = (line_2_y_1 - line_1_y_1);
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
    }
    return result;
}

static inline int16_t process_range_complete(
    int16_t lower, int16_t upper, uint8_t *flags,
    int16_t line_1_x_1, int16_t line_1_y_1,
    int16_t line_1_x_2, int16_t line_1_y_2,
    uint8_t line_1_direction
)
{
    int16_t result = 0;
    // Comparing line_1_index to all upcoming values
    for (int16_t line_2_index = lower; line_2_index < upper; line_2_index += DATA_SIZE)
    {
        // Reading the 4 16 bit values
        int16_t line_2_x_1 = pgm_read_word_near(line_2_index);
        int16_t line_2_y_1 = pgm_read_word_near(line_2_index += DATA_SIZE);
        int16_t line_2_x_2 = pgm_read_word_near(line_2_index += DATA_SIZE);
        int16_t line_2_y_2 = pgm_read_word_near(line_2_index += DATA_SIZE);

        // Ignore bad diagonals
        if ((line_2_x_1 != line_2_x_2) && (line_2_y_1 != line_2_y_2) && abs(line_2_y_2 - line_2_y_1) != abs(line_2_x_2 - line_2_x_1)) continue;

        if (line_2_x_1 > line_2_x_2)
        {
            line_2_x_1 ^= line_2_x_2;
            line_2_x_2 ^= line_2_x_1;
            line_2_x_1 ^= line_2_x_2;

            if (line_2_y_1 != line_2_y_2)
            {
                line_2_y_1 ^= line_2_y_2;
                line_2_y_2 ^= line_2_y_1;
                line_2_y_1 ^= line_2_y_2;
            }            
        }
        else if (line_2_x_1 == line_2_x_2 && line_2_y_1 > line_2_y_2)
        {
            line_2_y_1 ^= line_2_y_2;
            line_2_y_2 ^= line_2_y_1;
            line_2_y_1 ^= line_2_y_2;
        }

        // Getting the line orientation
        line_direction line_2_direction;
        if (line_2_x_1 == line_2_x_2)
        {
            line_2_direction = verticle;
        }
        else if (line_2_y_1 == line_2_y_2)
        {
            line_2_direction = horizontal;
        }
        else if (line_2_y_1 > line_2_y_2)
        {
            line_2_direction = diagonal_up;
        }
        else
        {
            line_2_direction = diagonal_down;
        }
        
        // Line1/2 Orientation are the same
        if (line_1_direction == line_2_direction)
        {
            if (line_1_direction == horizontal) // Comparing horizontal lines, checking they're on the same y value and overlap on x values
            {
                if (line_1_y_1 == line_2_y_1 && (line_1_x_1 <= line_2_x_2 && line_1_x_2 >= line_2_x_1))
                {
                    int16_t overlap_minimum = max(line_1_x_1, line_2_x_1);
                    int16_t overlap_index = overlap_minimum - line_1_x_1;
                    int16_t overlap_length = (min(line_1_x_2, line_2_x_2) - overlap_minimum) + overlap_index;

                    for (size_t i = overlap_index; i <= overlap_length; i++)
                    {
                        result += set_flag(flags, i >> 3, _BV(i % 8));
                    }
                }
            }
            else if (line_1_direction == verticle)  // Comparing verticle lines, checking they're on the same x value and overlap on y values
            {
                if (line_1_x_1 == line_2_x_1 && (line_1_y_1 <= line_2_y_2 && line_1_y_2 >= line_2_y_1))
                {
                    int16_t overlap_minimum = max(line_1_y_1, line_2_y_1);
                    int16_t overlap_index = overlap_minimum - line_1_y_1;
                    int16_t overlap_length = min(line_1_y_2, line_2_y_2) - overlap_minimum + overlap_index;

                    for (size_t i = overlap_index; i <= overlap_length; i++)
                    {
                        result += set_flag(flags, i >> 3, _BV(i % 8));
                    }                    
                }
            }
            else
            {
                if (abs(line_2_x_1 - line_1_x_1) == abs(line_2_y_1 - line_1_y_1) && (line_1_x_1 <= line_2_x_2 && line_1_x_2 >= line_2_x_1))
                {
                    int16_t overlap_minimum = max(line_1_x_1, line_2_x_1);
                    int16_t overlap_index = overlap_minimum - line_1_x_1;
                    int16_t overlap_length = (min(line_1_x_2, line_2_x_2) - overlap_minimum) + overlap_index;

                    for (size_t i = overlap_index; i <= overlap_length; i++)
                    {
                        result += set_flag(flags, i >> 3, _BV(i % 8));
                    }
                }
            }
        
        }
        else if (line_1_direction == horizontal && line_2_direction == verticle) // line_1_index is horizontal, line_2_index is verticle. Verifying they cross
        {
            if (line_2_x_1 >= line_1_x_1 && line_2_x_1 <= line_1_x_2 && line_1_y_1 >= line_2_y_1 && line_1_y_1 <= line_2_y_2)
            {
                int16_t difference = (line_2_x_1 - line_1_x_1);
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
        else if (line_1_direction == verticle && line_2_direction == horizontal) // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
        {
            if (line_2_y_1 >= line_1_y_1 && line_2_y_1 <= line_1_y_2 && line_1_x_1 >= line_2_x_1 && line_1_x_1 <= line_2_x_2)
            {
                int16_t difference = (line_2_y_1 - line_1_y_1);
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
        else if ((line_1_direction == diagonal_down || line_1_direction == diagonal_up) && line_2_direction == horizontal) // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
        {
            int16_t cross_point = (line_1_direction == diagonal_up ? -1 : 1) * (line_2_y_1 - line_1_y_1) + line_1_x_1;
            
            if (cross_point >= line_2_x_1 && cross_point <= line_2_x_2)
            {
                int16_t difference = abs(cross_point - line_1_x_1);                
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
        else if ((line_1_direction == diagonal_down || line_1_direction == diagonal_up) && line_2_direction == verticle) // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
        {
            int16_t cross_point = (line_1_direction == diagonal_up ? -1 : 1) * (line_2_x_1 - line_1_x_1) + line_1_y_1;
            
            if (cross_point >= line_2_y_1 && cross_point <= line_2_y_2)
            {
                int16_t difference = abs(cross_point - line_1_y_1);
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
        else if (line_1_direction == diagonal_down && line_2_direction == diagonal_up) // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
        {
            int16_t height_difference = line_1_y_1 - line_2_y_1;
            if (height_difference > 0 && !(height_difference & 1))
            {
                int16_t index = (height_difference >> 1);
                int16_t cross_point = index + line_1_x_1;
                if (cross_point >= line_2_x_1 && cross_point <= line_2_x_2)
                {
                    result += set_flag(flags, index >> 3, _BV(index % 8));
                }
            }
        }
        else if (line_1_direction == horizontal && (line_2_direction == diagonal_down || line_2_direction == diagonal_up)) // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
        {
            int16_t cross_point = (line_2_direction == diagonal_up ? -1 : 1) * (line_1_y_1 - line_2_y_1) + line_2_x_1;
            
            if (cross_point >= line_1_x_1 && cross_point <= line_1_x_2)
            {
                int16_t difference = cross_point - line_1_x_1;
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
        else
        {
            int16_t cross_point = (line_2_direction == diagonal_up ? -1 : 1) * (line_1_x_1 - line_2_x_1) + line_2_y_1;
            if (cross_point >= line_1_y_1 && cross_point <= line_1_y_2)
            {
                

                int16_t difference = abs(cross_point - line_1_y_1);
                result += set_flag(flags, difference >> 3, _BV(difference % 8));
            }
        }
    }
    return result;
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
int16_t process_report_complete(const int16_t input[], const int16_t size)
{
    const uint8_t data_size = sizeof(int16_t);

    const int16_t lower = (int16_t)input;
    const int16_t upper = lower + (size);
    const int16_t upper_main = lower + (size) - data_size;

    uint8_t flags[FLAGS_SIZE];

    int16_t result = 0;

    for (int16_t line_1_index = lower; line_1_index < upper_main; ) //line_1_index += step_size
    {
        int16_t index = line_1_index;
        int16_t line_1_x_1 = pgm_read_word_near(line_1_index);
        int16_t line_1_y_1 = pgm_read_word_near(line_1_index += data_size);
        int16_t line_1_x_2 = pgm_read_word_near(line_1_index += data_size);
        int16_t line_1_y_2 = pgm_read_word_near(line_1_index += data_size);

        line_1_index += data_size;
        
        // Ignore bad diagonals
        if ((line_1_x_1 != line_1_x_2) && (line_1_y_1 != line_1_y_2) && abs(line_1_y_2 - line_1_y_1) != abs(line_1_x_2 - line_1_x_1)) continue;

        if (line_1_x_1 > line_1_x_2)
        {
            line_1_x_1 ^= line_1_x_2;
            line_1_x_2 ^= line_1_x_1;
            line_1_x_1 ^= line_1_x_2;

            if (line_1_y_1 != line_1_y_2)
            {
                line_1_y_1 ^= line_1_y_2;
                line_1_y_2 ^= line_1_y_1;
                line_1_y_1 ^= line_1_y_2;
            }            
        }
        else if (line_1_x_1 == line_1_x_2 && line_1_y_1 > line_1_y_2)
        {
            line_1_y_1 ^= line_1_y_2;
            line_1_y_2 ^= line_1_y_1;
            line_1_y_1 ^= line_1_y_2;
        }

        // Getting the line orientation
        line_direction line_1_direction;
        if (line_1_x_1 == line_1_x_2)
        {
            line_1_direction = verticle;
        }
        else if (line_1_y_1 == line_1_y_2)
        {
            line_1_direction = horizontal;
        }
        else if (line_1_y_1 > line_1_y_2)
        {
            line_1_direction = diagonal_up;
        }
        else
        {
            line_1_direction = diagonal_down;
        }
        memset(flags, 0, FLAGS_SIZE);

        process_range_complete(lower, index, flags,
            line_1_x_1, line_1_y_1, line_1_x_2, line_1_y_2,
            line_1_direction
        );

        result += process_range_complete(line_1_index, upper, flags,
            line_1_x_1, line_1_y_1, line_1_x_2, line_1_y_2,
            line_1_direction
        );

        
    }
    
    return result;
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
int16_t process_report(const int16_t input[], const int16_t size)
{
    const uint8_t data_size = sizeof(int16_t);

    const int16_t lower = (int16_t)input;
    const int16_t upper = lower + (size);
    const int16_t upper_main = lower + (size) - data_size;

    uint8_t flags[FLAGS_SIZE];

    int16_t in_h, in_v, x_h, x_v;
    int16_t fin_h, fin_v, fx_h, fx_v;

    in_h = in_v = x_h = x_v = 0;
    fin_h = fin_v = fx_h = fx_v = 0;

    int16_t result = 0;

    for (int16_t line_1_index = lower; line_1_index < upper_main; ) //line_1_index += step_size
    {
        int16_t index = line_1_index;
        int16_t line_1_x_1 = pgm_read_word_near(line_1_index);
        int16_t line_1_y_1 = pgm_read_word_near(line_1_index += data_size);
        int16_t line_1_x_2 = pgm_read_word_near(line_1_index += data_size);
        int16_t line_1_y_2 = pgm_read_word_near(line_1_index += data_size);

        line_1_index += data_size;

        // Ignore Diagnols
        if ((line_1_x_1 != line_1_x_2) && (line_1_y_1 != line_1_y_2)) continue;

        // Flip so x1/y1 is always lower than x2/y2
        if (line_1_x_1 > line_1_x_2)
        {
            line_1_x_1 ^= line_1_x_2;
            line_1_x_2 ^= line_1_x_1;
            line_1_x_1 ^= line_1_x_2;
        }

        if (line_1_y_1 > line_1_y_2)
        {
            line_1_y_1 ^= line_1_y_2;
            line_1_y_2 ^= line_1_y_1;
            line_1_y_1 ^= line_1_y_2;
        }

        // Serial.print(F("line_1_x_1: "));
        // Serial.print(line_1_x_1);
        // Serial.print(F(" line_1_y_1: "));
        // Serial.print(line_1_y_1);
        // Serial.print(F(" line_1_x_2: "));
        // Serial.print(line_1_x_2);
        // Serial.print(F(" line_1_y_2: "));
        // Serial.println(line_1_y_2);

        // Getting the line orientation
        line_direction line_1_direction = line_1_x_1 == line_1_x_2 ? verticle : horizontal;

        memset(flags, 0, FLAGS_SIZE);

        process_range(lower, index, flags,
            line_1_x_1, line_1_y_1, line_1_x_2, line_1_y_2,
            line_1_direction, &fin_h, &fin_v, &fx_h, &fx_v
        );

        result += process_range(line_1_index, upper, flags,
            line_1_x_1, line_1_y_1, line_1_x_2, line_1_y_2,
            line_1_direction, &in_h, &in_v, &x_h, &x_v
        );

        
    }
    
    return result;
}

void setup()
{
    Serial.begin(115200);

    // Waiting for the serial to start
    while (!Serial) {}

    // Small delay because I was having Serial Print issues
    delay(1000);

    uint32_t startTime;
    uint32_t endTime;
    uint32_t duration;
    int16_t value = 0;

    Serial.println(F("Beginning Day 5 - Part 1"));

    startTime = micros();
    value = process_report(LINE_INPUT, sizeof(LINE_INPUT));
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("process_report time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("process_report value: "));
    Serial.println(value);

    Serial.println(F("Beginning Day 5 - Part 2"));

    startTime = micros();
    value = process_report_complete(LINE_INPUT, sizeof(LINE_INPUT));
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("process_report_detailed_branchless time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("process_report_detailed_branchless value: "));
    Serial.println(value);
}

void loop() { }