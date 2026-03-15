#pragma once

#include <Arduino.h>
#include "config/Config.h"
#include "motor/MotorDriver.h"

#if ENCODER_ENABLED
#include "encoder/Encoder.h"
#endif

enum class DoorState {
    CLOSED,
    OPENING,
    OPEN,
    CLOSING,
    STOPPED,
    CALIBRATING,
    ERROR
};

class DoorController {
public:
#if ENCODER_ENABLED
    DoorController(MotorDriver& motor,
                   uint8_t endstopOpenPin, uint8_t endstopClosePin,
                   Encoder* encoder = nullptr);
#else
    DoorController(MotorDriver& motor,
                   uint8_t endstopOpenPin, uint8_t endstopClosePin);
#endif

    void begin();
    void update();

    void open();
    void close();
    void stop();
    void reset();
    void calibrate();
    void clearCalibration();

    DoorState           getState()           const { return _state; }
    const char*         getStateString()     const;
    uint8_t             getCurrentSpeed()    const { return _currentSpeed; }
    const String&       getErrorMessage()    const { return _errorMessage; }
    bool                isCalibrated()       const { return _calibrated; }
    float               getPositionPercent() const;
    float               getRPM()             const;
    bool                hasEncoder()         const;

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

    // Encoder-related
    void updateCalibration();
    void checkPositionLimits();
    void syncEncoderAtEndstop(bool isOpen);
    void loadCalibration();
    void saveCalibration();

    MotorDriver& _motor;
    uint8_t _endstopOpenPin;
    uint8_t _endstopClosePin;

#if ENCODER_ENABLED
    Encoder* _encoder;
#endif

    DoorState _state            = DoorState::STOPPED;
    uint8_t   _currentSpeed     = 0;
    uint8_t   _maxSpeed         = MOTOR_MAX_SPEED;
    uint16_t  _rampTimeMs       = MOTOR_RAMP_TIME_MS;
    uint32_t  _timeoutMs        = MOTOR_TIMEOUT_MS;

    uint32_t _moveStartTime     = 0;

    // Endstop debouncing
    uint32_t _lastDebounceOpen  = 0;
    uint32_t _lastDebounceClose = 0;
    bool _rawEndstopOpen        = false;
    bool _rawEndstopClose       = false;
    bool _endstopOpen           = false;
    bool _endstopClose          = false;

    // Encoder / calibration
    bool     _calibrated        = false;
    bool     _encoderDetected   = false;
    int32_t  _positionClosed    = 0;
    int32_t  _positionOpen      = 0;
    int32_t  _totalRange        = 0;
    uint32_t _encoderDetectStart = 0;

    enum class CalibPhase { IDLE, CLOSING_TO_ZERO, PAUSE, OPENING_TO_MAX };
    CalibPhase _calibPhase      = CalibPhase::IDLE;
    uint32_t   _calibStartTime  = 0;

    String _errorMessage;
};
