#ifndef ANALOG_GAUGE_H
#define ANALOG_GAUGE_H

#include <DFRobot_Display.h>

#include <functional>
#include <stdint.h>
#include <system/valueconsumer.h>
#include <system/configurable.h>
#include <system/wantsenable.h>
#include <system/valueproducer.h>

class AnalogGauge : public NumericConsumer, public Configurable, public WantsEnable {

    public:
        AnalogGauge(DFRobot_Display* pDisplay, NumericProducer* pdDisplaySource,
                    double minVal = 0.0, double maxVal = 100.0,
                    String config_id = "", String schema = "");

        virtual void enable() override;

        virtual void set_input(float newValue, uint8_t idx = 0) override;

    private:
       double minVal;
       double maxVal;
       double valueRange;
       double lastValue = -99.9;

       int blinkCount = 0;
       int wifiIcon = -1;

       DFRobot_Display& display;

        void drawDisplay();
        void updateGauge(float newValue);
        void updateWifiStatus();

        void calcPoint(double pct, uint16_t radius, int16_t& x, int16_t& y);
        void drawHash(double pct, uint16_t startRadius, uint16_t endRadius, uint16_t color);
        void drawGaugeRange(double minPct, double maxPct, double inc, uint16_t color);
        void drawGaugeTick(double pct, uint16_t startRadius, uint16_t endRadius, uint16_t color);
        void drawNeedle(double value, uint16_t color);

};

#endif