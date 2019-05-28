#ifndef ANALOG_GAUGE_H
#define ANALOG_GAUGE_H

#include <DFRobot_Display.h>

#include <functional>
#include <stdint.h>
#include <system/valueconsumer.h>
#include <system/configurable.h>
#include <system/enable.h>
#include <system/valueproducer.h>


/**
 * An analog gauge that displays on an LCD.  The gauge supports up to five inputs
 * with each input having a specialized display suffix.
 *   If a BooleanProducer is attached to the gauge, the gauge can scroll thru 
 * five inputs on the digital display.  The analog dial will always display 
 * the incomming value of input 0.
 */
class AnalogGauge : public NumericConsumer, public BooleanConsumer,
                    public Configurable, public Enable {

    public:
        AnalogGauge(DFRobot_Display* pDisplay,
                    double minVal = 0.0, double maxVal = 100.0,
                    String config_path = "");

        virtual void enable() override;

        /**
         * Handles input of new values to display in digital portion.
         */
        virtual void set_input(float newValue, uint8_t inputChannel = 0) override;

        /**
         * Handles button presses
         */
        virtual void set_input(bool newValue, uint8_t inputChannel = 0) override;

        void setGaugeIcon(uint8_t* pNewIcon) { pGaugeIcon = pNewIcon; } 

        void setValueSuffix(char suffix, int inputChannel);

        void setDefaultDisplayIndex(int inputChannel) { currentDisplayChannel = inputChannel; }

        // For reading and writing the configuration
        virtual JsonObject& get_configuration(JsonBuffer& buf) override;
        virtual bool set_configuration(const JsonObject& config) override;

    private:
        double minVal;
        double maxVal;
        double valueRange;

        double lastDialValue = -99.9;
        double lastDisplayValue = -99.9;
        double* input;
        char* valueSuffix;
        int currentDisplayChannel;
        int maxDisplayChannel;
        bool ipDisplayed = false;


        int blinkCount = 0;
        int wifiIcon = -1;
        uint8_t* pGaugeIcon;

        DFRobot_Display& display;

        void drawDisplay();
        void updateGauge();
        void updateWifiStatus();

        void calcPoint(double pct, uint16_t radius, int16_t& x, int16_t& y);
        void drawHash(double pct, uint16_t startRadius, uint16_t endRadius, uint16_t color);
        void drawGaugeRange(double minPct, double maxPct, double inc, uint16_t color);
        void drawGaugeTick(double pct, uint16_t startRadius, uint16_t endRadius, uint16_t color);
        void drawNeedle(double value, uint16_t color);

};

#endif