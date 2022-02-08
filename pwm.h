class T_PWM {
  public:
    int pin = 2;
    String name;
    uint16_t val, min, max;


    T_PWM(int pin, String name);
    String toString(void);
    String toJSON(void);
    void setVal(uint8_t val);
    uint8_t toPWM(uint8_t val);
    uint8_t toVAL(uint8_t pwm);
};
