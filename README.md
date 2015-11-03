# createGPL
Create [gnuplot](http://www.gnuplot.info/) ".gpl" file from ".dat" binary file with header section.

This program is developed to plot the input file of [maxximino/dpacalc](https://github.com/maxximino/dpacalc)

## Input header file format
| Size        	| Description																| 
| :-----------: |:-----------------------------------------:| 
| uint32_t	   	| number of trace 													| 
| uint32_t      | number of samples per trace      					| 
| char 					| data type (f - float or d - double)      	| 
| uint8_t 			| known data length per trace (DPA purpose)	| 


## Usage
| Param | Type			| Desc														| 
| :----:|:---------:|:-------------------------------:| 
| -in		|	file path	| input file .dat									|
| -out	|	file path	| output file .dat								|
| -gpl	|	file path	| output file .gpl								|
| -n		|	int				| number of traces to compute			|
| -min	|	int				| min value for x (sample) range	|
| -max	|	int				| max value for x (sample) range	|
