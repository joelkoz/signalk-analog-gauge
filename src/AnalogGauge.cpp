#include <Arduino.h>
#include "AnalogGauge.h"
#include "GaugeIcons.h"
#include "sensesp_app.h"

AnalogGauge::AnalogGauge(DFRobot_Display* pDisplay, double minVal, double maxVal,
                         String config_id, String schema) :
   NumericConsumer(),
   Configurable(config_id, schema),
   display(*pDisplay), minVal{minVal}, maxVal{maxVal} {

   display.fillScreen(DISPLAY_BLACK);
   display.setTextColor(DISPLAY_WHITE);
   display.setTextBackground(DISPLAY_BLACK);
   display.setCursor(30, 58);
   display.setTextSize(2);
   display.printf("SensESP");
}


void AnalogGauge::calcPoint(double pct, uint16_t radius, int16_t& x, int16_t& y) {
    double angle = PI * (1.0 - pct);
    x = radius * cos(angle);
    y = -(radius * sin(angle));
}


void AnalogGauge::drawHash(double pct, uint16_t startRadius, uint16_t endRadius, uint16_t color) {

  int16_t x0, y0, x1, y1;
  calcPoint(pct, startRadius, x0, y0);
  calcPoint(pct, endRadius, x1, y1);
  display.drawLine(x0, y0, x1, y1, color);
}


void AnalogGauge::drawGaugeRange(double minPct, double maxPct, double inc, uint16_t color) {
  for (double pct = minPct; pct <= maxPct; pct += inc) {
     drawHash(pct, 58, 70, color);
  }
}


void AnalogGauge::drawGaugeTick(double pct, uint16_t startRadius, uint16_t endRadius, uint16_t color) {
  double minVal = pct - 0.01;
  double maxVal = pct + 0.01;
  for (double val = minVal; val <= maxVal; val += 0.0033) {
     drawHash(val, startRadius, endRadius, color);
  }
}



void AnalogGauge::drawNeedle(double value, uint16_t color) {
    double pct = (value - minVal) / valueRange;
    drawHash(pct, 5, 40, color);
}


void AnalogGauge::drawDisplay() {

  display.fillScreen(DISPLAY_BLACK);

  // Draw the icon
  display.drawBmp((uint8_t*)image_data_icontemp, -14, 35, 28, 26);  


  // Do some pre-calc
  valueRange = maxVal - minVal;

  // Draw gauge ranges
  drawGaugeTick(0.01, 45, 57, DISPLAY_WHITE);
  drawGaugeTick(0.25, 50, 57, DISPLAY_WHITE);
  drawGaugeTick(0.5, 45, 57, DISPLAY_WHITE);
  drawGaugeTick(0.75, 50, 57, DISPLAY_WHITE);
  drawGaugeTick(1.0, 45, 57, DISPLAY_WHITE);

  display.fillCircle(0, 0, 5, DISPLAY_WHITE);

}



void AnalogGauge::updateWifiStatus() {

   bool wifiConnected = sensesp_app->isWifiConnected();
   bool sigkConnected = sensesp_app->isSignalKConnected();

   int blinkRate = (wifiConnected ? 4 : 2);

   bool displayAsConnected = (wifiConnected && sigkConnected) || (blinkCount % blinkRate == 0);

  if (displayAsConnected) {
     if (wifiIcon != 1) {
        display.drawBmp((uint8_t*)image_data_iconwifi, 31, 7, 23, 20);  
     }
     wifiIcon = 1;
  }
  else {
     if (wifiIcon != 0) {
        display.fillRect(31, 7, 23, 20, DISPLAY_BLACK);
     }
     wifiIcon = 0;
  }

  blinkCount = (blinkCount + 1) % blinkRate;
}


void AnalogGauge::updateGauge(float newValue) {

   if (newValue > maxVal) {
      newValue = maxVal;
   }
   else if (newValue < minVal) {
      newValue = minVal;
   }

   if (newValue != lastValue) {
      uint16_t gaugeColor = DISPLAY_WHITE;

      // Update the pointer...
      drawNeedle(lastValue, DISPLAY_BLACK);
      lastValue = newValue;
      drawNeedle(newValue, gaugeColor);

      display.setTextColor(gaugeColor);
      display.setTextBackground(DISPLAY_BLACK);
      display.setCursor(44, 78);
      display.setTextSize(2);
      display.printf("%4.1f", lastValue);
   }

}


void AnalogGauge::enable() {
    debugI("AnalogGauge enabled");
    drawDisplay();
    app.onRepeat(500, [this]() { this->updateWifiStatus(); });
}


void AnalogGauge::set_input(float newValue, uint8_t idx) {
   updateGauge(newValue);
}
