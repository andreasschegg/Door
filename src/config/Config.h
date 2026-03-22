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

// Encoder (DFRobot FIT0185 — Hall effect, quadrature)
#define PIN_ENCODER_A       16      // Encoder channel A
#define PIN_ENCODER_B       17      // Encoder channel B
#define ENCODER_ENABLED     1       // Set to 0 to compile without encoder

// =============================================
// Motor Settings (DFRobot FIT0185: 12V, 83RPM, 45kg·cm)
// =============================================
#define MOTOR_MAX_SPEED         255     // PWM duty cycle (0-255)
#define MOTOR_RAMP_TIME_MS      1200    // Soft-start ramp (longer for high-torque motor)
#define MOTOR_TIMEOUT_MS        25000   // Max runtime per operation
#define CALIBRATION_SPEED       180     // Reduced speed for calibration (~70%)

// =============================================
// Encoder
// =============================================
#define ENCODER_CPR             2096    // Counts per revolution (gearbox output)
#define ENCODER_DETECT_PULSES   10      // Min pulses to confirm encoder present
#define ENCODER_DETECT_MS       500     // Detection time window

// =============================================
// Safety — Current Sensing
// =============================================
// IS pins are behind a 10kΩ/18kΩ voltage divider to protect the 3.3V ADC.
// Divider ratio: 18k / (10k + 18k) = 0.643
// At max 5V IS output: 5 × 0.643 = 3.21V — safely under 3.3V ADC limit.
//
// BTS7960 IS ratio: ~8500:1 (load current to sense current).
// With typical module sense resistor (~1kΩ):
//   V_IS ≈ I_load / 8.5
// After voltage divider: V_ADC = V_IS × 0.643
//
// FIT0185: stall current 7A, normal load ~1-3A.
// Trip at ~5A: V_IS = 5/8.5 = 0.588V → after divider 0.378V → ADC ~469
// Set threshold to 482 (trips at ~5.1A, well under 7A stall).
#define IS_R_TOP                10000   // Voltage divider top resistor (Ohm)
#define IS_R_BOTTOM             18000   // Voltage divider bottom resistor (Ohm)
#define CURRENT_THRESHOLD       482     // ADC value — ~5A trip point (18kΩ divider)
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
