#include "DoorController.h"
#include "config/Config.h"
#include <Preferences.h>

static Preferences prefs;

// ---------------------------------------------------------
#if ENCODER_ENABLED
DoorController::DoorController(MotorDriver& motor,
                               uint8_t endstopOpenPin,
                               uint8_t endstopClosePin,
                               Encoder* encoder)
    : _motor(motor)
    , _endstopOpenPin(endstopOpenPin)
    , _endstopClosePin(endstopClosePin)
    , _encoder(encoder) {}
#else
DoorController::DoorController(MotorDriver& motor,
                               uint8_t endstopOpenPin,
                               uint8_t endstopClosePin)
    : _motor(motor)
    , _endstopOpenPin(endstopOpenPin)
    , _endstopClosePin(endstopClosePin) {}
#endif

// ---------------------------------------------------------
void DoorController::begin() {
    pinMode(_endstopOpenPin,  INPUT_PULLUP);
    pinMode(_endstopClosePin, INPUT_PULLUP);

    _motor.begin();

    delay(50);
    _endstopOpen  = (digitalRead(_endstopOpenPin)  == LOW);
    _endstopClose = (digitalRead(_endstopClosePin) == LOW);
    _rawEndstopOpen  = _endstopOpen;
    _rawEndstopClose = _endstopClose;

#if ENCODER_ENABLED
    if (_encoder) {
        _encoder->begin();
        loadCalibration();

        if (_endstopClose) {
            _encoder->resetPosition();
        } else if (_endstopOpen && _calibrated) {
            _encoder->setPosition(_positionOpen);
        }
    }
#endif

    if (_endstopClose)          _state = DoorState::CLOSED;
    else if (_endstopOpen)      _state = DoorState::OPEN;
    else                        _state = DoorState::STOPPED;

    Serial.printf("[Door] Initial state: %s", getStateString());
#if ENCODER_ENABLED
    if (_encoder)
        Serial.printf(", encoder: %s, calibrated: %s",
                      _encoder->isConnected() ? "yes" : "detecting",
                      _calibrated ? "yes" : "no");
#endif
    Serial.println();
}

// ---------------------------------------------------------
void DoorController::update() {
    checkEndstops();

#if ENCODER_ENABLED
    if (_encoder) {
        _encoder->update();

        // Auto-detect encoder during first movement
        if (!_encoderDetected && _encoder->isConnected()) {
            _encoderDetected = true;
            Serial.println("[Door] Encoder detected");
        }
    }

    if (_state == DoorState::CALIBRATING) {
        updateCalibration();
        return;
    }
#endif

    if (_state != DoorState::OPENING && _state != DoorState::CLOSING)
        return;

    updateRamp();
    checkCurrent();
    checkTimeout();

#if ENCODER_ENABLED
    if (_encoder && _encoderDetected && _calibrated)
        checkPositionLimits();
#endif

    // Endstop-based stop (always active as safety fallback)
    if (_state == DoorState::OPENING && _endstopOpen) {
        _motor.stop();
        _currentSpeed = 0;
        syncEncoderAtEndstop(true);
        transitionTo(DoorState::OPEN);
    } else if (_state == DoorState::CLOSING && _endstopClose) {
        _motor.stop();
        _currentSpeed = 0;
        syncEncoderAtEndstop(false);
        transitionTo(DoorState::CLOSED);
    }
}

// ---------------------------------------------------------
void DoorController::open() {
    if (_state == DoorState::OPEN || _state == DoorState::OPENING) return;
    if (_state == DoorState::ERROR || _state == DoorState::CALIBRATING) return;

    if (_endstopOpen) { transitionTo(DoorState::OPEN); return; }

    if (_state == DoorState::CLOSING) {
        _motor.stop();
        delay(100);
    }

    _currentSpeed   = 0;
    _moveStartTime  = millis();
    _encoderDetectStart = millis();
    _motor.enable();
    transitionTo(DoorState::OPENING);
}

void DoorController::close() {
    if (_state == DoorState::CLOSED || _state == DoorState::CLOSING) return;
    if (_state == DoorState::ERROR || _state == DoorState::CALIBRATING) return;

    if (_endstopClose) { transitionTo(DoorState::CLOSED); return; }

    if (_state == DoorState::OPENING) {
        _motor.stop();
        delay(100);
    }

    _currentSpeed   = 0;
    _moveStartTime  = millis();
    _encoderDetectStart = millis();
    _motor.enable();
    transitionTo(DoorState::CLOSING);
}

