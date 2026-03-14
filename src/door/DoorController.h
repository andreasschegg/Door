#pragma once

#include <Arduino.h>
#include "motor/MotorDriver.h"

enum class DoorState {
    CLOSED,
    OPENING,
    OPEN,
    CLOSING,
    STOPPED,
    ERROR
};

class DoorController {
public:
    DoorController(MotorDriver& motor,
                   uint8_t endstopOpenPin, uint8_t endstopClosePin);

    void begin();
    void update();      // Call every loop() iteration

    void open();
    void close();
    void stop();
    void reset();       // Clear error, transition to STOPPED

    DoorState           getState()        const { return _state; }
    const char*         getStateString()  const;
    uint8_t             getCurrentSpeed() const { return _currentSpeed; }
    const String&       getErrorMessage() const { return _errorMessage; }

    void setMaxSpeed(uint8_t speed)     { _maxSpeed   = speed; }
    void setRampTimeMs(uint16_t ms)     { _rampTimeMs = ms; }
    void setTimeoutMs(uint32_t ms)      { _timeoutMs  = ms; }

private:
    static const char* stateToString(DoorState s);

    void updateRamp();
    void checkEndstops();
    void checkCurrent();
    void checkTimeout();
    void transitionTo(DoorState newState);

    MotorDriver& _motor;
    uint8_t _endstopOpenPin;
    uint8_t _endstopClosePin;

    DoorState _state            = DoorState::STOPPED;
    uint8_t   _currentSpeed     = 0;
    uint8_t   _maxSpeed         = 255;
    uint16_t  _rampTimeMs       = 800;
    uint32_t  _timeoutMs        = 15000;

    uint32_t _moveStartTime     = 0;

    // Endstop debouncing
    uint32_t _lastDebounceOpen  = 0;
    uint32_t _lastDebounceClose = 0;
    bool _rawEndstopOpen        = false;
    bool _rawEndstopClose       = false;
    bool _endstopOpen           = false;
    bool _endstopClose          = false;

    String _errorMessage;
};
