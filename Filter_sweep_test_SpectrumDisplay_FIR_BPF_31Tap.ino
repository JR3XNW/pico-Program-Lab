/*
  Digital filter spectrum bar graph display program
  Using a Raspberry Pi Pico, the spectrum of the signal through 
  the FIR filter is displayed as a bar graph on the OLED display.
  FIR filter 31taps Bandpass 1kHz 50Hz
  2024/06/30 JR3XNW
*/
#include <Arduino.h>
#include <U8g2lib.h>
#include <arduinoFFT.h> // v2.0.2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
ArduinoFFT<double> FFT;  // v2.0.2 Explicit data types using templates

const int filterLength = 31; // Number of filter taps.
float filterBuffer[filterLength] = {0.0};
float firCoeffs[filterLength] = { // FIR filter coefficients for Bandpass 1kHz 100Hz
  0.000000, -0.003430, -0.008679, -0.016746, 
 -0.027240, -0.038247, -0.046693, -0.049118, 
 -0.042670, -0.026024, 0.000000, 0.032351, 
 0.066210, 0.095939, 0.116277, 0.123504, 
 0.116277, 0.095939, 0.066210, 0.032351, 
 0.000000, -0.026024, -0.042670, -0.049118, 
 -0.046693, -0.038247, -0.027240, -0.016746, 
 -0.008679, -0.003430, 0.000000,
};

int bufferIndex = 0;
const int samples = 128; // Number of FFT samples.
const float startFrequency = 100.0;  // Start frequency (Hz)
const float endFrequency = 10000.0;  // End frequency (Hz)
const float sampleRate = 20000.0;    // Sample rate (Hz)

// Update to FIR filter function
float firFilter(float input) {
  filterBuffer[bufferIndex] = input;
  bufferIndex = (bufferIndex + 1) % filterLength;
  float sum = 0.0;
  for (int i = 0; i < filterLength; i++) {
    int index = (bufferIndex - i + filterLength) % filterLength;
    sum += filterBuffer[index] * firCoeffs[i];
  }
  return sum;
}

// frequency sweep function.
void frequencySweep(double *vReal, double *vImag) {
  float frequencyStep = (endFrequency - startFrequency) / samples;
  for (int i = 0; i < samples; i++) {
    float currentFrequency = startFrequency + (i * frequencyStep);
    float signal = sin(2 * PI * currentFrequency * millis() / 1000.0);
    float filteredSignal = firFilter(signal);
    vReal[i] = filteredSignal;
    vImag[i] = 0;
  }
}

// FFT and OLED display functions (line graph display).
void displayFFT(double *vReal, double *vImag) {
  FFT.windowing(vReal, samples, FFTWindow::Hamming, FFTDirection::Forward);  // Use new enum types instead of FFT_WIN_TYP_HAMMING and FFT_FORWARD
  FFT.compute(vReal, vImag, samples, FFTDirection::Forward);  // Change FFT_REVERSE to FFTDirection::Reverse
  FFT.complexToMagnitude(vReal, vImag, samples);  // No change

  // Clear graph drawing area (overwrite with background color).
  // Here, the graph area is overwritten with a black rectangle.
  u8g2.setDrawColor(0); // Set background color (usually black).
  u8g2.drawBox(0, 0, 128, 52); // Clear graph area.
  u8g2.setDrawColor(1); // Re-set the drawing color (white).

  // Drawing FFT results.
  for (int i = 1; i < (samples / 2); i++) {
    int x = map(i, 1, samples / 2, 0, 128);
    int y = map(vReal[i], 0, 16, 53, 0); // Scaling of spectral intensity, changing the third argument (now 14) of the map function
    u8g2.drawLine(x, 53, x, y);
  }

  // Display the text "FIR filter 31taps Bandpass 1kHz 100Hz"
  char str[40] = "FIR BPF filter 31taps 1kHz 50Hz";
  byte x = u8g2.getStrWidth(str);
  u8g2.setFont(u8g2_font_micro_tr); // Set the font for the numbers
  //u8g2.setFont(u8g2_font_6x10_tr); // Set a suitable font for the title
  u8g2.drawStr(128 - x, 5, str);

  // Send buffer to update screen.
  u8g2.sendBuffer();
}

void setup() {
  u8g2.begin();
  // Redraw the static content (lines and texts) each time to prevent flickering
  u8g2.drawLine(0, 53, 128, 53);
  
  for (int xl = 10; xl < 127; xl += 13) {
    u8g2.drawLine(xl, 53, xl, 55);
  }
  u8g2.setFont(u8g2_font_micro_tr); // Set the font for the numbers
  u8g2.drawStr(8, 64, "1k");  //-2 10
  u8g2.drawStr(21, 64, "2k");
  u8g2.drawStr(34, 64, "3k"); 
  u8g2.drawStr(47, 64, "4k");
  u8g2.drawStr(60, 64, "5k");
  u8g2.drawStr(73, 64, "6k");
  u8g2.drawStr(86, 64, "7k");
  u8g2.drawStr(99, 64, "8k");
  u8g2.drawStr(112, 64, "9k");
}

void loop() {
  double vReal[samples];
  double vImag[samples] = {0}; // Imaginary part is initialized to 0.

  frequencySweep(vReal, vImag);
  displayFFT(vReal, vImag); // Call a function to display FFT results.
}
