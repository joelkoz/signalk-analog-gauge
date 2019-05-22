#ifndef _filteredpassthrough_H
#define _filteredpassthrough_H


#include "transforms/passthrough.h"


/**
 * A filter that allows only values destined for a certain input channel to
 * be passed through.
 */
template <typename T>
class FilteredPassthrough : public Passthrough<T> {

    public:
        FilteredPassthrough(int allowedChannel = 0, String sk_path="", String config_id="", String schema="") :
            Passthrough<T>(sk_path, config_id, schema), allowedChannel{allowedChannel} {
        }

        virtual void set_input(T newValue, uint8_t valueIdx = 0) override {
            if (valueIdx == allowedChannel) {
                ValueProducer<T>::output = newValue;
                this->notify();
            }
        }

    protected:
        int allowedChannel;
};


#define NumericFilteredPassthrough FilteredPassthrough<float>
#define IntegerFilteredPassthrough FilteredPassthrough<int>
#define BooleanFilteredPassthrough FilteredPassthrough<bool>
#define StringFilteredPassthrough FilteredPassthrough<String>

#endif