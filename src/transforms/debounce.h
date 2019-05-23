#ifndef debounce_H
#define debounce_H


#include "transforms/transform.h"

/**
 * Debounce is a boolean passthrough transform that will only
 * accept value changes that are sufficiently spaced apart to
 * be passed on to the next consumer.
 */
class Debounce : public OneToOneBooleanTransform {

    public:
        Debounce(int msMinDelay = 200, String sk_path="", String config_id="", String schema="");

        virtual void set_input(bool newValue, uint8_t inputChannel = 0) override;

    protected:
        unsigned long lastTime;
        int msMinDelay;
};

#endif