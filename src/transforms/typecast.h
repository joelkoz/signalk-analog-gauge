#ifndef _typecast_H_
#define _typecast_H_

#include <functional>

#include "transforms/transform.h"

template <typename C, typename P>
class Typecast : public Transform<C, P> {

    public:
       Typecast(std::function<P(C)> cast) : Transform<C, P>(""), cast_{cast} {}

       virtual void set_input(C new_value, uint8_t input_channel = 0) override {
           this->emit(cast_(new_value));
       }

    private:
       std::function<P(C)> cast_;
};
 

class IntToBool : public Typecast<int, bool> {

    public:
       IntToBool();
};


class  BoolToInt : public Typecast<bool, int> {
    public:
       BoolToInt();
};


#endif
