#ifndef _kelvinToFahrenheit_H
#define _kelvinToFahrenheit_H

#include "transforms/transform.h"

class KelvinToFahrenheit : public NumericTransform {

    public:
       KelvinToFahrenheit(String config_path="");

        virtual void set_input(float K, uint8_t inputChannel = 0) override;
};


#endif
