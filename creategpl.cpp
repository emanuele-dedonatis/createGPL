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

#include "include/creategpl.hpp"

/**
 * Create [gnuplot](http://www.gnuplot.info/) ".gpl" file from ".dat" binary file with header section.
 * This program is developed to plot the input file of [maxximino/dpacalc](https://github.com/maxximino/dpacalc)
 */
int main(int argc, char** argv) {

    parseParams(argc, argv);
   
    openFiles();
    
    parseInputFile();
        
    createDat();
    createGpl();
        
    closeFiles();
        
    cout << endl << "[FINISHED]" << endl << endl;
    
    return 0;
}

/**
 * Parse the input parameters
 */
void parseParams(int argc, char** argv) {
    if (argc < 5) { 
        cout << "usage: " << argv[0] << " -in <input_dat> -traces <traces_dat> -mean <mean_dat> -gpl <output_gpl> -num <number_of_traces> -min <x_min> -max <x_max>" << endl;
        exit(0);
    }
    
    cout << endl << "Parsing input parameters ... " << flush;
    for (int i = 1; i < argc; i = i+2) {
        if (i + 1 != argc){
            if (strncmp (argv[i],"-in",3) == 0) {
                inputFile = argv[i + 1];
            } else if (strncmp (argv[i],"-traces",7) == 0) {
                tracesFile = argv[i + 1];
            } else if (strncmp (argv[i],"-mean",5) == 0) {
                meanFile = argv[i + 1];
            } else if (strncmp (argv[i],"-gpl",4) == 0) {
                gplFile = argv[i + 1];
            } else if (strncmp (argv[i],"-num",4) == 0) {
                n_traces = atoi(argv[i + 1]);
            } else if (strncmp (argv[i],"-min",4) == 0) {
                x_min = atoi(argv[i + 1]);
            } else if (strncmp (argv[i],"-max",4) == 0) {
                x_max = atoi(argv[i + 1]);
            } else {
                cout << "[ERROR]" << endl << "Not enough or invalid arguments, please try again." << endl;
                exit(0);
            }
        }
    }
    cout << "[OK]" << endl;
    
    cout << "| Input data file: " << inputFile << endl;
    cout << "| Traces data file: " << tracesFile << endl;
    cout << "| Mean data file: " << tracesFile << endl;
    cout << "| Gnuplot file: " << gplFile << endl;
    cout << "| Number of traces: ";
    if(n_traces == 0)   cout << "all"<< endl;
    else                cout << n_traces << endl;
    cout << "| From sample: " << x_min << endl;
    cout << "| To sample: ";
    if(x_max == 0)   cout << "all"<< endl;
    else                cout << x_max << endl;
    
    cout << endl;
}

/**
 * Open the I/O files
 */
void openFiles() {
    cout << "Opening input file ..." << flush;
    fin.open(inputFile);
    if(fin.fail()) {
        cerr << " [ERROR]" << endl;
        exit(0);
    }else
        cout << " [OK]" << endl;
    
    cout << "Creating traces file ...";
    traces_dat.open(tracesFile);
    if(traces_dat.fail()) {
        cerr << " [ERROR]" << endl;
        exit(0);
    }else
        cout << " [OK]" << endl;
    
    cout << "Creating mean file ...";
    mean_dat.open(meanFile);
    if(mean_dat.fail()) {
        cerr << " [ERROR]" << endl;
        exit(0);
    }else
        cout << " [OK]" << endl;
    
    cout << "Creating gpl file ...";
    gpl.open(gplFile);
    if(gpl.fail()) {
        cerr << " [ERROR]" << endl;
        exit(0);
    }else
        cout << " [OK]" << endl;
}

/**
 * Close the opened files
 */
void closeFiles() {
    fin.close();
    traces_dat.close();
    mean_dat.close();
    gpl.close();
}

/**
 * Parse the header informations
 */
void parseInputFile() {
    cout << endl << "Parsing input file ..." << flush;
    
    fin.seekg(0, fin.beg);
        
    fin.read((char*)&raw.numtraces, sizeof(raw.numtraces));
    dim.n_traces = raw.numtraces;
        
    fin.read((char*)&raw.numsamples_per_trace, sizeof(raw.numsamples_per_trace));
    dim.n_samples = raw.numsamples_per_trace;
        
    fin.read((char*)&raw.datatype, sizeof(raw.datatype));
    switch ( raw.datatype ) {
        case 'f':   dim.sample = 4; break;
        case 'd':   dim.sample = 8; break;
        default:    cerr << "[ERROR]: Sample data type should be f <float> or d <double> " << endl; exit(0);
    }
        
    fin.read((char*)&raw.knowndatalength, sizeof(raw.knowndatalength));
    dim.knowndata = (int) raw.knowndatalength;
    dim.header = sizeof(raw.numtraces) + sizeof(raw.numsamples_per_trace) + sizeof(raw.datatype) + sizeof(raw.knowndatalength);
    dim.trace = dim.n_samples * dim.sample;   
    dim.file = dim.header + dim.n_traces * (dim.knowndata + dim.trace);

    if(n_traces == 0 || n_traces > dim.n_traces)
        n_traces = dim.n_traces;
    if(x_max == 0 || x_max > dim.n_samples) 
        x_max = dim.n_samples;
    if(x_min >= x_max) {
        cerr << "[ERROR]: Sample rate must be consistent" <<endl; 
        exit(0);
    }
    
    cout << " [OK]" << endl;
        
    cout << "| Number of traces: " << dim.n_traces << endl;
    cout << "| Number of samples per trace: " << dim.n_samples << endl;
    cout << "| Sample size: " << dim.sample << " Bytes" << endl;
    cout << "| Known data size: " << dim.knowndata << " Bytes" << endl;
    cout << "| Header size: " << dim.header << " Bytes" << endl;
    cout << "| Trace size: " << dim.trace << " Bytes" << endl;
    cout << "| File size: " << dim.file << " Bytes" << endl;
}

