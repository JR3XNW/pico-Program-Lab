# pico-Program-Lab
# Digital Signal Processing Program Overview

This program utilizes Digital Signal Processing (DSP) to analyze and graphically display the spectrum of audio or other signals. It serves as a valuable tool in amateur radio and acoustic analysis, consisting of several key components:

## Initial Setup and Library Imports

- `U8g2lib.h` is used for OLED display manipulation.
- `arduinoFFT.h` is utilized for performing Fast Fourier Transform (FFT).

## Global Variables Definition

- `filterLength`, `filterBuffer`, `firCoeffs` are utilized for the FIR filter setup and implementation.
- `samples`, `startFrequency`, `endFrequency` are associated with the FFT settings.

## FIR Filter Function (`firFilter`)

This function applies an FIR filter to the input signal, outputting the filtered signal. It involves adding the current input value to a buffer, weighting the values in the buffer with filter coefficients, and computing their total sum.

## Frequency Sweep Function (`frequencySweep`)

Generates a signal within the set range from start to end frequency, passes it through the FIR filter, and stores the results in the array for the real part of the FFT. The imaginary part is initialized to 0.

## FFT and OLED Display Function (`displayFFT`)

Performs FFT, converts the complex results to magnitude (amplitude), and displays it on the OLED display as a bar graph. The FFT results are preprocessed with a window function, followed by the FFT computation and conversion of complex results to magnitude, which are then plotted as lines on the display.

## `setup` and `loop` Functions

The `setup` function handles the initial settings of the OLED display and draws static UI elements. The `loop` function conducts the frequency sweep, calculates the FFT using its results, and draws these results on the display.

### Variable Usage

- **`filterLength`**: Defines the length (number of taps) of the filter, determining the FIR filter's precision and characteristics.
- **`filterBuffer`**: A buffer to hold the current samples being processed.
- **`firCoeffs`**: Coefficients of the FIR filter, determining its characteristics, e.g., which frequencies to pass or suppress.
- **`samples`, `startFrequency`, `endFrequency`**: Define the number of samples for FFT analysis and the frequency range to be analyzed.

### Modifying the Filter Program

Changing the FIR filter coefficients (`firCoeffs` array) allows for modification of the filter's characteristics. New filter coefficients should be chosen or calculated based on the design requirements of the filter, e.g., emphasizing certain frequency bands or designing low/high-pass filters to suppress certain bands. Calculating filter coefficients requires expertise in digital signal processing, though online tools are also available for generating these coefficients.

#### Coefficient Calculation

Designing an FIR filter necessitates calculating coefficients based on filter specifications (cut-off frequency, bandpass characteristics, etc.). Functions from MATLAB's Filter Design Toolbox or Python's `scipy.signal.firwin` can be used for this purpose.

#### Updating Coefficients

Newly calculated or chosen coefficients should be assigned to the `firCoeffs` array, adjusting the array size (`filterLength`) accordingly to match the new coefficients' count.

#### Program Testing

After updating the coefficients, run the program to ensure the filter operates as expected. Observing the filter's response to an input signal and analyzing the output signal's frequency characteristics with a spectrum analyzer can prove beneficial.

### Considerations

Changing the FIR filter's coefficients can affect the filter's delay characteristics and stability. Particularly, a high number of taps (`filterLength`) can increase computational load, impacting real-time processing capabilities. Enhancing filter performance requires consideration not only of coefficient accuracy but also the number of samples processed (`samples`), the sampling rate, and the resolution of the FFT.

