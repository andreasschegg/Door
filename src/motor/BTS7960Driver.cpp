#include "BTS7960Driver.h"
#include "config/Config.h"

BTS7960Driver::BTS7960Driver(uint8_t rpwmPin, uint8_t lpwmPin,
                             uint8_t renPin,  uint8_t lenPin,
                             uint8_t risPin,  uint8_t lisPin)
    : _rpwmPin(rpwmPin), _lpwmPin(lpwmPin)
    , _renPin(renPin),    _lenPin(lenPin)
    , _risPin(risPin),    _lisPin(lisPin) {}

void BTS7960Driver::begin() {
    pinMode(_renPin, OUTPUT);
    pinMode(_lenPin, OUTPUT);

    // LEDC PWM setup
    ledcSetup(PWM_CHANNEL_RPWM, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_LPWM, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(_rpwmPin, PWM_CHANNEL_RPWM);
    ledcAttachPin(_lpwmPin, PWM_CHANNEL_LPWM);

    // Current sense — input-only ADC pins
    pinMode(_risPin, INPUT);
    pinMode(_lisPin, INPUT);

    stop();
    enable();
}

void BTS7960Driver::setSpeed(uint8_t speed, bool forward) {
    if (!_enabled) return;

    if (forward) {
        ledcWrite(PWM_CHANNEL_LPWM, 0);
        ledcWrite(PWM_CHANNEL_RPWM, speed);
    } else {
        ledcWrite(PWM_CHANNEL_RPWM, 0);
        ledcWrite(PWM_CHANNEL_LPWM, speed);
    }
}

void BTS7960Driver::stop() {
    ledcWrite(PWM_CHANNEL_RPWM, 0);
    ledcWrite(PWM_CHANNEL_LPWM, 0);
}

void BTS7960Driver::brake() {
    ledcWrite(PWM_CHANNEL_RPWM, 255);
    ledcWrite(PWM_CHANNEL_LPWM, 255);
}

uint16_t BTS7960Driver::getCurrentSense() {
    uint16_t r = analogRead(_risPin);
    uint16_t l = analogRead(_lisPin);
    return max(r, l);
}

void BTS7960Driver::enable() {
    digitalWrite(_renPin, HIGH);
    digitalWrite(_lenPin, HIGH);
    _enabled = true;
}

void BTS7960Driver::disable() {
    stop();
    digitalWrite(_renPin, LOW);
    digitalWrite(_lenPin, LOW);
    _enabled = false;
}
