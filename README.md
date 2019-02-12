# DD6Extras

This is collection of [ROOT](https://root.cern/) macros and C++ programs complementary with the [DesktopDigitizer6](https://github.com/kasia-rusiecka/DesktopDigitizer6) project and useful for preliminary analysis of data recorded with the CAEN Desktop Digitizer.

## Prerequisites 
* Required ROOT version 6.13/01
* libDesktopDigitizer6.so

## Environmental settings
* $SFDATA - path to the directory where data is stored
* $DD6PATH - path to the DesktopDigitizer6 project and libDesktopDigitizer6.so library

## Lisf of macros

### SignalsViewer.C
description

### BaseLine.C
description

### calibrate.C
description

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

