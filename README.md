
# Advent of Code 2021

I'll be attempting the 2021 advent of code using an Arduino Uno, while attempting to keep my code performant. The limitations of the Uno are a nice little challenge, while also refreshing up on my C/C++.

## --- Day 1: Sonar Sweep ---

<details>
  <summary>Click to expand!</summary>

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

</details>


## --- Day 2: Dive! ---

<details>
  <summary>Click to expand!</summary>

### Part 1

Pretty simple problem where we need to process an action with a value. As we only had 3 actions I used an enum, which also let me easily keep the code input the same without having to translate any values. I then loop through the input stepping 16 bits per step (8 bit for the command, 8 bit for the value), while keeping a count of the depth and horizontal position. Only issues was chosing the correct datatypes, as the output was larger than an unsigned 16 bit integer.

### Part 2

Part 2 didn't need much extra code, and a lot of the previous code was reused. However datatypes reared their ugly head as making assumptions like the aim staying within an 8bit integer was wrong.

### Output

Beginning Day 2 - Part 1

dive_controls time: 1664 μs

dive_controls value: 1499229

Beginning Day 2 - Part 2

dive_controls_with_aim time: 2412 μs

dive_controls_with_aim value: 1340836560

</details>

## --- Day 3: Binary Diagnostic ---

<details>
  <summary>Click to expand!</summary>

### Part 1

This was a lot more fun! I didn't have too much of an issue with part 1, other than figuring out a way of dynamically handling the length of the bits in the input. While this wasn't important was dropped later, I did like the solution I came up with. The code isn't that fast and I have this feeling there must be more optimized approaches but I didn't think of any for part 1. I also optimized this function by only counting the 1's and then taking the inverse of the result for the alternate value.

### Part 2

This is where the real fun began! I really wanted to avoid over processing and wanted to attempt to be as effecient! I thought about methods of handling values or storing the values that match either filter, however I believe I'd very easily max the ram and instead opted for processing over caching. I instead build a progressive filter that gets applied to each value during each pass of the loop. I also employeed some branchless techniques to speed up the core of the loop, which almost doubled the speed compared to using branches. I beieve there's some optimization I add where instead of bitshifting each value I read from progmem, I could apply a bit mask, as I think I could save a couple cycles total with this approach. I was really happy to see this approach was much faster than Part 1!

### Output

Beginning Day 3 - Part 1

process_report time: 51092 μs

process_report value: 1092896

Beginning Day 3 - Part 2

process_report_detailed_branchless time: 27920 μs

process_report_detailed_branchless value: 4672151

process_report_detailed_branched time: 49212 μs

process_report_detailed_branched value: 4672151

</details>