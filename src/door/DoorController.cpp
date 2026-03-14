#include "DoorController.h"
#include "config/Config.h"

// ---------------------------------------------------------
DoorController::DoorController(MotorDriver& motor,
                               uint8_t endstopOpenPin,
                               uint8_t endstopClosePin)
    : _motor(motor)
    , _endstopOpenPin(endstopOpenPin)
    , _endstopClosePin(endstopClosePin) {}

// ---------------------------------------------------------
void DoorController::begin() {
    pinMode(_endstopOpenPin,  INPUT_PULLUP);
    pinMode(_endstopClosePin, INPUT_PULLUP);

    _motor.begin();

    // Let pins settle, then read initial endstop state
    delay(50);
    _endstopOpen  = (digitalRead(_endstopOpenPin)  == LOW);
    _endstopClose = (digitalRead(_endstopClosePin) == LOW);
    _rawEndstopOpen  = _endstopOpen;
    _rawEndstopClose = _endstopClose;

    if (_endstopClose)     _state = DoorState::CLOSED;
    else if (_endstopOpen) _state = DoorState::OPEN;
    else                   _state = DoorState::STOPPED;

    Serial.printf("[Door] Initial state: %s\n", getStateString());
}

// ---------------------------------------------------------
void DoorController::update() {
    checkEndstops();

    if (_state != DoorState::OPENING && _state != DoorState::CLOSING)
        return;

    updateRamp();
    checkCurrent();
    checkTimeout();

    if (_state == DoorState::OPENING && _endstopOpen) {
        _motor.stop();
        _currentSpeed = 0;
        transitionTo(DoorState::OPEN);
    } else if (_state == DoorState::CLOSING && _endstopClose) {
        _motor.stop();
        _currentSpeed = 0;
        transitionTo(DoorState::CLOSED);
    }
}

// ---------------------------------------------------------
void DoorController::open() {
    if (_state == DoorState::OPEN || _state == DoorState::OPENING) return;
    if (_state == DoorState::ERROR) return;

    if (_endstopOpen) { transitionTo(DoorState::OPEN); return; }

    // If currently closing, stop motor briefly before reversing
    if (_state == DoorState::CLOSING) {
        _motor.stop();
        delay(100);
    }

    Serial.println("[Door] Opening...");
    _currentSpeed   = 0;
    _moveStartTime  = millis();
    _motor.enable();
    transitionTo(DoorState::OPENING);
}

void DoorController::close() {
    if (_state == DoorState::CLOSED || _state == DoorState::CLOSING) return;
    if (_state == DoorState::ERROR) return;

    if (_endstopClose) { transitionTo(DoorState::CLOSED); return; }

    if (_state == DoorState::OPENING) {
        _motor.stop();
        delay(100);
    }

    Serial.println("[Door] Closing...");
    _currentSpeed   = 0;
    _moveStartTime  = millis();
    _motor.enable();
    transitionTo(DoorState::CLOSING);
}

void DoorController::stop() {
    _motor.stop();
    _currentSpeed = 0;
    if (_state == DoorState::OPENING || _state == DoorState::CLOSING)
        transitionTo(DoorState::STOPPED);
}

void DoorController::reset() {
    _motor.stop();
    _currentSpeed  = 0;
    _errorMessage  = "";
    transitionTo(DoorState::STOPPED);
}

// ---------------------------------------------------------
const char* DoorController::stateToString(DoorState s) {
    switch (s) {
        case DoorState::CLOSED:  return "CLOSED";
        case DoorState::OPENING: return "OPENING";
        case DoorState::OPEN:    return "OPEN";
        case DoorState::CLOSING: return "CLOSING";
        case DoorState::STOPPED: return "STOPPED";
        case DoorState::ERROR:   return "ERROR";
        default:                 return "UNKNOWN";
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
