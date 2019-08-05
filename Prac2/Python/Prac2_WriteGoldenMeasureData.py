#!/usr/bin/python3
"""
Python Practical 2 Code for storing a csv file of Golden Measure data.
Keegan Crankshaw (Adapted by Callum Tilbury)
EEE3096S Prac 2 2019
Date: 5 August 2019

"""

# import Relevant Librares
import Timing
from data import carrier, data

# Define values.
c = carrier *30
d = data *30
result = []

# Main function
def main():
    f = open("../GoldenMeasureData.csv","w+") # Overwrite old file to ensure clean data set
    for i in range(len(c)):
        f.write("{},".format(c[i]*d[i]))
    f.write("end")
    f.close()

# Only run the functions if this module is run
if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
    except Exception as e:
        print("Error: {}".format(e))
