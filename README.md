# DD6Extras

This is collection of [ROOT](https://root.cern/) macros and C++ programs complementary with the [DesktopDigitizer6](https://github.com/kasia-rusiecka/DesktopDigitizer6) project and useful for preliminary analysis of data recorded with the CAEN Desktop Digitizer.

If you want all the macros to be loaded automatically in your ROOT sessions in all directories add to your .rootrc file:
```
Unix.*.Root.MacroPath:      .:$(HOME)/Macros:/path/to/this/project:
```

## Prerequisites 
* Required ROOT version 6.13/01
* libDesktopDigitizer6.so

## Environmental settings
* $SFDATA - path to the directory where data is stored
* $DD6PATH - path to the DesktopDigitizer6 project and libDesktopDigitizer6.so library

## Lisf of macros

### SignalsViewer.C

ROOT script for fast viewing of signals recorded by the Desktop Digitizer. Opens binary files recorded by the Desktop Digitizer, reads them and fills histograms. Signals are plotted on the canvas with the WaitPrimitive() method, so to see next signal double-click on the canvas is required. Script should be run in the directory where data is stored. Binary data is sufficient for SignalsViewer() functions, data doesn't have to be digitized. Function CutAndView() requires digitized data in order to impose demanded cuts. Three functions are implememnted within this macro:
1. `SignalsViewer(Int_t ch0, Int_t ch1, Int_t ylimit, Bool_t BL)` Draws correlated signals from two chosen channels `ch0` and `ch1`. 
2. `SignalsViewer(Int_t ch, Bool_t BL)` Draws a signal from a chosen channel `ch`.
3. `CutAndView(Int_t ch, TString mode, Double_t xmin, Double_t xmax, Int_t no)` Allows to view specific signals, selected based on the cut on signal amplitude (fAmp), uncalibrated integral (fCharge), calibrated integral (fCal) or time T0 (fT0). Cut ranges are given as xmin and xmax. `no` signals are drawn on the divided canvas.

If .rootrc file is setup correctly this macro will be loaded in ROOT session automatically and all functions will be available in data directories.

To run type:
```
root
.L SignalsViewer.C
Function(arguments)
```

### BaseLine.C
description

### Calibrate.C

Runs digitization/calibration of the whole measurement series. Logfile containing list of measurement names and corresponding source positions is opened. Digitization is performed for the measurements included in this logfile. For each measurement program digit (part of  the DesktopDigitizer6) is executed, so correct configuration files config.txt should be provided. 

To compile type:
```
gcc Calibrate.C -lstdc++
```

To run type:
```
./Calibrate.o path/to/log.txt
```

### AttFast.C

ROOT macro for fast determination of attenuation length. As an argument takes name of the logfile containing names of the measurements and corresponding source positions. Determines attenuation length with two methods:
1. Combined channels - based on charge ratio from both channels,
2. Separate channels - approximated, based on position of the 511 keV peak, but not  background-subtracted. Peaks positions are determined with TSpectrum.

Created canvases:
1. All charge spectra from this measurements
2. Ratio histograms
3. Attenuation graphs and curves, both methods

To run type:
```
root
.L AttFast.C
AttFast("logfile.txt")
```
