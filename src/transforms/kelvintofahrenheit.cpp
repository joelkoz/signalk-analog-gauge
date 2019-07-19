#include "kelvintofahrenheit.h"

KelvinToFahrenheit::KelvinToFahrenheit(String config_path) :
   NumericTransform(config_path) {
}


void KelvinToFahrenheit::set_input(float K, uint8_t inputChannel) {
    output = 9.0 / 5.0 * (K - 273.15) + 32.0;
    notify();
}
