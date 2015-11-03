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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <cstring>

using namespace std;

struct RawHeader {
    uint32_t numtraces;
    uint32_t numsamples_per_trace;
    char datatype;
    uint8_t knowndatalength;
};

struct Dimensions {
    int n_traces;
    int n_samples;
    size_t sample;
    size_t knowndata;
    size_t header;
    size_t trace;
    size_t file;
};

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc < 5) { 
        std::cout << "usage: " << argv[0] << " -in <input_dat> -out <output_dat> -gpl <output_gpl> -n <number_of_traces> -min <x_min> -max <x_max>" << endl;
        exit(0);
    }
    
    cout << endl;
    char *inputFile, *outputFile, *gplFile;
    int n_traces = 0, x_min = 0, x_max = 0;
    for (int i = 1; i < argc; i = i+2) {
        if (i + 1 != argc){
            if (strncmp (argv[i],"-in",3) == 0) {
                inputFile = argv[i + 1];
                cout << "Input file: " << inputFile << endl;
            } else if (strncmp (argv[i],"-out",3) == 0) {
                outputFile = argv[i + 1];
                cout << "Output file: " << outputFile << endl;
            } else if (strncmp (argv[i],"-gpl",3) == 0) {
                gplFile = argv[i + 1];
                cout << "Gpl file: " << gplFile << endl;
            } else if (strncmp (argv[i],"-n",2) == 0) {
                n_traces = atoi(argv[i + 1]);
                cout << "Number of traces: " << n_traces << endl;
            } else if (strncmp (argv[i],"-min",4) == 0) {
                x_min = atoi(argv[i + 1]);
                cout << "From sample: " << x_min << endl;
            } else if (strncmp (argv[i],"-max",4) == 0) {
                x_max = atoi(argv[i + 1]);
                cout << "To sample: " << x_max << endl;
            } else {
                std::cout << "Not enough or invalid arguments, please try again.\n";
                exit(0);
            }
        }
    }
    
    cout << endl;
    
    ifstream fin(inputFile);
    ofstream fout(outputFile);
    ofstream gpl (gplFile);
    
    if(fin && fout && gpl) {
        cout << endl << "Parsing input file ..." << endl;
    
        RawHeader raw;
        Dimensions dim;
        fin.seekg(0, fin.beg);
        
        fin.read((char*)&raw.numtraces, sizeof(raw.numtraces));
        dim.n_traces = raw.numtraces;
        
        fin.read((char*)&raw.numsamples_per_trace, sizeof(raw.numsamples_per_trace));
        dim.n_samples = raw.numsamples_per_trace;
        
        fin.read((char*)&raw.datatype, sizeof(raw.datatype));
        switch ( raw.datatype ) {
            case 'f':   dim.sample = 4; break;
            case 'd':   dim.sample = 8; break;
            default:    cerr << "Sample data type should be f <float> or d <double> " << endl; return -1;
	}
        
        fin.read((char*)&raw.knowndatalength, sizeof(raw.knowndatalength));
        dim.knowndata = (int) raw.knowndatalength;
        
        cout << "Number of traces: " << dim.n_traces << endl;
        cout << "Number of samples per trace: " << dim.n_samples << endl;
        cout << "Sample size: " << dim.sample << " Bytes" << endl;
        cout << "Known data size: " << dim.knowndata << " Bytes" << endl;

        dim.header = sizeof(raw.numtraces) + sizeof(raw.numsamples_per_trace) + sizeof(raw.datatype) + sizeof(raw.knowndatalength);
        cout << "Header size: " << dim.header << " Bytes" << endl;
        
        dim.trace = dim.n_samples * dim.sample;
        cout << "Trace size: " << dim.trace << " Bytes" << endl;
        
        dim.file = dim.header + dim.n_traces * (dim.knowndata + dim.trace);
        cout << "File size: " << dim.file << " Bytes" << endl;

        if(n_traces == 0 || n_traces > dim.n_traces)
            n_traces = dim.n_traces;
        if(x_max == 0 || x_max > dim.n_samples) 
            x_max = dim.n_samples;
        if(x_min >= x_max) {
            cerr << "Sample rate must be consistent" <<endl; 
            return -1;
        }
        
        cout << endl << "Creating output file ..." << endl;

        for(int isample = x_min; isample < x_max; isample++) {
            fout << isample;
            for(int itrace = 0; itrace < n_traces; itrace++) {
                fin.seekg (dim.header + (itrace + 1)*dim.knowndata + (itrace)*dim.trace + isample * dim.sample, fin.beg);
                switch ( raw.datatype ) {
                    case 'f':   float fsample; 
                                fin.read((char*)&fsample, dim.sample);
                                fout << " " << fsample;
                                break;
                    case 'd':   double dsample; 
                                fin.read((char*)&dsample, dim.sample);
                                fout << " " << dsample;
                                break;
                }
            }
            fout << endl;
        }
        
        cout << endl << "Creating gpl file ..." << endl;
        
        gpl << "set term png size 3000,1500 crop;" << endl;
        gpl << "set output \"power_traces.png\";" << endl;
        gpl << "set autoscale;" << endl;
        gpl << "set xtic auto;" << endl;
        gpl << "set ytic auto;" << endl;
        gpl << "set xrange [" << x_min << ":" << x_max << "];" << endl;
        gpl << "set key off;" << endl;
        gpl << "set title \"Power Traces\";" << endl;
        gpl << "set xlabel \"time\";" << endl;
        gpl << "set ylabel \"Power consumption\";" << endl << endl;

        gpl << "plot  "; 
        for(int itrace = 0; itrace < n_traces; itrace++) {
            if(itrace != 0){
                gpl << ", " ;
            }
            gpl << "\""<< outputFile << "\" ";
            gpl << "u 1:" << itrace+2 << " ";
            gpl << "t \"" << itrace+1 << "\" ";
            gpl << "with lines";
        }
        

        fin.close();
        fout.close();
        gpl.close();
        
        cout << endl << "[Finished]" << endl << endl;
    }
    return 0;
}

