#include <stdint.h>

T_PWM::T_PWM(int pin, String name) {
  this->pin = pin;
  this->val = 20;
  this->min = 0;
  this->max = 255;
  this->name  = name;
}

String T_PWM::toString(void) {
  return String("asd");
}

String T_PWM::toJSON(void) {
  String ret = "{\"name\":\"" + this->name + "\",\"pin\":" + this->pin + ",\"val\":" + this->val + "   }";

  return ret;
}

void T_PWM::setVal(uint8_t val) {
  this->val = val;
  //this->val = (this->val <= this->min) ? this->min : this->val;

  analogWrite(this->pin, (uint16_t)(this->max * this->val / 100));

  String asd = "{\"PWM\":" + this->toJSON() + "}";

  webSocket.broadcastTXT(asd);
}
