/*
 Name:		day1.ino
 Created:	12/1/2021 7:34:55 PM
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

#define COUNTER_SIZE 4

typedef struct instance
{
    uint16_t total;
    uint8_t size;
    instance *head;

    uint8_t increment(uint16_t value)
    {
        if (size > 0) {
            total += value;
            size--;
        }
        
        if (head != NULL)
        {
            if (size == 0) {
                uint8_t is_deeper = head->total < total;
                delete head;
                return is_deeper;
            };
            return head->increment(value);
        }
        return 0;            
    }

    instance(instance *head, uint8_t size) : head(head), size(size)
    { 
        total = 0;
    }
} instance_t;


/**
 * Runs through the scans and counts how many scans in sequence are deeper than the previous
 * @param input A pointer to our array scans
 * @param size The length of the scans
 * @return A count of the number of depth increases
 */
uint16_t depth_scan(const uint16_t input[], uint16_t size)
{
    uint8_t step_size = sizeof(uint16_t);
    uint16_t count = 0;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + (size - step_size);
    
    uint16_t current = pgm_read_word_near(upper);

    for (uint16_t i = upper - step_size; i >= lower; i-=step_size)
    {
        uint16_t previous = pgm_read_word_near(i);
        count += (previous < current);
        current = previous;
    }
    return count;
}

/**
 * Runs through the scans counts up a sequence of scans.
 * Once a sum upto the scan count is complete, it will be compared to the previous sum.
 * @param input A pointer to our array scans
 * @param size The length of the scans
 * @param scans_count The number of scans to add up
 * @return A count of the number of depth increases
 */
uint16_t depth_scan_sum(const uint16_t input[], uint16_t size, uint8_t scans_count)
{
    uint8_t step_size = sizeof(uint16_t);
    uint16_t count = 0;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + size;
    
    instance_t *current = NULL;

    for (uint16_t i = lower; i < upper; i+=step_size)
    {
        uint16_t value = pgm_read_word_near(i);
        current = new instance_t(current, scans_count);
        count += current->increment(value);
    }
    return count;
}

void setup()
{
    Serial.begin(115200);

    // Waiting for the serial to start
    while (!Serial) {}

    // Small delay because I was having Serial Print issues
    delay(500);

    uint16_t startTime;
    uint16_t endTime;
    uint16_t duration;
    uint16_t count = 0;

    Serial.println(F("Beginning Day 1 - Part 1"));

    startTime = micros();
    count = depth_scan(DEPTH_INPUT, sizeof(DEPTH_INPUT));
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("depth_scan time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("depth_scan value: "));
    Serial.println(count);

    Serial.println(F("Beginning Day 1 - Part 2"));

    startTime = micros();
    count = depth_scan_sum(DEPTH_INPUT, sizeof(DEPTH_INPUT), 3);
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("depth_scan_sum time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("depth_scan_sum value: "));
    Serial.println(count);
}

void loop() { }