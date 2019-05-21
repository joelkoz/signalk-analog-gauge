#include <Arduino.h>
#include "AnalogGauge.h"
#include "GaugeIcons.h"
#include "sensesp_app.h"

#include <ESP8266WiFi.h>
#include <math.h>

#ifndef PI
#define PI 3.1457
#endif

#define MAX_GAUGE_INPUTS 5


AnalogGauge::AnalogGauge(DFRobot_Display* pDisplay,
                         double minVal, double maxVal,
                         String config_id, String schema) :
   NumericConsumer(),
   Configurable(config_id, schema),
   display(*pDisplay), minVal{minVal}, maxVal{maxVal} {

   maxDisplayIdx = 0;
   currentDisplayIdx = 0;

   input = new double[MAX_GAUGE_INPUTS];
   valueSuffix = new char[MAX_GAUGE_INPUTS];

   display.fillScreen(DISPLAY_BLACK);
   display.setTextColor(DISPLAY_WHITE);
   display.setTextBackground(DISPLAY_BLACK);
   display.setCursor(26, 58);
   display.setTextSize(2);
   display.printf("SensESP");

   pGaugeIcon = (uint8_t*)image_data_icontemp;

}

void AnalogGauge::setValueSuffix(char suffix, int valueIdx) {
   if (valueIdx >= 0 && valueIdx < MAX_GAUGE_INPUTS) {

      if (valueIdx > maxDisplayIdx) {
         maxDisplayIdx = valueIdx;
      }

      valueSuffix[valueIdx] = suffix;
   }
   else {
      debugW("WARNING! calling AnalogGauge::setValueSuffix with a value index out of range - ignoring.");
   }
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
        if (wifiConnected && sigkConnected) {
            display.drawBmp(pGaugeIcon, -14, 35, 28, 26);  
        }
        else {
           display.drawBmp((uint8_t*)image_data_iconwifi, -12, 35, 23, 20);  
        }
     }
     wifiIcon = 1;
  }
  else {
     if (wifiIcon != 0) {
        if (wifiConnected && !sigkConnected) {
            display.drawBmp(pGaugeIcon, -14, 35, 28, 26);  
        }
        else {
           display.fillRect(-12, 35, 23, 20, DISPLAY_BLACK);
        }
     }
     wifiIcon = 0;
  }

  blinkCount = (blinkCount + 1) % blinkRate;
}


void AnalogGauge::updateGauge() {

   float newDialValue = input[0];

   if (newDialValue > maxVal) {
      newDialValue = maxVal;
   }
   else if (newDialValue < minVal) {
      newDialValue = minVal;
   }

   uint16_t gaugeColor = DISPLAY_WHITE;

   // Update the dial...
   if (newDialValue != lastDialValue) {
      drawNeedle(lastDialValue, DISPLAY_BLACK);
      lastDialValue = newDialValue;
      drawNeedle(newDialValue, gaugeColor);

   }


   bool wifiConnected = sensesp_app->isWifiConnected();
   if (!wifiConnected || currentDisplayIdx > maxDisplayIdx) {
      if (!ipDisplayed) {
         // Display the device's IP address...
         display.fillRect(-64, 6, 128, 35, DISPLAY_BLACK);
         display.setTextColor(gaugeColor);
         display.setTextBackground(DISPLAY_BLACK);
         display.setCursor(40, 80);
         display.setTextSize(1);
         display.printf("%s", WiFi.localIP().toString().c_str());
         ipDisplayed = true;
      }
   }
   else {
      if (ipDisplayed) {
         // Clear out the IP address
         display.fillRect(-64, 6, 128, 25, DISPLAY_BLACK);
         ipDisplayed = false;
      }

      // Update the digital display...
      float newDisplayValue = input[currentDisplayIdx];
      if (newDisplayValue != lastDisplayValue) {

         display.setTextColor(gaugeColor);
         display.setTextBackground(DISPLAY_BLACK);
         display.setCursor(40, 78);
         display.setTextSize(2);
         display.printf("%5.1f%c", newDisplayValue, valueSuffix[currentDisplayIdx]);

      }
   }

}


void AnalogGauge::enable() {
    drawDisplay();
    app.onRepeat(500, [this]() { this->updateWifiStatus(); });
    app.onRepeat(750, [this]() { this->updateGauge(); });
}


void AnalogGauge::set_input(float newValue, uint8_t idx) {

   if (idx >= 0 && idx < MAX_GAUGE_INPUTS) {

      input[idx] = newValue;
   }
}


void AnalogGauge::set_input(bool buttonPressed, uint8_t idx) {

   if (buttonPressed) {
       if (currentDisplayIdx > maxDisplayIdx) {
         currentDisplayIdx = 0;
       }
       else {
         currentDisplayIdx++;
       }
      updateGauge();
   }
}


JsonObject& AnalogGauge::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["default_display"] = currentDisplayIdx;
  return root;
}


bool AnalogGauge::set_configuration(const JsonObject& config) {

  String expected[] = { "default_display" };
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      debugE("Can not set Interpolate configuration: missing json field %s\n", str.c_str());
      return false;
    }
  }

  currentDisplayIdx = config["default_display"];
}