void DoorController::stop() {
    _motor.stop();
    _currentSpeed = 0;
    if (_state == DoorState::OPENING || _state == DoorState::CLOSING ||
        _state == DoorState::CALIBRATING)
        transitionTo(DoorState::STOPPED);
}

void DoorController::reset() {
    _motor.stop();
    _currentSpeed  = 0;
    _errorMessage  = "";
    transitionTo(DoorState::STOPPED);
}

// ---------------------------------------------------------
// Calibration
// ---------------------------------------------------------
void DoorController::calibrate() {
#if ENCODER_ENABLED
    if (!_encoder) return;
    Serial.println("[Door] Starting calibration...");
    _motor.stop();
    _currentSpeed = 0;
    _calibPhase = CalibPhase::CLOSING_TO_ZERO;
    _calibStartTime = millis();
    _moveStartTime = millis();
    _motor.enable();
    transitionTo(DoorState::CALIBRATING);
#endif
}

void DoorController::clearCalibration() {
    _calibrated = false;
    _positionOpen = 0;
    _totalRange = 0;
    prefs.begin("door", false);
    prefs.clear();
    prefs.end();
    Serial.println("[Door] Calibration cleared");
}

#if ENCODER_ENABLED
void DoorController::updateCalibration() {
    checkEndstops();
    checkCurrent();

    if (millis() - _calibStartTime > MOTOR_TIMEOUT_MS * 2) {
        _motor.stop();
        _currentSpeed = 0;
        _errorMessage = "Calibration timed out";
        _calibPhase = CalibPhase::IDLE;
        transitionTo(DoorState::ERROR);
        return;
    }

    switch (_calibPhase) {
        case CalibPhase::CLOSING_TO_ZERO:
            _motor.setSpeed(CALIBRATION_SPEED, false);
            _currentSpeed = CALIBRATION_SPEED;
            if (_endstopClose) {
                _motor.stop();
                _currentSpeed = 0;
                _encoder->resetPosition();
                _positionClosed = 0;
                Serial.println("[Door] Calibration: zero position set");
                _calibPhase = CalibPhase::PAUSE;
                _calibStartTime = millis();
            }
            break;

        case CalibPhase::PAUSE:
            if (millis() - _calibStartTime > 500) {
                _calibPhase = CalibPhase::OPENING_TO_MAX;
                _calibStartTime = millis();
                _moveStartTime = millis();
            }
            break;

        case CalibPhase::OPENING_TO_MAX:
            _motor.setSpeed(CALIBRATION_SPEED, true);
            _currentSpeed = CALIBRATION_SPEED;
            if (_endstopOpen) {
                _motor.stop();
                _currentSpeed = 0;
                _positionOpen = _encoder->getPosition();
                _totalRange = _positionOpen - _positionClosed;
                _calibrated = true;
                _encoderDetected = true;
                saveCalibration();
                Serial.printf("[Door] Calibration complete: range = %d pulses\n", _totalRange);
                _calibPhase = CalibPhase::IDLE;
                transitionTo(DoorState::OPEN);
            }
            break;

        default:
            break;
    }
}
#endif

void DoorController::checkPositionLimits() {
#if ENCODER_ENABLED
    if (!_encoder || !_calibrated) return;

    int32_t pos = _encoder->getPosition();
    int32_t margin = _totalRange / 20;  // 5% margin for deceleration

    if (_state == DoorState::OPENING && pos >= _positionOpen - margin) {
        if (pos >= _positionOpen) {
            _motor.stop();
            _currentSpeed = 0;
            transitionTo(DoorState::OPEN);
        }
    } else if (_state == DoorState::CLOSING && pos <= _positionClosed + margin) {
        if (pos <= _positionClosed) {
            _motor.stop();
            _currentSpeed = 0;
            transitionTo(DoorState::CLOSED);
        }
    }
#endif
}

void DoorController::syncEncoderAtEndstop(bool isOpen) {
#if ENCODER_ENABLED
    if (!_encoder) return;
    if (isOpen && _calibrated) {
        _encoder->setPosition(_positionOpen);
    } else if (!isOpen) {
        _encoder->resetPosition();
    }
#endif
}

