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
```
make pulse
make merge
make super
make select
```

|Main       | make pulse |
|Merge      | make merge |
|SuperMerge | make super |
|Select     | make select|

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
* `sample` is the ratio onf downsampling.
* `binwidth` is the time resolution of the digitiser, in us.
* `buffer` is the total length of the digitiser buffer.
* `printgraph` sets how many graph skips before saving, 0 doesn't save.
* `printevent` sets the threshold in PMT fired for full 2D event plot, -1 don't print.
* `percent` is the position of the peak in the graph window, in percent.
* `eventlength` is the bin size of graph window.
* `lowbound` is the lower bound for the TOF cut.
* `upbound` is the upper bound for the TOF cut.
