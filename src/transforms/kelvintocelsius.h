#ifndef _kelvinToCelsius_H
#define _kelvinToCelsius_H

#include "transforms/transform.h"

class KelvinToCelsius : public NumericTransform {

    public:
       KelvinToCelsius(String config_path="");

        virtual void set_input(float K, uint8_t inputChannel = 0) override;
};


#endif
