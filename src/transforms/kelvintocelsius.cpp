#include "kelvintocelsius.h"

KelvinToCelsius::KelvinToCelsius(String sk_path, String config_path) :
   SymmetricNumericTransform(sk_path, config_path) {
}


void KelvinToCelsius::set_input(float K, uint8_t inputChannel) {
    output = K - 273.15;
    notify();
}
