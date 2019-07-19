#include "kelvintofahrenheit.h"

KelvinToFahrenheit::KelvinToFahrenheit(String sk_path, String config_path) :
   SymmetricNumericTransform(sk_path, config_path) {
}


void KelvinToFahrenheit::set_input(float K, uint8_t inputChannel) {
    output = 9.0 / 5.0 * (K - 273.15) + 32.0;
    notify();
}
