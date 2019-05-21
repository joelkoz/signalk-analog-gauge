#ifndef _voltage_divider_H
#define _voltage_divider_H

#include "transforms/transform.h"


/**
 * A transform that uses the voltage divider formula to resolve the resistance
 * of R2 in the circuit, given a fixed voltage sent into the divider (Vin),
 * a known resistance an the junction of the two resistors (R1), and the
 * output of the divider (Vout).  This is the typical way a sensor such as
 * a temperature sender or oil pressure sender is wired up: the sender's resistance
 * varies based on temperature or pressure, so it is usually "R2" in a voltage
 * divider.  R1 and/or Vin is selected in such a way to ensure Vout (the value
 * read by the MCU's analog input) does not exceed 3.3 volts.
 */
class VoltageDividerR2 : public OneToOneTransform<float> {

    public:
        VoltageDividerR2(float R1, float Vin = 3.3, String sk_path="", String id="", String schema="");

        virtual void set_input(float Vout, uint8_t ignored = 0) override;

        // For outputting the results as SignalK
        String as_json() override;

        // For reading and writing the configuration of this transformation
        virtual JsonObject& get_configuration(JsonBuffer& buf) override;
        virtual bool set_configuration(const JsonObject& config) override;

    protected:
        float R1;
        float Vin;

};

#endif
