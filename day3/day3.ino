/*
 Name:		day3.ino
 Created:	12/3/2021 8:42:20 PM
 Author:	Tosso
*/

#include "input.h"
#include <avr/pgmspace.h>

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
uint32_t process_report(const uint16_t input[], uint16_t size, uint8_t bit_size)
{
    uint8_t step_size = sizeof(uint16_t); // Stepping the size of an enum and 8bit command
    uint16_t half_input_count = (size / step_size) >> 1;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + size;
    uint16_t counter[bit_size];

    memset(counter, 0, sizeof(counter));

    for (uint16_t i = lower; i < upper; i += step_size)
    {
        uint16_t input = pgm_read_word_near(i);
        uint8_t position = 0;
        uint16_t tester;

        while (((tester = _BV(position)) <= input))
        {

            counter[position++] += (input & tester) > 0;
        }
    }

    uint16_t result = 0;

    int8_t position = -1;
    uint16_t tester;

    while (counter[++position]) if (counter[position] > half_input_count) result |= _BV(position);

    // To get te inverse I shift a 1 past the size of the result, and take off 1 to get 1's in the positions I need for the mask
    return (uint32_t)result * (result ^ ((1 << position) - 1));
}

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
uint32_t process_report_detailed_branchless(const uint16_t input[], uint16_t size, uint8_t bit_size)
{
    uint8_t step_size = sizeof(uint16_t);

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + size;
    
    uint16_t oxygen_filter = 0;
    uint16_t co2_filter = 0;

    uint16_t position = bit_size - 1;

    do
    {
        int16_t oxygen_counter = 0;
        uint16_t oxygen_total_counter = 0;
        int16_t c02_counter = 0;
        uint16_t c02_total_counter = 0;

        // Shifting our ready for the next bit
        oxygen_filter = oxygen_filter << 1;
        co2_filter = co2_filter << 1;

        for (uint16_t i = lower; i < upper; i += step_size)
        {
            // Shifting the input right ignoring bits we don't care about
            uint16_t input = pgm_read_word_near(i) >> position;
            // The value of the LSB of the input
            uint8_t value = (input & 1);
            // Whether the input matches our patterns (<=1 is used because we ignore the LSB)
            uint8_t is_oxygen = ((input ^ oxygen_filter) <= 1);
            uint8_t is_c02 = ((input ^ co2_filter) <= 1);

            // Increment our counters
            oxygen_total_counter += is_oxygen;
            c02_total_counter += is_c02;

            oxygen_counter += is_oxygen && value;
            c02_counter += is_c02 && value;
        }

        // setting the bit in the filter based off the count if there were more than 1 value, if not set it to the last value
        oxygen_filter |= ((oxygen_total_counter > 1 && (oxygen_total_counter - oxygen_counter) <= oxygen_total_counter >> 1) || (oxygen_total_counter == 1 && oxygen_counter > 0));
        co2_filter |= ((c02_total_counter > 1 && (c02_total_counter - c02_counter) > c02_total_counter >> 1) || (c02_total_counter == 1 && c02_counter > 0));

    } while (position-- > 0);


    return (uint32_t)oxygen_filter * co2_filter;
}

/**
 * Given the input of a list of values, this function will loop through while filtering out values
 * that don't match our pattern. These patterns are generated by counting the occurance of 1's in that position.
 * Two patterns are used, one where 1's are more common, and 1 where 0's are more common.
 * If only 1 value remains then the count is ignored and that value is chosen for that bit position instead.
 * At the end the two patterns will match a single value in the array, and we return these values multiple together.
 * This function uses branches as a comparison to branchless
 * @param input A pointer to our array of values we're processing
 * @param size The length of the array
 * @param bit_size the number of bits in the inputs
 * @return The two patterns multiplied together.
 */
uint32_t process_report_detailed_branched(const uint16_t input[], uint16_t size, uint8_t bit_size)
{
    uint8_t step_size = sizeof(uint16_t); // Stepping the size of an enum and 8bit command
    uint16_t half_input_count = (size / step_size) >> 1;

    uint16_t lower = (uint16_t)input;
    uint16_t upper = lower + size;
    

    uint16_t oxygen_filter = 0;
    uint16_t co2_filter = 0;

    uint16_t position = bit_size - 1;

    do
    {
        int16_t oxygen_counter = 0;
        uint16_t oxygen_total_counter = 0;
        int16_t c02_counter = 0;
        uint16_t c02_total_counter = 0;

        // Shifting our ready for the next bit
        oxygen_filter = oxygen_filter << 1;
        co2_filter = co2_filter << 1;

        for (uint16_t i = lower; i < upper; i += step_size)
        {
            // Shifting the input right ignoring bits we don't care about
            uint16_t input = pgm_read_word_near(i) >> position;
            // The value of the LSB of the input
            uint8_t value = (input & 1);

            // Whether the input matches our patterns (<=1 is used because we ignore the LSB)
            if ((input ^ oxygen_filter) <= 1)
            {
                oxygen_total_counter++;
                oxygen_counter += value;
            }

            if ((input ^ co2_filter) <= 1)
            {
                c02_total_counter++;
                c02_counter += value;
            }
        }

        // setting the bit in the filter based off the count if there were more than 1 value, if not set it to the last value
        if ((oxygen_total_counter > 1 && (oxygen_total_counter - oxygen_counter) <= oxygen_total_counter >> 1) || (oxygen_total_counter == 1 && oxygen_counter > 0))
        {
            oxygen_filter++;
        }

        if ((c02_total_counter > 1 && (c02_total_counter - c02_counter) > c02_total_counter >> 1) || (c02_total_counter == 1 && c02_counter > 0))
        {
            co2_filter++;
        }
    } while (position-- > 0);


    return (uint32_t)oxygen_filter * co2_filter;
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

    Serial.println(F("Beginning Day 3 - Part 1"));

    startTime = micros();
    value = process_report(DIAGNOSTIC_INPUT, sizeof(DIAGNOSTIC_INPUT), BIT_SIZE);
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("process_report time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("process_report value: "));
    Serial.println(value);

    Serial.println(F("Beginning Day 3 - Part 2"));

    startTime = micros();
    value = process_report_detailed_branchless(DIAGNOSTIC_INPUT, sizeof(DIAGNOSTIC_INPUT), BIT_SIZE);
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("process_report_detailed_branchless time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("process_report_detailed_branchless value: "));
    Serial.println(value);

    startTime = micros();
    value = process_report_detailed_branched(DIAGNOSTIC_INPUT, sizeof(DIAGNOSTIC_INPUT), BIT_SIZE);
    endTime = micros();
    duration = endTime - startTime;
    
    Serial.print(F("process_report_detailed_branched time: "));
    Serial.print(duration);
    Serial.println(F(" μs"));
    Serial.print(F("process_report_detailed_branched value: "));
    Serial.println(value);
}

void loop() { }