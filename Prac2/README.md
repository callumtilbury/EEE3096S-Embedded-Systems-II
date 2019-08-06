# EEE3096S Practical 2
## Benchmarking Assistance

These files are adapted from [this practical](https://github.com/kcranky/EEE3096S/tree/master/Prac2) as a part of the Embedded Systems II course at UCT. They aim to assist in the benchmarking process required for the second practical, by adding csv output, and some loop functionality.

## Setup
In order to establish the algorithm's accuracy for various implementations, you'll need to create a _correct_ dataset to compare with.
You can easily do this by running "Python/Prac2_WriteGoldenMeasureData.py" file once.

## To benchmark the unthreaded loop:
- Navigate to the ./C/ directory
- Run:

```shell
$ bash loop "Message"
```

where "Message" will be included in the header of the output csv chunk. This can help with data synthesis later on.

- The above command will output data to a file Prac2/log.csv, with the columns:

`<Bitwidth of Data>, <Time-to-completion>, <Epsilon Value>, <# of Inaccuracies for given Epsilon>`

## To run the threaded loop:

- Navigate to the ./C/ directory
- Run:
   
```shell
$ bash loop_threaded "Message"
```
   
- This will output data to to the same file, Prac2/log.csv, but this time with the columns:

`<# of Threads>, <Time-to-completion>, <Epsilon Value>, <# of Inaccuracies for given Epsilon>`
