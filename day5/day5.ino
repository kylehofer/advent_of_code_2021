/*
 Name:		day3.ino
 Created:	12/3/2021 8:42:20 PM
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

#define SERIAL_BUFFER_SIZE 8


typedef struct point
{
    uint16_t x;
    uint16_t y;
    point *tail;

    point() : x(0xFFFF), y(0xFFFF), tail(NULL) {}
    point(uint16_t x, uint16_t y) : x(x), y(y), tail(NULL) { }
} point_t;



enum line_direction {
    verticle, horizontal
};


void add(uint16_t x, uint16_t y, point *item)
{
    static uint16_t total = 0;
    while ((item->x != x || item->y != y) && item->tail)
    {
        item = item->tail;
    }

    if ((item->x != x || item->y != y) && !item->tail) {
        total++;
        Serial.println(total);
        item->tail = new point_t(x, y);
    }
}

uint16_t calculate(point *item)
{
    uint16_t result = 0;
    while ((item = item->tail) != NULL) {result++;}
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
uint16_t process_report(const uint16_t input[], const uint16_t size)
{
    const uint8_t data_size = sizeof(uint16_t);

    const uint8_t step_size = data_size << 2; // Stepping the size of an enum and 8bit command

    const uint16_t lower = (uint16_t)input;
    const uint16_t upper = lower + (size);

    uint16_t buffer[2000];

    memset(buffer, 0, 2000);

    for (size_t i = 0; i < 2000; i++)
    {
        buffer[i];
    }
    

    return;

    point_t *counter = new point_t();

    for (uint16_t l1 = lower; l1 < upper; ) //l1 += step_size
    {
        uint16_t l1x1 = pgm_read_word_near(l1);
        uint16_t l1y1 = pgm_read_word_near(l1 += data_size);
        uint16_t l1x2 = pgm_read_word_near(l1 += data_size);
        uint16_t l1y2 = pgm_read_word_near(l1 += data_size);

        l1 += data_size;

        if ((l1x1 != l1x2) && (l1y1 != l1y2)) continue;

        if (l1x1 > l1x2)
        {
            l1x1 ^= l1x2;
            l1x2 ^= l1x1;
            l1x1 ^= l1x2;
        }

        if (l1y1 > l1y2)
        {
            l1y1 ^= l1y2;
            l1y2 ^= l1y1;
            l1y1 ^= l1y2;
        }

        // Serial.print(F("l1x1: "));
        // Serial.print(l1x1);
        // Serial.print(F(" l1y1: "));
        // Serial.print(l1y1);
        // Serial.print(F(" l1x2: "));
        // Serial.print(l1x2);
        // Serial.print(F(" l1y2: "));
        // Serial.println(l1y2);

        line_direction l1d = l1x1 == l1x2 ? verticle : horizontal;

        
        for (uint16_t l2 = l1; l2 < upper; l2 += data_size)
        {
            
            uint16_t l2x1 = pgm_read_word_near(l2);
            uint16_t l2y1 = pgm_read_word_near(l2 += data_size);
            uint16_t l2x2 = pgm_read_word_near(l2 += data_size);
            uint16_t l2y2 = pgm_read_word_near(l2 += data_size);

            if ((l2x1 != l2x2) && (l2y1 != l2y2)) continue;

            if (l2x1 > l2x2)
            {
                l2x1 ^= l2x2;
                l2x2 ^= l2x1;
                l2x1 ^= l2x2;
            }

            if (l2y1 > l2y2)
            {
                l2y1 ^= l2y2;
                l2y2 ^= l2y1;
                l2y1 ^= l2y2;
            }

            
            // Serial.print(F("l2x1: "));
            // Serial.print(l2x1);
            // Serial.print(F(" l2y1: "));
            // Serial.print(l2y1);
            // Serial.print(F(" l2x2: "));
            // Serial.print(l2x2);
            // Serial.print(F(" l2y2: "));
            // Serial.println(l2y2);

            line_direction l2d = l2x1 == l2x2 ? verticle : horizontal;

            if (l1d == l2d)
            {
                if (l1d == horizontal)
                {
                    if (l1y1 == l2y1 && ((l1x1 >= l2x1 && l1x1 <= l2x2) || (l1x2 >= l2x1 && l1x2 <= l2x2)))
                    {
                        uint16_t point_max = min(l1x2, l2x2);

                        // Serial.print(F("X Match Min: "));
                        // Serial.print(max(l1x1, l2x1));
                        // Serial.print(F(" max: "));
                        // Serial.print(point_max);
                        // Serial.print(F(" y: "));
                        // Serial.println(l1y1);
                        for (uint16_t i = max(l1x1, l2x1); i <= point_max; i++)
                        {
                            
                            add(i, l1y1, counter);
                        }                        
                    }
                }
                else
                {
                    if (l1x1 == l2x1 && ((l1y1 >= l2y1 && l1y1 <= l2y2) || (l1y2 >= l2y1 && l1y2 <= l2y2)))
                    {
                        uint16_t point_max = min(l1y2, l2y2);

                        // Serial.print(F("Y Match Min: "));
                        // Serial.print(max(l1y1, l2y1));
                        // Serial.print(F(" max: "));
                        // Serial.print(point_max);
                        // Serial.print(F(" x: "));
                        // Serial.println(l1x1);
                       
                        for (uint16_t i = max(l1y1, l2y1); i <= point_max; i++)
                        {
                            
                            add(l1x1, i, counter);
                        }   
                    }
                }
                continue;
            }
            else if (l1d == horizontal)
            {
                if (l2x1 >= l1x1 && l2x1 <= l1x2 && l1y1 >= l2y1 && l1y1 <= l2y2)
                {
                    add(l1x1, l2y1, counter);
                }
            }
            else
            {
                if (l1x1 >= l2x1 && l1x1 <= l2x2 && l2y1 >= l1y1 && l2y1 <= l1y2)
                {
                    add(l2x1, l1y1, counter);
                }
            }
        }
    }
    
    return calculate(counter);
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