/*
  Digital filter spectrum bar graph display program
  This is a Raspberry Pi Pico that displays the results of IIR filtered signal processing as a bar graph on an OLED display. 
  Specifically, a second-order Butterworth filter is implemented to display the spectrum of the filtered signal.
  IIR filter 2nd order 3kHz
  2024/06/18 JR3XNW
*/
#include <Arduino.h>
#include <U8g2lib.h>
#include <arduinoFFT.h> // v2.0.2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
ArduinoFFT<double> FFT;  // v2.0.2 Explicit data types using templates

// フィルターの係数 (2次のバターワースフィルター)
const int filterOrder = 2;
double b[filterOrder + 1] = { 0.03478604, 0.06957207, 0.03478604 }; // 係数b
double a[filterOrder + 1] = { 1.0, -1.40750534, 0.54664949 }; // 係数a

double filterBufferX[filterOrder + 1] = {0.0};
double filterBufferY[filterOrder + 1] = {0.0};
const int samples = 128; // Number of FFT samples.
const float startFrequency = 100.0;  // Start frequency (Hz)
const float endFrequency = 10000.0;  // End frequency (Hz)

// IIRフィルターの実装
double iirFilter(double input) {
  // 入力をバッファに追加
  for (int i = filterOrder; i > 0; i--) {
    filterBufferX[i] = filterBufferX[i - 1];
    filterBufferY[i] = filterBufferY[i - 1];
  }
  filterBufferX[0] = input;

  // フィルター出力の計算
  double output = 0.0;
  for (int i = 0; i <= filterOrder; i++) {
    output += b[i] * filterBufferX[i];
    if (i > 0) {
      output -= a[i] * filterBufferY[i];
    }
  }
  filterBufferY[0] = output;

  return output;
}

// frequency sweep function.
void frequencySweep(double *vReal, double *vImag) {
  float frequencyStep = (endFrequency - startFrequency) / samples;
  for (int i = 0; i < samples; i++) {
    float currentFrequency = startFrequency + (i * frequencyStep);
    float signal = sin(2 * PI * currentFrequency * millis() / 256.0);
    double filteredSignal = iirFilter(signal);
    vReal[i] = filteredSignal;
    vImag[i] = 0;
  }
}

// FFT and OLED display functions (line graph display).
void displayFFT(double *vReal, double *vImag) {
  FFT.windowing(vReal, samples, FFTWindow::Hamming, FFTDirection::Forward);  // Use new enum types instead of FFT_WIN_TYP_HAMMING and FFT_FORWARD
  FFT.windowing(vImag, samples, FFTWindow::Hamming, FFTDirection::Forward);
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
    int y = map(vReal[i], 0, 24, 53, 0); // Scaling of spectral intensity, changing the third argument (now 14) of the map function
    u8g2.drawLine(x, 53, x, y);
  }

  // Display the text "IIR filter 2nd order 3kHz"
  char str[30] = "IIR filter 2nd order 3kHz";
  byte x = u8g2.getStrWidth(str);
  u8g2.setFont(u8g2_font_micro_tr); // Set the font for the numbers
  u8g2.drawStr(128 - x, 5, str);

  // Send buffer to update screen.
  u8g2.sendBuffer();
  delay(20);
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