void DoorController::loadCalibration() {
    prefs.begin("door", true);
    _calibrated = prefs.getBool("cal", false);
    _positionOpen = prefs.getInt("posOpen", 0);
    prefs.end();
    if (_calibrated) {
        _positionClosed = 0;
        _totalRange = _positionOpen - _positionClosed;
        Serial.printf("[Door] Loaded calibration: range = %d pulses\n", _totalRange);
    }
}

void DoorController::saveCalibration() {
    prefs.begin("door", false);
    prefs.putBool("cal", true);
    prefs.putInt("posOpen", _positionOpen);
    prefs.end();
}

// ---------------------------------------------------------
// Accessors
// ---------------------------------------------------------
float DoorController::getPositionPercent() const {
#if ENCODER_ENABLED
    if (!_encoder || !_calibrated || _totalRange == 0) return -1.0f;
    int32_t pos = _encoder->getPosition() - _positionClosed;
    return constrain((float)pos / _totalRange * 100.0f, 0.0f, 100.0f);
#else
    return -1.0f;
#endif
}

float DoorController::getRPM() const {
#if ENCODER_ENABLED
    if (!_encoder) return 0;
    return _encoder->getRPM();
#else
    return 0;
#endif
}

bool DoorController::hasEncoder() const {
#if ENCODER_ENABLED
    return _encoder != nullptr && _encoderDetected;
#else
    return false;
#endif
}

// ---------------------------------------------------------
const char* DoorController::stateToString(DoorState s) {
    switch (s) {
        case DoorState::CLOSED:      return "CLOSED";
        case DoorState::OPENING:     return "OPENING";
        case DoorState::OPEN:        return "OPEN";
        case DoorState::CLOSING:     return "CLOSING";
        case DoorState::STOPPED:     return "STOPPED";
        case DoorState::CALIBRATING: return "CALIBRATING";
        case DoorState::ERROR:       return "ERROR";
        default:                     return "UNKNOWN";
    }
}

const char* DoorController::getStateString() const {
    return stateToString(_state);
}

void DoorController::transitionTo(DoorState newState) {
    Serial.printf("[Door] %s -> %s\n", stateToString(_state), stateToString(newState));
    _state = newState;
}

// ---------------------------------------------------------
void DoorController::updateRamp() {
    uint32_t elapsed = millis() - _moveStartTime;

    uint8_t target = (elapsed < _rampTimeMs)
        ? map(elapsed, 0, _rampTimeMs, 0, _maxSpeed)
        : _maxSpeed;

    if (target != _currentSpeed) {
        _currentSpeed = target;
        _motor.setSpeed(_currentSpeed, _state == DoorState::OPENING);
    }
}

void DoorController::checkEndstops() {
    uint32_t now = millis();

    bool rawOpen = (digitalRead(_endstopOpenPin) == LOW);
    if (rawOpen != _rawEndstopOpen) {
        _lastDebounceOpen = now;
        _rawEndstopOpen   = rawOpen;
    }
    if ((now - _lastDebounceOpen) > DEBOUNCE_MS)
        _endstopOpen = _rawEndstopOpen;

    bool rawClose = (digitalRead(_endstopClosePin) == LOW);
    if (rawClose != _rawEndstopClose) {
        _lastDebounceClose = now;
        _rawEndstopClose   = rawClose;
    }
    if ((now - _lastDebounceClose) > DEBOUNCE_MS)
        _endstopClose = _rawEndstopClose;
}

void DoorController::checkCurrent() {
    uint16_t current = _motor.getCurrentSense();
    if (current > CURRENT_THRESHOLD) {
        Serial.printf("[Door] Overcurrent: %d\n", current);
        _motor.stop();
        _currentSpeed  = 0;
        _errorMessage  = "Overcurrent - possible obstruction";
        transitionTo(DoorState::ERROR);
    }
}

void DoorController::checkTimeout() {
    if ((millis() - _moveStartTime) > _timeoutMs) {
        Serial.println("[Door] Timeout");
        _motor.stop();
        _currentSpeed  = 0;
        _errorMessage  = "Operation timed out";
        transitionTo(DoorState::ERROR);
    }
}
