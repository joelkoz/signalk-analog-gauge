#include <Arduino.h>

#include "sensesp_app.h"
#include "wiring_helpers.h"
#include "transforms/linear.h"
#include "devices/analog_input.h"

#include "DFRobot_ST7687S_Latch.h"

#include "AnalogGauge.h"

const char* sk_path = "electrical.generator.engine.waterTemp";
const char* schema = "";

float k = 1; 
float c = 0;
const char* config_id = "/electrical/generator_temp";

// Wiring configuration and setup for TFT display
uint8_t pin_cs = D3, pin_rs = D1, pin_wr = D2, pin_lck = D4;
DFRobot_ST7687S_Latch display(pin_cs, pin_rs, pin_wr, pin_lck);

ReactESP app([] () {
  #ifndef SERIAL_DEBUG_DISABLED
  Serial.begin(115200);

  // A small arbitrary delay is required to let the
  // serial port catch up

  delay(100);
  Debug.setSerialEnabled(true);
  #endif

  // Initialize the LCD display
  display.begin();

  sensesp_app = new SensESPApp();

  AnalogInput* pSensor = new AnalogInput();

//  OneToOneTransform<float>* pTransform = new Linear(sk_path, k, c, config_id, schema);
  OneToOneTransform<float>* pTransform = new Linear(sk_path, 1.0 / 1024.0 * 3.3, 0, config_id, schema);

  pSensor->connectTo(pTransform);
    
  // Gauge display setup...
  AnalogGauge *pGauge = new AnalogGauge(&display, 1.7, 3.0);
  
  pTransform->connectTo(pGauge);
  
  sensesp_app->enable();

});
