# createGPL
Create gnuplot ".gpl" file from ".dat" binary file with header section.

This program is developed to plot the input file of [maxximino/dpacalc](https://github.com/maxximino/dpacalc)

## Input header file format
<uint32_t> 	number of trace
<uint32_t> 	number of samples per trace
<char> 			data type (f - float || d - double)
<uint8_t> 	known data length per trace (DPA purpose)

## Usage
./createGPL 
-in 	<file_path>		.dat input file
-out 	<file_path>		.dat output file
-gpl 	<file_path>		.gpl output file
-n 		<int> 				number of traces to compute
-min 	<int>					min value for x (sample) range 
-max 	<int>					max value for x (sample) range
