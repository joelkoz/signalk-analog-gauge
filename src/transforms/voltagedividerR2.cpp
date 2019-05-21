#include "voltagedividerR2.h"

VoltageDividerR2::VoltageDividerR2(float R1, float Vin, String sk_path, String id, String schema) :
   OneToOneTransform<float>(sk_path, id, schema), R1{R1}, Vin{Vin} {
}

void VoltageDividerR2::set_input(float Vout, uint8_t ignored) {
    output = (Vout * R1) / (Vin - Vout);
    notify();
}

String VoltageDividerR2::as_json() {
  DynamicJsonBuffer jsonBuffer;
  String json;
  JsonObject& root = jsonBuffer.createObject();
  root.set("path", this->sk_path);
  root.set("value", output);
  root.printTo(json);
  return json;
}


JsonObject& VoltageDividerR2::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["sk_path"] = sk_path;
  root["Vin"] = Vin;
  root["R1"] = R1;
  return root;
}


bool VoltageDividerR2::set_configuration(const JsonObject& config) {

  String expected[] = { "sk_path", "Vin", "R1" };
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      debugE("Can not set VoltageDividerR2: configuration: missing json field %s\n", str.c_str());
      return false;
    }
  }

  sk_path = config["sk_path"].as<String>();
  Vin = config["Vin"];
  R1 = config["R1"];

  return true;

}
