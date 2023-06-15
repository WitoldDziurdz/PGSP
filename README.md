# Parallel GSP Engines

This is a parallel implementation of the Generalized Sequential Pattern (GSP) algorithm. This program allows you to run different types of GSP engines (Classic, SPSPM, and Hash engines) on both CPU and GPU to find frequent sequences in a given dataset.

## Building

This project uses C++20 and requires a compiler that supports this version of the language. The PDC++ compiler is required to compile SYCL. A tutorial on how to install and compile a SYCL project can be found [here](URL_to_your_tutorial).

## Usage

You can run the program with the following command:

```bash
./cuda [options]

--info : Enable info logs.
--debug : Enable debug logs.
--print : Print the result to the console.
--file : Write the result to a file.
--min <value> : Set the minimum support value. It should be a float number between 0 and 1. The default is 0.01.
--data <file_name> : Read data from a file in the ./input/ directory. If this option is not used, the program will use a simple default dataset.
--gsp : Use the classic GSP engine.
--sgsp_cpu : Use the SPSPM engine on the CPU.
--hgsp_cpu : Use the Hash engine on the CPU.
--sgsp_gpu : Use the SPSPM engine on the GPU.
--hgsp_gpu : Use the Hash engine on the GPU.
--all : Use all engines. This is the default option if no engine is specified.
```
##  Example
Here's an example of how to use the program:

```bash
./cuda --min 0.1 --data mydata.txt --gsp --print
```
This command will run the classic GSP engine with a minimum support value of 0.1, using the data in the mydata.txt file, and print the result to the console.

## Contact
For any issues or suggestions, please contact the author.
