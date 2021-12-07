/*
 Name:		day3.ino
 Created:	12/3/2021 8:42:20 PM
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

#define SERIAL_BUFFER_SIZE 8

#define FLAGS_SIZE (920 >> 3)
#define set_flag(f,g,b) f[g] & b ? 0 : f[g] |= b

enum line_direction {
    verticle, horizontal
};

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
uint16_t process_report(const uint16_t input[], const uint16_t size)
{
    const uint8_t data_size = sizeof(uint16_t);

    const uint16_t lower = (uint16_t)input;
    const uint16_t upper = lower + (size);

    uint8_t flags[FLAGS_SIZE];

    uint16_t in_h, in_v, x_h, x_v;

    in_h = in_v = x_h = x_v = 0;

    uint16_t result = 0;

    for (uint16_t line_1_index = lower; line_1_index < upper; ) //line_1_index += step_size
    {
        uint16_t sta = line_1_index;
        uint16_t line_1_x_1 = pgm_read_word_near(line_1_index);
        uint16_t line_1_y_1 = pgm_read_word_near(line_1_index += data_size);
        uint16_t line_1_x_2 = pgm_read_word_near(line_1_index += data_size);
        uint16_t line_1_y_2 = pgm_read_word_near(line_1_index += data_size);

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

        // Comparing line_1_index to all upcoming values
        for (uint16_t line_2_index = lower; line_2_index < sta; line_2_index += data_size)
        {
            // Reading the 4 16 bit values
            uint16_t line_2_x_1 = pgm_read_word_near(line_2_index);
            uint16_t line_2_y_1 = pgm_read_word_near(line_2_index += data_size);
            uint16_t line_2_x_2 = pgm_read_word_near(line_2_index += data_size);
            uint16_t line_2_y_2 = pgm_read_word_near(line_2_index += data_size);

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

            
            // Serial.print(F("line_2_x_1: "));
            // Serial.print(line_2_x_1);
            // Serial.print(F(" line_2_y_1: "));
            // Serial.print(line_2_y_1);
            // Serial.print(F(" line_2_x_2: "));
            // Serial.print(line_2_x_2);
            // Serial.print(F(" line_2_y_2: "));
            // Serial.println(line_2_y_2);

            // Getting the line orientation
            line_direction line_2_direction = line_2_x_1 == line_2_x_2 ? verticle : horizontal;

            // Line1/2 Orientation are the same
            if (line_1_direction == line_2_direction)
            {
                if (line_1_direction == horizontal) // Comparing horizontal lines, checking they're on the same y value and overlap on x values
                {
                    if (line_1_y_1 == line_2_y_1 && ((line_1_x_1 >= line_2_x_1 && line_1_x_1 <= line_2_x_2) || (line_1_x_2 >= line_2_x_1 && line_1_x_2 <= line_2_x_2)))
                    {
                        // Serial.println(F("Inline Horizontal"));

                        // Serial.print(F("X Match Min: "));
                        // Serial.print(max(line_1_x_1, line_2_x_1));
                        // Serial.print(F(" max: "));
                        // Serial.print(min(line_1_x_2, line_2_x_2));
                        // Serial.print(F(" y: "));
                        // Serial.println(line_1_y_1);

                        // Incrementing by the cross over length

                        uint16_t minimum = max(line_1_x_1, line_2_x_1);
                        uint16_t start = minimum - line_1_x_1;
                        uint16_t maximum = (min(line_1_x_2, line_2_x_2) - minimum) + start;

                        for (size_t i = start; i <= maximum; i++)
                        {
                            set_flag(flags, i >> 3, _BV(i % 8));                       
                        }
                    }
                }
                else  // Comparing verticle lines, checking they're on the same x value and overlap on y values
                {
                    if (line_1_x_1 == line_2_x_1 && ((line_1_y_1 >= line_2_y_1 && line_1_y_1 <= line_2_y_2) || (line_1_y_2 >= line_2_y_1 && line_1_y_2 <= line_2_y_2)))
                    {
                        // Serial.print(F("Y Match Min: "));
                        // Serial.print(max(line_1_y_1, line_2_y_1));
                        // Serial.print(F(" max: "));
                        // Serial.print(min(line_1_y_2, line_2_y_2));
                        // Serial.print(F(" x: "));
                        // Serial.println(line_1_x_1);

                        uint16_t minimum = max(line_1_y_1, line_2_y_1);
                        uint16_t start = minimum - line_1_y_1;
                        uint16_t maximum = min(line_1_y_2, line_2_y_2) - minimum + start;

                        for (size_t i = start; i <= maximum; i++)
                        {
                            set_flag(flags, i >> 3, _BV(i % 8));                          
                        }
                        

                        // Incrementing by the cross over length
                    }
                }
            }
            else if (line_1_direction == horizontal) // line_1_index is horizontal, line_2_index is verticle. Verifying they cross
            {
                if (line_2_x_1 >= line_1_x_1 && line_2_x_1 <= line_1_x_2 && line_1_y_1 >= line_2_y_1 && line_1_y_1 <= line_2_y_2)
                {
                    uint16_t difference = line_1_x_2 - line_1_x_1;
                    set_flag(flags, difference >> 3, _BV(difference % 8));
                }
            }
            else  // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
            {
                if (line_1_x_1 >= line_2_x_1 && line_1_x_1 <= line_2_x_2 && line_2_y_1 >= line_1_y_1 && line_2_y_1 <= line_1_y_2)
                {
                    uint16_t difference = line_1_y_2 - line_2_y_1;
                    set_flag(flags, difference >> 3, _BV(difference % 8));
                    // add(line_2_x_1, line_1_y_1, output);
                }
            }
        }
        
        // Comparing line_1_index to all upcoming values
        for (uint16_t line_2_index = line_1_index; line_2_index < upper; line_2_index += data_size)
        {
            // Reading the 4 16 bit values
            uint16_t line_2_x_1 = pgm_read_word_near(line_2_index);
            uint16_t line_2_y_1 = pgm_read_word_near(line_2_index += data_size);
            uint16_t line_2_x_2 = pgm_read_word_near(line_2_index += data_size);
            uint16_t line_2_y_2 = pgm_read_word_near(line_2_index += data_size);

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

            
            // Serial.print(F("line_2_x_1: "));
            // Serial.print(line_2_x_1);
            // Serial.print(F(" line_2_y_1: "));
            // Serial.print(line_2_y_1);
            // Serial.print(F(" line_2_x_2: "));
            // Serial.print(line_2_x_2);
            // Serial.print(F(" line_2_y_2: "));
            // Serial.println(line_2_y_2);

            // Getting the line orientation
            line_direction line_2_direction = line_2_x_1 == line_2_x_2 ? verticle : horizontal;

            // Line1/2 Orientation are the same
            if (line_1_direction == line_2_direction)
            {
                if (line_1_direction == horizontal) // Comparing horizontal lines, checking they're on the same y value and overlap on x values
                {
                    if (line_1_y_1 == line_2_y_1 && ((line_1_x_1 >= line_2_x_1 && line_1_x_1 <= line_2_x_2) || (line_1_x_2 >= line_2_x_1 && line_1_x_2 <= line_2_x_2)))
                    {
                        // Serial.println(F("Inline Horizontal"));

                        // Serial.print(F("X Match Min: "));
                        // Serial.print(max(line_1_x_1, line_2_x_1));
                        // Serial.print(F(" max: "));
                        // Serial.print(min(line_1_x_2, line_2_x_2));
                        // Serial.print(F(" y: "));
                        // Serial.println(line_1_y_1);

                        // Incrementing by the cross over length

                        uint16_t minimum = max(line_1_x_1, line_2_x_1);
                        uint16_t start = minimum - line_1_x_1;
                        uint16_t maximum = (min(line_1_x_2, line_2_x_2) - minimum) + start;

                        for (size_t i = start; i <= maximum; i++)
                        {
                            if (set_flag(flags, i >> 3, _BV(i % 8))) {
                                in_h++;
                                result++;
                            };                          
                        }
                    }
                }
                else  // Comparing verticle lines, checking they're on the same x value and overlap on y values
                {
                    if (line_1_x_1 == line_2_x_1 && ((line_1_y_1 >= line_2_y_1 && line_1_y_1 <= line_2_y_2) || (line_1_y_2 >= line_2_y_1 && line_1_y_2 <= line_2_y_2)))
                    {
                        // Serial.print(F("Y Match Min: "));
                        // Serial.print(max(line_1_y_1, line_2_y_1));
                        // Serial.print(F(" max: "));
                        // Serial.print(min(line_1_y_2, line_2_y_2));
                        // Serial.print(F(" x: "));
                        // Serial.println(line_1_x_1);

                        uint16_t minimum = max(line_1_y_1, line_2_y_1);
                        uint16_t start = minimum - line_1_y_1;
                        uint16_t maximum = min(line_1_y_2, line_2_y_2) - minimum + start;

                        for (size_t i = start; i <= maximum; i++)
                        {
                            
                            if (set_flag(flags, i >> 3, _BV(i % 8))) {
                                in_v++;
                                result++;
                            };                           
                        }
                        

                        // Incrementing by the cross over length
                        
                    }
                }
            }
            else if (line_1_direction == horizontal) // line_1_index is horizontal, line_2_index is verticle. Verifying they cross
            {
                if (line_2_x_1 >= line_1_x_1 && line_2_x_1 <= line_1_x_2 && line_1_y_1 >= line_2_y_1 && line_1_y_1 <= line_2_y_2)
                {
                    uint16_t difference = line_1_x_2 - line_1_x_1;
                    if (set_flag(flags, difference >> 3, _BV(difference % 8))) {
                        x_h++;
                        result++;
                    }                    
                }
            }
            else  // line_1_index is verticle, line_2_index is horizontal. Verifying they cross
            {
                if (line_1_x_1 >= line_2_x_1 && line_1_x_1 <= line_2_x_2 && line_2_y_1 >= line_1_y_1 && line_2_y_1 <= line_1_y_2)
                {
                    uint16_t difference = line_1_y_2 - line_2_y_1;
                    if (set_flag(flags, difference >> 3, _BV(difference % 8))) {
                        x_v++;
                        result++;
                    }

                    // add(line_2_x_1, line_1_y_1, output);
                }
            }
        }
    }

    Serial.print(F("in_h "));
    Serial.print(in_h);
    Serial.print(F(" in_v: "));
    Serial.print(in_v);
    Serial.print(F(" x_h: "));
    Serial.print(x_h);
    Serial.print(F(" x_v: "));
    Serial.print(x_v);
    Serial.print(F(" result: "));
    Serial.println(result);
    
    return result;
}

void setup()
{
    Serial.begin(115200);

    // Waiting for the serial to start
    while (!Serial) {}

    // Small delay because I was having Serial Print issues
    delay(1000);

    uint16_t startTime;
    uint16_t endTime;
    uint16_t duration;
    uint16_t value = 0;

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

    // Serial.println(F("Beginning Day 3 - Part 2"));

    // startTime = micros();
    // value = process_report_detailed_branchless(DIAGNOSTIC_INPUT, sizeof(DIAGNOSTIC_INPUT), BIT_SIZE);
    // endTime = micros();
    // duration = endTime - startTime;
    
    // Serial.print(F("process_report_detailed_branchless time: "));
    // Serial.print(duration);
    // Serial.println(F(" μs"));
    // Serial.print(F("process_report_detailed_branchless value: "));
    // Serial.println(value);
}

void loop() { }