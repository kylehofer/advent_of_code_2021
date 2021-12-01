/*
 Name:		day1.ino
 Created:	12/1/2021 7:34:55 PM
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

/**
 * Runs through the scans and counts how many scans in sequence are deeper than the previous
 * @param input A pointer to our array scans
 * @param size The length of the scans
 * @return An integer count of the number of depth increases
 */
uint16_t depth_scan(const uint16_t input[], uint16_t size)
{
    Serial.println("depth_scan");
    uint8_t step_size = sizeof(uint16_t);
    uint16_t count = 0;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + (size - step_size);
    
    uint16_t current = pgm_read_word_near(upper);

    for (uint16_t i = upper - step_size; i >= lower; i-=step_size)
    {
        uint16_t previous = pgm_read_word_near(i);
        count += 1 * (previous < current);
        current = previous;
    }
    return count;
}

void setup()
{
    Serial.begin(115200);

    // Waiting for the serial to start
    while (!Serial) {}

    uint16_t startTime;
    uint16_t endTime;
    uint16_t duration;
    uint16_t count = 0;

    Serial.println("Beginning Day 1 - Part 1");

    startTime = micros();
    count = depth_scan(DEPTH_INPUT, sizeof(DEPTH_INPUT));
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print("depth_scan time: ");
    Serial.print(duration);
    Serial.println(" μs");
    Serial.print("depth_scan value: ");
    Serial.println(count);
}

void loop() { }