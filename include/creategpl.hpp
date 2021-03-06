/*
Copyright (C) 2012	Emanuele De Donatis

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef CREATEGPL_HPP
#define CREATEGPL_HPP

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <cstring>

#define N_TRACES_TO_PLOT 10

using namespace std;

/*
 * Header file informations
 */
struct RawHeader {
    uint32_t numtraces;
    uint32_t numsamples_per_trace;
    char datatype;
    uint8_t knowndatalength;
};
RawHeader raw;

/*
 * Dimension of traces
 */
struct Dimensions {
    int n_traces;
    int n_samples;
    size_t sample;
    size_t knowndata;
    size_t header;
    size_t trace;
    size_t file;
};
Dimensions dim;

/*
 * Input parameters
 */
char *inputFile, *tracesFile, *meanFile, *gplFile;
int n_traces = 0, x_min = 0, x_max = 0;

/*
 * I/O files
 */
ifstream fin;
ofstream traces_dat, mean_dat, gpl;

/*
 * Methods
 */
void parseParams(int argc, char** argv);
void openFiles();
void closeFiles();
void parseInputFile();
void createDat();
void createGpl();

#endif /* CREATEGPL_HPP */

