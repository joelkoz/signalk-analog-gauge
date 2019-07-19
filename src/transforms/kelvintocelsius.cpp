#include "kelvintocelsius.h"

KelvinToCelsius::KelvinToCelsius(String config_path) :
   NumericTransform(config_path) {
}


void KelvinToCelsius::set_input(float K, uint8_t inputChannel) {
    output = K - 273.15;
    notify();
}
