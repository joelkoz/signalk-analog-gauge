#include <Arduino.h>
#include "AnalogGauge.h"
#include "GaugeIcons.h"
#include "sensesp_app.h"

#include <ESP8266WiFi.h>
#include <math.h>
#include <cstring>

#ifndef PI
#define PI 3.1457
#endif

#define MAX_GAUGE_INPUTS 5


AnalogGauge::ValueColor::ValueColor() {
}

AnalogGauge::ValueColor::ValueColor(float minVal, float maxVal, uint16_t color) : minVal{minVal}, maxVal{maxVal}, color{color} {
}

AnalogGauge::ValueColor::ValueColor(JsonObject obj) : minVal{obj["minVal"]}, maxVal{obj["maxVal"]}, color{obj["color"]} {

}


AnalogGauge::AnalogGauge(DFRobot_Display* pDisplay,
                         double minVal, double maxVal,
                         String config_path) :
   NumericConsumer(),
   Configurable(config_path),
   display(*pDisplay), minVal{minVal}, maxVal{maxVal} {

   maxDisplayChannel = 0;
   currentDisplayChannel = 0;

   input = new double[MAX_GAUGE_INPUTS];
   valueSuffix = new char[MAX_GAUGE_INPUTS];
   std::memset(valueSuffix, ' ', MAX_GAUGE_INPUTS);
   precision = new uint8[MAX_GAUGE_INPUTS];
   std::memset(precision, 0, MAX_GAUGE_INPUTS);

   display.fillScreen(DISPLAY_BLACK);
   display.setTextColor(DISPLAY_WHITE);
   display.setTextBackground(DISPLAY_BLACK);
   display.setCursor(26, 58);
   display.setTextSize(2);
   display.printf("SensESP");
   pGaugeIcon = (uint8_t*)image_data_icontemp;

}


void AnalogGauge::addValueRange(const ValueColor& newRange) {
   ValueColor* pRange = new ValueColor(newRange);
   valueColors.insert(*pRange);
}


uint16_t AnalogGauge::getValueColor(float value) {

  std::set<ValueColor>::iterator it = valueColors.begin();

  while (it != valueColors.end()) {
     auto& range = *it;

     if (value >= range.minVal && value <= range.maxVal) {
        // Here is our match
        return range.color;
     }

     it++;
  } // while

  return defaultValueColor;

}



void AnalogGauge::setValueSuffix(char suffix, int inputChannel) {
   if (inputChannel >= 0 && inputChannel < MAX_GAUGE_INPUTS) {

      if (inputChannel > maxDisplayChannel) {
         maxDisplayChannel = inputChannel;
      }

      valueSuffix[inputChannel] = suffix;
   }
   else {
      debugW("WARNING! calling AnalogGauge::setValueSuffix with an input channel out of range - ignoring.");
   }
}


