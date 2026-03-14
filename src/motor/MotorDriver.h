#pragma once

#include <Arduino.h>

/// Abstract motor driver interface.
/// Swap implementations to support different H-Bridge modules.
class MotorDriver {
public:
    virtual ~MotorDriver() = default;

    virtual void begin() = 0;

    /// Set motor speed (0-255) and direction (true = forward/open).
    virtual void setSpeed(uint8_t speed, bool forward) = 0;

    /// Coast stop — motor spins down freely.
    virtual void stop() = 0;

    /// Active brake — shorts motor windings.
    virtual void brake() = 0;

    /// Read current sense (raw ADC value, 0-4095).
    virtual uint16_t getCurrentSense() = 0;

    virtual void enable() = 0;
    virtual void disable() = 0;
};
