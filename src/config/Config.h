#pragma once

// =============================================
// Pin Configuration
// =============================================

// BTS7960 Motor Driver
#define PIN_RPWM        25      // Right PWM (forward / open)
#define PIN_LPWM        26      // Left PWM  (reverse / close)
#define PIN_R_EN        27      // Right enable
#define PIN_L_EN        14      // Left enable
#define PIN_R_IS        34      // Right current sense (ADC, input only)
#define PIN_L_IS        35      // Left current sense  (ADC, input only)

// Endstops (active LOW — connect switch between pin and GND)
#define PIN_ENDSTOP_OPEN    32
#define PIN_ENDSTOP_CLOSE   33

// =============================================
// Motor Settings
// =============================================
#define MOTOR_MAX_SPEED         255     // PWM duty cycle (0-255)
#define MOTOR_RAMP_TIME_MS      800     // Soft-start ramp duration
#define MOTOR_TIMEOUT_MS        15000   // Max runtime per operation

// =============================================
// Safety — Current Sensing
// =============================================
// IS pins are behind a 10kΩ/20kΩ voltage divider to protect the 3.3V ADC.
// Divider ratio: 20k / (10k + 20k) = 0.667
// The ADC reads 2/3 of the actual IS pin voltage.
//
// BTS7960 IS ratio: ~8500:1 (load current to sense current).
// With typical module sense resistor (~1kΩ):
//   V_IS ≈ I_load / 8.5     (e.g. 3A → 0.35V → ADC ~436 without divider)
// After voltage divider:     (e.g. 3A → 0.24V → ADC ~291)
//
// Threshold 1333 ≈ 2000 * 0.667 — triggers at roughly the same physical
// current as a raw threshold of 2000 would without the divider.
#define IS_R_TOP                10000   // Voltage divider top resistor (Ohm)
#define IS_R_BOTTOM             20000   // Voltage divider bottom resistor (Ohm)
#define CURRENT_THRESHOLD       1333    // ADC value — adjusted for divider
#define DEBOUNCE_MS             50      // Endstop debounce time

// =============================================
// PWM (LEDC)
// =============================================
#define PWM_FREQUENCY           20000   // 20 kHz — above audible range
#define PWM_RESOLUTION          8       // 8-bit (0-255)
#define PWM_CHANNEL_RPWM        0
#define PWM_CHANNEL_LPWM        1

// =============================================
// Network
// =============================================
#define MDNS_HOSTNAME           "door"
#define WEB_SERVER_PORT         80