void AnalogGauge::setPrecision(uint8 precision, int inputChannel) {
   if (inputChannel >= 0 && inputChannel < MAX_GAUGE_INPUTS) {

      if (inputChannel > maxDisplayChannel) {
         maxDisplayChannel = inputChannel;
      }

      this->precision[inputChannel] = precision;
   }
   else {
      debugW("WARNING! calling AnalogGauge::setPrecision with an input channel out of range - ignoring.");
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


  // Draw color ranges (if any)...
  std::set<ValueColor>::iterator it = valueColors.begin();
  while (it != valueColors.end()) {
     auto& range = *it;
     double minPct = (range.minVal - minVal) / valueRange;
     double maxPct = (range.maxVal - minVal) / valueRange;
     drawGaugeRange(minPct, maxPct, 0.002, range.color);
     it++;
  } // while


  display.fillCircle(0, 0, 3, DISPLAY_WHITE);

}



void AnalogGauge::updateWifiStatus() {

   bool wifiConnected = sensesp_app->isWifiConnected();
   bool sigkConnected = sensesp_app->isSignalKConnected();

   int blinkRate = 2;

   bool blinkDisplay =  (blinkCount % blinkRate == 0);

   int newIcon;

   if (wifiConnected && sigkConnected) {
      newIcon = 2;
   }
   else if (blinkDisplay) {
     newIcon = (displayIcon+1) % 4;
     if (!wifiConnected) {
        if (newIcon == 2) {
           newIcon = 0;
        }
     } 
   }
   else {
     newIcon = displayIcon;
   }

   switch (newIcon) {
      case 0:
         if (displayIcon != 0) {
            display.drawBmp((uint8_t*)image_data_iconwifi, -12, 35, 23, 20); 
            displayIcon = 0;
         }
         break;

      case 1:
      case 3:
         if (displayIcon != 1 && displayIcon != 3) {
            display.fillRect(-14, 35, 28, 26, DISPLAY_BLACK);
            displayIcon = newIcon;
         }
         break;

      case 2:
         if (displayIcon != 2) {
            display.drawBmp(pGaugeIcon, -14, 35, 28, 26);   
            displayIcon = 2;
         }
         break;

   } // switch

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

   uint16_t gaugeColor = getValueColor(newDialValue);

   // Update the dial...
   if (newDialValue != lastDialValue) {
      drawNeedle(lastDialValue, DISPLAY_BLACK);
      lastDialValue = newDialValue;
      drawNeedle(newDialValue, gaugeColor);

   }


   bool wifiConnected = sensesp_app->isWifiConnected();
   if (!wifiConnected || currentDisplayChannel > maxDisplayChannel) {
      if (!ipDisplayed) {
         // Display the device's IP address...
         display.fillRect(-64, 6, 128, 25, DISPLAY_BLACK);
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
      float newDisplayValue = input[currentDisplayChannel];
      if (newDisplayValue != lastDisplayValue) {

         display.setTextColor(gaugeColor);
         display.setTextBackground(DISPLAY_BLACK);
         display.setCursor(40, 78);
         display.setTextSize(2);
         char fmtStr[10];
         uint8 prec = precision[currentDisplayChannel];
         if (prec > 0) {
            int wholeSize = 6 - prec;
            sprintf(fmtStr, "%%%d.%df%%c" , wholeSize, (int)prec);
         }
         else {
            strcpy(fmtStr, "%5.0f%c ");
         }
         display.printf(fmtStr, newDisplayValue, valueSuffix[currentDisplayChannel]);

      }
   }

}


void AnalogGauge::enable() {
    load_configuration();
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
       if (currentDisplayChannel > maxDisplayChannel) {
         currentDisplayChannel = 0;
       }
       else {
         currentDisplayChannel++;
       }
      updateGauge();
   }
}


void AnalogGauge::get_configuration(JsonObject& root) {

  root["default_display"] = currentDisplayChannel;
  root["minVal"] = minVal;
  root["maxVal"] = maxVal;

  JsonArray ranges = root.createNestedArray("ranges");
  for (auto& range : valueColors) {
    JsonObject entry = ranges.createNestedObject();
    entry["minVal"] = range.minVal;
    entry["maxVal"] = range.maxVal;
    entry["color"] = range.color;
  }
}


bool AnalogGauge::set_configuration(const JsonObject& config) {

  String expected[] = { "default_display", "minVal", "maxVal" };
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      debugE("Can not set AnalogGauge configuration: missing json field %s\n", str.c_str());
      return false;
    }
  }

  currentDisplayChannel = config["default_display"];
  minVal = config["minVal"];
  maxVal = config["maxVal"];

  JsonArray ranges = config["ranges"];
  if (ranges.size() > 0) {
    valueColors.clear();
    for (JsonObject entry : ranges) {
        ValueColor range(entry);
        valueColors.insert(range);
    }

  }

  return true;

}


static const char SCHEMA[] PROGMEM = R"({
   "type": "object",
   "properties": {
      "minVal": { "title": "Minimum analog value", "type": "number"},
      "maxVal": { "title": "Maximum analog value", "type": "number"},
      "default_display": { "title": "Default input to display", "type": "number", "minimum": 0, "maximum": 5 },
      "ranges": { "title": "Ranges and colors",
                     "type": "array",
                     "items": { "title": "Range",
                                 "type": "object",
                                 "properties": {
                                       "minVal": { "title": "Min value for color", "type": "number" },
                                       "maxVal": { "title": "Max value for color", "type": "number" },
                                       "color": { "title": "Display color RGB", "type": "number" }
                                 }}}

   }
})";


String AnalogGauge::get_config_schema() {
   return FPSTR(SCHEMA);
}
