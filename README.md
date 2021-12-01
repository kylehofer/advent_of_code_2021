# Advent of Code 2021
I'll be attempting the 2021 advent of code using an Arduino Uno, while attempting to keep my code performant. The limitations of the Uno are a nice little challenge, while also refreshing up on my C/C++.


## --- Day 1: Sonar Sweep ---

### Part 1
Used a simple loop to decrement backwards through the array, and using the boolean return as an increment to my counter. The input size is fairly large and quickly filled the entirity of the Arduino's RAM, so I needed to use program memory on Day 1.

### Part 2
My initial thoughts on how I'd handle multiple progressive lists lead me to having rigid/structured counters with special conditions. I wasn't a fan of these approaches and instead I moved over to use a dynamic approach where I generate a progressive linked list using a FIFO order. The adding of values flows through the list, with the head being popped off when a size check can be performed.