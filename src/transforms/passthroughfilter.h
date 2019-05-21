#ifndef passthroughfilter_H
#define passthroughfilter_H


#include "transforms/transform.h"


/**
 * A filter that allows only values destined for a certain input channel to
 * be passed through.
 */
template <typename T>
class PassthroughFilter : public OneToOneTransform<T> {

    public:
        PassthroughFilter(int allowedChannel = 0, String sk_path="", String config_id="", String schema="") :
            OneToOneTransform<T>(sk_path, config_id, schema), allowedChannel{allowedChannel} {
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


#define NumericPassthroughFilter PassthroughFilter<float>
#define IntegerPassthroughFilter PassthroughFilter<int>
#define BooleanPassthroughFilter PassthroughFilter<bool>
#define StringPassthroughFilter PassthroughFilter<String>

#endif