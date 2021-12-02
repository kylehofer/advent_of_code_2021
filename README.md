
# Advent of Code 2021

I'll be attempting the 2021 advent of code using an Arduino Uno, while attempting to keep my code performant. The limitations of the Uno are a nice little challenge, while also refreshing up on my C/C++.

  
  

## --- Day 1: Sonar Sweep ---

  

### Part 1

Used a simple loop to decrement backwards through the array, and using the boolean return as an increment to my counter. The input size is fairly large and quickly filled the entirity of the Arduino's RAM, so I needed to use program memory on Day 1.

  

### Part 2

My initial thoughts on how I'd handle multiple progressive lists lead me to having rigid/structured counters with special conditions. I wasn't a fan of these approaches and instead I moved over to use a dynamic approach where I generate a progressive linked list using a FIFO order. The adding of values flows through the list, with the head being popped off when a size check can be performed.

  

#### Update

A workmate informed me about how the comparison can be made a lot easier. As we're trying to see the difference in the sum of sequences of numbers, you can use a trick where you only compare the difference of the sums. For example if we have the sequence, A, B, C, D, and we want to compare the sum of A + B + C, and B + C + D, we only need to compare A and D, as B/C are the same.

  

### Output

Beginning Day 1 - Part 1

depth_scan time: 3096 μs

depth_scan value: 1502

Beginning Day 1 - Part 2

depth_scan_sum time: 34532 μs

depth_scan_sum value: 1538

depth_scan_sum_optimized time: 4308 μs

depth_scan_sum_optimized value: 1538
  

## --- Day 2: Dive! ---  

### Part 1

Pretty simple problem where we need to process an action with a value. As we only had 3 actions I used an enum, which also let me easily keep the code input the same without having to translate any values. I then loop through the input stepping 16 bits per step (8 bit for the command, 8 bit for the value), while keeping a count of the depth and horizontal position. Only issues was chosing the correct datatypes, as the output was larger than an unsigned 16 bit integer.

  

### Part 2

This additionally problem didn't need much extra code, and a lot of the previous code was reused. However datatypes reared their ugly head as making assumptions like the aim staying within an 8bit integer was wrong.

### Output

Beginning Day 2 - Part 1

dive_controls time: 1664 μs

dive_controls value: 1499229

Beginning Day 2 - Part 2

dive_controls_with_aim time: 2412 μs

dive_controls_with_aim value: 1340836560
