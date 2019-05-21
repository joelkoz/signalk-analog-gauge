#include <Arduino.h>

#include "sensesp_app.h"
#include "wiring_helpers.h"
#include "transforms/linear.h"
#include "devices/analog_input.h"

#include "DFRobot_ST7687S_Latch.h"

#include "displays/AnalogGauge.h"
#include "transforms/analogvoltage.h"
#include "transforms/voltagedividerR2.h"
#include "transforms/temperatureinterpreter.h"


const char* sk_path = "electrical.generator.engine.waterTemp";


// Wiring configuration and setup for TFT display
#define Display DFRobot_ST7687S_Latch
uint8_t pin_cs = D3, pin_rs = D1, pin_wr = D2, pin_lck = D4;

ReactESP app([] () {

#ifdef ARDUINO
  #ifndef SERIAL_DEBUG_DISABLED
  Serial.begin(115200);

  // A small arbitrary delay is required to let the
  // serial port catch up

  delay(100);
  Debug.setSerialEnabled(true);
  #endif
#endif

  // Initialize the LCD display
  Display* pDisplay = new Display(pin_cs, pin_rs, pin_wr, pin_lck);
  pDisplay->begin();

  sensesp_app = new SensESPApp();

  AnalogInput* pAnalogInput = new AnalogInput();
  AnalogVoltage* pVoltageOut = new AnalogVoltage();
  pAnalogInput->connectTo(pVoltageOut);

  VoltageDividerR2* pSenderValue = new VoltageDividerR2(51, 3.33, "", "/gauge/inputs");
  pVoltageOut->connectTo(pSenderValue);

  TemperatureInterpreter* pTempInKelvin = new TemperatureInterpreter("", "/gauge/display/curve");
  pSenderValue->connectTo(pTempInKelvin);

  Linear* pTempAdjust = new Linear(sk_path, 1.0, 0.0, "/gauge/display/adjust");
  pTempInKelvin->connectTo(pTempAdjust);


  // Gauge display setup...
  AnalogGauge *pGauge = new AnalogGauge(pDisplay, pTempAdjust, 255.37, 414.71);
 
  sensesp_app->enable();

});
