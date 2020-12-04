#ifndef ANALOG_GAUGE_H
#define ANALOG_GAUGE_H

#include <set>

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
        class ValueColor {
            public:
               float minVal;
               float maxVal;
               uint16_t color;

               ValueColor();
               ValueColor(float minVal, float maxVal, uint16_t color);
               ValueColor(JsonObject obj);

               friend bool operator<(const ValueColor& lhs, const ValueColor& rhs) { return lhs.minVal < rhs.minVal; }               
        };

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

        void setValueSuffix(char suffix, int inputChannel = 0);

        void setPrecision(uint8 precision, int inputChannel = 0);

        void setDefaultDisplayIndex(int inputChannel) { currentDisplayChannel = inputChannel; }


        /**
         * Value ranges allow for setting gauge colors like "normal operating range"
         * and "red line" values.
         */
        void addValueRange(const ValueColor& newRange);

        /**
         * Returns the color associated with the specified value. If no specific color
         * range can be found, the default color is returned.
         * @see addValueRange()
         * @see setDefaultValueColor
         */
        uint16_t getValueColor(float value);


        /**
         * Sets the default color for the display when a value range is requested an no specified
         * value range color can be found.
         */
        void setDefaultValueColor(uint16_t newDefaultColor) { defaultValueColor = newDefaultColor; }


        uint16_t getDefaultValueColor() { return defaultValueColor; }

        // For reading and writing the configuration
        virtual void get_configuration(JsonObject& doc) override;
        virtual bool set_configuration(const JsonObject& config) override;
        virtual String get_config_schema() override;
        
    private:
        double minVal;
        double maxVal;
        double valueRange;

        double lastDialValue = -99.9;
        double lastDisplayValue = -99.9;
        double* input;
        char* valueSuffix;
        uint8* precision;
        int currentDisplayChannel;
        int maxDisplayChannel;
        bool ipDisplayed = false;

        uint16_t defaultValueColor = DISPLAY_WHITE;

        int blinkCount = 0;
        int displayIcon = -1;
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

        std::set<ValueColor> valueColors;
};

#endif