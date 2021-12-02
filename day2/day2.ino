/*
 Name:		day2.ino
 Created:	12/2/2021 6:47:33 PM
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

/**
 * Runs through an array of commands and keeps count of the movements in both the horizontal, and depth directions.
 * @param input A pointer to our array scans
 * @param size The length of the scans
 * @return The depth * horizontal movement
 */
uint32_t dive_controls(const uint8_t input[], uint16_t size)
{
    uint8_t step_size = sizeof(uint8_t) + sizeof(uint8_t); // Stepping the size of an enum and 8bit command

    uint16_t depth = 0;
    uint16_t distance = 0;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + size;
    
    uint16_t current = pgm_read_word_near(upper);

    for (uint16_t i = lower; i < upper; i += step_size)
    {
        uint8_t command = pgm_read_byte_near(i);
    
        switch (command)
        {
        case forward:
            distance += pgm_read_byte_near(i + 1);
            break;
        case down:
            depth += pgm_read_byte_near(i + 1);
            break; 
        case up:
            depth -= pgm_read_byte_near(i + 1);
            break;       
        default:
            break;
        }
    }

    return (uint32_t)distance * depth;
}

/**
 * Runs through an array of commands.
 * up/down commands adjust the aim, while horizontal commands adjust
 * the horizontal position in addition to adjusting the depth by the aim * horizontal movement.
 * @param input A pointer to our array scans
 * @param size The length of the scans
 * @return The depth * horizontal movement
 */
uint32_t dive_controls_with_aim(const uint8_t input[], uint16_t size)
{
    uint8_t step_size = sizeof(uint8_t) + sizeof(uint8_t); // Stepping the size of an enum and 8bit command

    uint32_t depth = 0;
    uint16_t distance = 0;
    int16_t aim = 0;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + size;
    
    uint16_t current = pgm_read_word_near(upper);

    for (uint16_t i = lower; i < upper; i += step_size)
    {
        uint8_t command = pgm_read_byte_near(i);
        int8_t value = pgm_read_byte_near(i + 1);
        switch (command)
        {
        case forward:
            distance += value;
            depth += (value * aim);
            break;
        case down:            
            aim += value;
            break; 
        case up:
            aim -= value;            
            break;       
        default:
            break;
        }
    }

    return depth * distance;
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
    uint32_t value = 0;

    Serial.println(F("Beginning Day 2 - Part 1"));

    startTime = micros();
    value = dive_controls(COMMANDS_INPUT, sizeof(COMMANDS_INPUT));
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("dive_controls time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("dive_controls value: "));
    Serial.println(value);

    Serial.println(F("Beginning Day 2 - Part 2"));

    startTime = micros();
    value = dive_controls_with_aim(COMMANDS_INPUT, sizeof(COMMANDS_INPUT));
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("dive_controls_with_aim time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("dive_controls_with_aim value: "));
    Serial.println(value);
}

void loop() { }