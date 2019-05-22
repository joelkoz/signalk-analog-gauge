#include "transform/interpolate.h"
#include "ReactESP.h"

float getValue(Interpolate& i, float input) {
   i.set_input(input);
   return i.get();
}


void testVal(Interpolate& i, float input) {
    float output = getValue(i, input);
    printf("Value of %.2f is %.3f", input, output);
}


int main(int argc, char **argv) {

    Interpolate i("someCustomSK");

    i.addSample(Interpolate::Sample(5, 40));
    i.addSample(Interpolate::Sample(15, 30));
    i.addSample(Interpolate::Sample(20, 25));
    i.addSample(Interpolate::Sample(30, 10));
    i.addSample(Interpolate::Sample(40, 1));

    for (int input = 5; input < 40; input++) {
        printf("\n\nTesting input of %d\n", input);
        testVal(i, input);
    }

    StaticJsonBuffer<1000> buf;
    JsonObject& jObj = i.get_configuration(buf);

    String json;
    jObj.printTo(json);

    printf("\n json is:\n%s\n", json.c_str());


    Interpolate i2("");
    i2.set_configuration(jObj);

    StaticJsonBuffer<1000> buf2;
    JsonObject& jObj2 = i2.get_configuration(buf2);
    String json2;
    jObj2.printTo(json2);

    printf("\n json2 is:\n%s\n", json2.c_str());

    return 0;
}
