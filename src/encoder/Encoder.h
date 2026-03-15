#pragma once

#include <Arduino.h>
#include <driver/pcnt.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint16_t cpr);

    void begin();
    void update();              // Call periodically (~100ms) for RPM calculation

    int32_t  getPosition();
    void     resetPosition();
    void     setPosition(int32_t pos);

    float    getRPM() const { return _rpm; }
    bool     isConnected() const { return _detected; }

private:
    static void IRAM_ATTR overflowISR(void* arg);

    uint8_t  _pinA, _pinB;
    uint16_t _cpr;
    pcnt_unit_t _unit;

    volatile int32_t _overflowCount;
    int32_t  _positionOffset;

    // RPM
    int32_t  _lastPosition;
    uint32_t _lastRpmTime;
    float    _rpm;

    bool     _detected;
};
