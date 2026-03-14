#pragma once

#include "MotorDriver.h"

class BTS7960Driver : public MotorDriver {
public:
    BTS7960Driver(uint8_t rpwmPin, uint8_t lpwmPin,
                  uint8_t renPin,  uint8_t lenPin,
                  uint8_t risPin,  uint8_t lisPin);

    void begin() override;
    void setSpeed(uint8_t speed, bool forward) override;
    void stop() override;
    void brake() override;
    uint16_t getCurrentSense() override;
    void enable() override;
    void disable() override;

private:
    uint8_t _rpwmPin, _lpwmPin;
    uint8_t _renPin,  _lenPin;
    uint8_t _risPin,  _lisPin;
    bool _enabled = false;
};
