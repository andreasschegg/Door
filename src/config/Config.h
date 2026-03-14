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
// Safety
// =============================================
#define CURRENT_THRESHOLD       2000    // ADC value — overcurrent / obstruction
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
