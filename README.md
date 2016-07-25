# AnalystPMT v2
Packets for analysing ANNIE DAQ output files.
The main code scans the time profiles of the PMTs, looking for pulses and collecting them into a ROOT tree.
The operation resembles a zero suppression data acquisition.
Discrimination between signals and backgound is also done.

##Utils, PulseFinder and EventReco
These are three classes used by the main codes.
* "Utils" is a singleton base class, which holds the configuration procedures and other frequently used functions.
* "PulseFinder" is a derived class and takes care of scanning the data, selecting the pulses, gathering them into events, plotting histogerams and filling ROOT trees.
* "EventReco" is a derived class, called by PulseFinder, that evaluates some useful parameters for each pulses.

Input variables are set through a config file.

##Main, Merge (SuperMerge) and Select

There are three executables in the repository.
* "Main" is the fundamental executable that scans post processed ROOT files, retrieve the individual pulses and generates output ROOT files.
* "Merge" is used to join multiple output ROOT files into a single one. It's automatically called at the end of Main.
It's possible to Merge already merged ROOT files and this is done by the "SuperMerge" executable.
* "Selection" allows to create average pulse shapes using cuts on the ROOT trees

###Building and Usage
A Makefile is available to build the software.
To build them all
```
make
```

otherwise they can be built separately

|Executable | `make`   | 
|---------- |--------- |
|Main       | `pulse`  |
|Merge      | `merge`  |
|SuperMerge | `super`  |
|Select     | `select` |

Main requires 0 or 1 argument.
The sole argument is the directory where the root files (preferably from the same Run/Subrun) are stored.
Within this folder, a new directory called `result` is created.
If 0 argument are passed, the the current directory is used.

Two outputs are generated for each input root file: a root file and a text file.
The name is the same of the input file, except for the extension and a `r` in front of it if it's a root file or a `d` if it's a dat file.
The root file would contain all the histograms and graphs created, the text file some statistical information concerning the histograms and more.

```
./Main directory
```

For merging the root file into one, the Merge programs must be called.
It requires 0 or 1 argument, which is the folder of the output root file, therefore terminating with `/result`.
In no argoment is passed, then the current folder is used.

```
./Merge directory/result
```

For supermerging the merged root files into one, the SuperMerge programs must be called.
It requires the merged files' path as argument.

```
./SuperMerge dir1/Merged2.root dir3/Merged4.root ...
```

The code should be compatible with the older version of post processed ROOT files.
In order to analyse those files, the PMTData.h (MakeClass header) should be changed as long as the `sample` configuration in the config file.
However, this feature has never been tested.

##Config File
Utils class looks for a file called `config` in the current directory, where all the configuration are saved.
Since Utils is a singleton class, it is initialised only once as the setting of the config values.

* `debug` with 1 overwrites already analysed files, 0 analyses only the new ones.
* `verbosity` is the verbosity level.
* `hysratio` is the hysteresis ratio with respect to any threshold, 0 for no hysteresis.
* `thrpeak` is the threshold for pulse definition, in V .
* `trhevent` is the threshold for event definition.
* `thrsignal` is the threshold for signal discrimination from background, in us.
* `shapingtime` is the time window for peak search.
* `sample` is the ratio of downsampling.
* `binwidth` is the time resolution of the digitiser, in us.
* `buffer` is the total length of the digitiser buffer.
* `printgraph` sets how many graph skips before saving, 0 doesn't save.
* `printevent` sets the threshold in PMT fired for full 2D event plot, -1 don't print.
* `percent` is the position of the peak in the graph window, in percent.
* `eventlength` is the bin size of graph window.
* `lowbound` is the lower bound for the TOF cut.
* `upbound` is the upper bound for the TOF cut.

#How are pulses selected?
Everything above the `thrpeak` threshold is a pulse candidate.
The time profile is cut out around the peak into a time window `eventlength` wide.
The peak is placed at the `percent` of the time window.

At the same time, the number of PMTs fired vs time gives an indication of events occurrence.
If this number is greater than `threvent` then every pules that falls whithin `thrsignal` microseconds from the event time position is called a signal.
Other pulses are defined as background.

Signals are stored in the tEvent tree, while bakground pulses are stored in the tNoise tree.
The tree structure is as follows:

* EvtLength, is the time window length 
* TRG, is the trigger number
* PMTID, is the PMT ID number
* Baseline, is the average of the first 10 points of the pulse window, which is subtracted from the whole array
* Peak, is the heigth of the maximum with respect to 0
* P2V, is the time distance between the maximum (peak) and the minimum (valley)
* Time, is the time position of the rising edge, evaluated as 25% of peak heigth with cubic interpolation
* Width, is the width of the signal, spanning from Time to the 5% of the falling edge
* Charge, is the Reimann integral of 5 point around the peak
* Energy, is the Reimann integral from Time to Time+Width
* Area, is the Reimann integral of the modulus of the whole shape
* TOF, is the time difference between Time and the RWM time position
* Next, is the time difference between Time and the previous pulse's Time (if available)
* VETO, is set to 1 if the VETO was on during the pulse
* MRD2, is set to 1 if the second layer of the MRD was on during the pulse
* MRD3, is set to 1 if the third layer of the MRD was on during the pulse
* Pulse, is the time windows itself, saved as a float array EvtLength long
