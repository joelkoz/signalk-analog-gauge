#ifndef _temperatureinterpreter_h
#define _temperatureinterpreter_h

#include "transforms/curveinterpolator.h"

/**
 * A custom transform that takes a resistance value on ohms and returns the estimated 
 * temperature in Kelvin.  Sample data in this example were taken from a Westerbeke generator
 * temperature gauge.
 */
class TemperatureInterpreter : public CurveInterpolator {

    public:
        TemperatureInterpreter(String config_path="") :
           CurveInterpolator(NULL, config_path ) {

          // Populate a lookup table tp translate the ohm values returned by
          // our temperature sender to Kelvin
          clearSamples();
//          addSample(CurveInterpolator::Sample(knownOhmValue, knownKelvin));
          addSample(CurveInterpolator::Sample(0*1000.0, 408.15));
          addSample(CurveInterpolator::Sample(0.48*1000.0, 372.04));
          addSample(CurveInterpolator::Sample(0.51*1000.0, 369.26));
          addSample(CurveInterpolator::Sample(0.54*1000.0, 366.48));
          addSample(CurveInterpolator::Sample(0.60*1000.0, 363.71));
          addSample(CurveInterpolator::Sample(0.70*1000.0, 360.93));
          addSample(CurveInterpolator::Sample(0.75*1000.0, 359.82));
          addSample(CurveInterpolator::Sample(0.87*1000.0, 358.15));
          addSample(CurveInterpolator::Sample(0.92*1000.0, 355.37));
          addSample(CurveInterpolator::Sample(0.96*1000.0, 353.71));
          addSample(CurveInterpolator::Sample(1.0*1000.0, 352.59));
          addSample(CurveInterpolator::Sample(1.11*1000.0, 349.82));
          addSample(CurveInterpolator::Sample(1.19*1000.0, 347.04));
          addSample(CurveInterpolator::Sample(1.30*1000.0, 344.26));
          addSample(CurveInterpolator::Sample(1.40*1000.0, 341.48));
          addSample(CurveInterpolator::Sample(1.52*1000.0, 338.71));
          addSample(CurveInterpolator::Sample(1.62*1000.0, 335.93));
          addSample(CurveInterpolator::Sample(1.92*1000.0, 333.15));
          addSample(CurveInterpolator::Sample(2.18*1000.0, 330.37));
          addSample(CurveInterpolator::Sample(2.3*1000.0, 327.59));
          addSample(CurveInterpolator::Sample(2.53*1000.0, 324.81));
          addSample(CurveInterpolator::Sample(3.7*1000.0, 320.93));
          addSample(CurveInterpolator::Sample(4.45*1000.0, 312.54));
          addSample(CurveInterpolator::Sample(5.0*1000.0, 310.54));
          addSample(CurveInterpolator::Sample(5.47*1000.0, 304.37));
          addSample(CurveInterpolator::Sample(7.0*1000.0, 298.15));
          addSample(CurveInterpolator::Sample(10000, 273.15));
        }
};


#endif