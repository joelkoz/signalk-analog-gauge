#include <Arduino.h>

#include "sensesp_app.h"
#include "sensesp_app_builder.h"
#include "transforms/linear.h"
#include "sensors/analog_input.h"
#include "sensors/digital_input.h"

#include "DFRobot_ST7687S_Latch.h"

#include "displays/AnalogGauge.h"
#include "transforms/analogvoltage.h"
#include "transforms/voltagedivider.h"
#include "transforms/temperatureinterpreter.h"
#include "transforms/kelvintocelsius.h"
#include "transforms/kelvintofahrenheit.h"
#include "transforms/debounce.h"
#include "transforms/moving_average.h"
#include "transforms/change_filter.h"
#include "transforms/median.h"
#include "signalk/signalk_output.h"
#include "transforms/transform.h"
#include "transforms/typecast.h"

const char* sk_path = "electrical.generator.engine.coolantTemperature";


const float Vin = 5; // Voltage sent into the voltage divider circuit that includes the analog sender
const float R1 = 5100.0; // The resistance, in Ohms, of the R1 resitor in the analog sender divider circuit


const float minAnalogGaugeVal = 327.594; // Minimum value to display on analog gauge
const float maxAnalogGaugeVal = 377.594; // Max value to display on analog gauge

// Wiring configuration and setup for TFT display
#define Display DFRobot_ST7687S_Latch
uint8_t pin_cs = D2, pin_rs = D1, pin_wr = D3, pin_lck = D4;
uint8_t button_pin = D8;

ReactESP app([] () {

#ifdef ARDUINO
  #ifndef SERIAL_DEBUG_DISABLED
  Serial.begin(115200);

  // A small arbitrary delay is required to let the
  // serial port catch up

  delay(1000);
  Debug.setSerialEnabled(true);
  #endif
#endif

  // Create a builder object
  SensESPAppBuilder builder;

  // Create the global SensESPApp() object.
  sensesp_app = builder.set_hostname("sk-gen-temp")
                    ->set_sk_server("your-sk-server.local", 3000)
                    ->set_wifi("YOUR WIFI SSID", "YOUR-WIFI-PASSWORD")
                    ->set_standard_sensors(StandardSensors::NONE)
                    ->get_app();

  SKEmitter::Metadata* metadata = new SKEmitter::Metadata();
  metadata->display_name_ = "Generator Temp";
  metadata->description_ = "Generator coolant temperature";
  metadata->units_ = "K";

  // Initialize the LCD display
  Display* pDisplay = new Display(pin_cs, pin_rs, pin_wr, pin_lck);
  pDisplay->begin();
  AnalogGauge *pGauge = new AnalogGauge(pDisplay, minAnalogGaugeVal, maxAnalogGaugeVal, "/gauge/display");
  pGauge->addValueRange(AnalogGauge::ValueColor(349.817, 360.928, DISPLAY_GREEN));
  pGauge->addValueRange(AnalogGauge::ValueColor(360.928, 369.261, DISPLAY_YELLOW));
  pGauge->addValueRange(AnalogGauge::ValueColor(369.261, maxAnalogGaugeVal, DISPLAY_RED));

  AnalogInput* pAnalogInput = new AnalogInput();
  DigitalInputValue* pButton = new DigitalInputValue(button_pin, INPUT, CHANGE);
  

  NumericTransform* pTempInKelvin;
  Linear* pVoltage;
  NumericTransform* pR2;

  pAnalogInput->connect_to(new Median(10, "/gauge/voltage/samples")) ->
                connect_to(new MovingAverage(5, 1.0, "/gauge/voltage/avg")) ->
                connect_to(new AnalogVoltage()) ->
                connect_to(pVoltage = new Linear(1.0, 0.0, "/gauge/voltage/calibrate")) ->
                connect_to(pR2 = new VoltageDividerR2(R1, Vin)) ->
                connect_to(new TemperatureInterpreter("/gauge/temp/curve")) ->
                connect_to(new Linear(0.974, 0.0, "/gauge/temp/calibrate")) ->
                connect_to(pTempInKelvin = new ChangeFilter(1, 10, 6, "/gauge/output/change")) ->
                connect_to(new SKOutputNumber(sk_path, "/gauge/output/sk", metadata)) ->
                connect_to(pGauge);
  pGauge->setValueSuffix('k', 0);


  pTempInKelvin->connect_to(new KelvinToFahrenheit()) -> connect_to(pGauge, 1);
  pGauge->setValueSuffix('f', 1);
  pGauge->setDefaultDisplayIndex(1);
  
  
  pTempInKelvin->connect_to(new KelvinToCelsius()) -> connect_to(pGauge, 2);
  pGauge->setValueSuffix('c', 2);
    
  pVoltage->connect_to(pGauge, 3);
  pGauge->setValueSuffix('v', 3);
  pGauge->setPrecision(3, 3);

  pR2->connect_to(pGauge, 4);
  pGauge->setValueSuffix('o', 4);
  pR2->connect_to(new ChangeFilter(1, 1000, 6))
       -> connect_to(new SKOutputNumber("", "/gauge/temp/sender/sk"));


  pButton->connect_to(new IntToBool()) ->
          connect_to(new Debounce()) ->
          connect_to(pGauge);

  sensesp_app->enable();

});