/**
 * Create the .dat output file with the extracted traces samples
 */
void createDat() {
    cout << endl << "Creating dat files ... " << flush;

    for(int isample = x_min; isample < x_max; isample++) {
        traces_dat << isample;
        mean_dat << isample;
        
        int n = 0;
        float mean = 0;
        float M2 = 0;
        
        for(int itrace = 0; itrace < n_traces; itrace++) {
            n++;
            double delta = 0;
            double sample = 0;
            fin.seekg (dim.header + (itrace + 1)*dim.knowndata + (itrace)*dim.trace + isample * dim.sample, fin.beg);
            switch ( raw.datatype ) {
                case 'f':   float fsample; 
                            fin.read((char*)&fsample, dim.sample);
                            traces_dat << " " << fsample;
                            sample = fsample;
                            break;
                case 'd':   double dsample; 
                            fin.read((char*)&dsample, dim.sample);
                            traces_dat << " " << dsample;
                            sample = (float) dsample;
                            break;
            }
            delta = sample - mean;
            mean = mean + delta/n;
            M2 = M2 + delta*(sample - mean);
        }
        
        float variance = 0;
        if (n > 1)
            variance = M2 / (n - 1);
        
        mean_dat << " " << mean << " " << variance;
        traces_dat << endl;
        mean_dat << endl;
    }
    
    cout << "[OK]" << endl;
}

/**
 * Create the gnuplot file
 */
void createGpl() {
    cout << "Creating gpl file ... " << flush;
        
    gpl << "set term png size 3000,1500 crop;" << endl;
    gpl << "set output \"power_traces.png\";" << endl;
    gpl << "set autoscale;" << endl;
    gpl << "set xtic auto;" << endl;
    gpl << "set ytic auto;" << endl;
    gpl << "set xrange [" << x_min << ":" << x_max << "];" << endl;
    gpl << "set key off;" << endl;
    gpl << "set xlabel \"Sample number\";" << endl;
    gpl << "set ylabel \"Power Trace\";" << endl << endl;

    gpl << "plot  "; 
    for(int itrace = 0; itrace < N_TRACES_TO_PLOT; itrace++) {
        if(itrace != 0){
            gpl << ", " ;
        }
        gpl << "\""<< tracesFile << "\" ";
        gpl << "u 1:" << itrace+2 << " ";
        gpl << "t \"" << itrace+1 << "\" ";
        gpl << "with lines";
    }
    
    gpl << endl << endl << "set term png size 3000,1500 crop;" << endl;
    gpl << "set output \"power_traces_mean_" << x_min << "_" << x_max << ".png\";" << endl;
    gpl << "set autoscale;" << endl;
    gpl << "set xtic auto;" << endl;
    gpl << "set ytic auto;" << endl;
    gpl << "set xrange [" << x_min << ":" << x_max << "];" << endl;
    gpl << "set key off;" << endl;
    gpl << "set xlabel \"Sample number\";" << endl;
    gpl << "set ylabel \"Power Trace Mean\";" << endl;
    gpl << "set style line 1 lw 5 linecolor rgb \"blue\";"  << endl << endl;

    gpl << "plot  ";
    gpl << "\""<< meanFile << "\" ";
    gpl << "u 1:2 ";
    gpl << "t \"mean\" ";
    gpl << "with lines ls 1";
    
    gpl << endl << endl << "set term png size 3000,1500 crop;" << endl;
    gpl << "set output \"power_traces_variance_" << x_min << "_" << x_max << ".png\";" << endl;
    gpl << "set autoscale;" << endl;
    gpl << "set xtic auto;" << endl;
    gpl << "set ytic auto;" << endl;
    gpl << "set xrange [" << x_min << ":" << x_max << "];" << endl;
    gpl << "set key off;" << endl;
    gpl << "set xlabel \"Sample number\";" << endl;
    gpl << "set ylabel \"Power Trace Variance\";" << endl << endl;
    gpl << "set style line 1 lw 5 linecolor rgb \"green\";"  << endl << endl;

    gpl << "plot  ";
    gpl << "\""<< meanFile << "\" ";
    gpl << "u 1:3 ";
    gpl << "t \"variance\" ";
    gpl << "with lines ls 1";
    
    
    cout << "[OK]" << endl;
}