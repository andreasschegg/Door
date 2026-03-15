#include "Encoder.h"
#include "config/Config.h"

#define PCNT_HIGH_LIMIT  30000
#define PCNT_LOW_LIMIT  -30000

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint16_t cpr)
    : _pinA(pinA), _pinB(pinB), _cpr(cpr)
    , _unit(PCNT_UNIT_0)
    , _overflowCount(0), _positionOffset(0)
    , _lastPosition(0), _lastRpmTime(0), _rpm(0)
    , _detected(false) {}

void Encoder::begin() {
    // Channel 0: count on A edges, use B for direction
    pcnt_config_t cfgA = {};
    cfgA.pulse_gpio_num = _pinA;
    cfgA.ctrl_gpio_num  = _pinB;
    cfgA.channel         = PCNT_CHANNEL_0;
    cfgA.unit            = _unit;
    cfgA.pos_mode        = PCNT_COUNT_INC;  // rising A + B high = forward
    cfgA.neg_mode        = PCNT_COUNT_DEC;  // falling A + B high = reverse
    cfgA.lctrl_mode      = PCNT_MODE_REVERSE;
    cfgA.hctrl_mode      = PCNT_MODE_KEEP;
    cfgA.counter_h_lim   = PCNT_HIGH_LIMIT;
    cfgA.counter_l_lim   = PCNT_LOW_LIMIT;
    pcnt_unit_config(&cfgA);

    // Channel 1: count on B edges, use A for direction
    pcnt_config_t cfgB = {};
    cfgB.pulse_gpio_num = _pinB;
    cfgB.ctrl_gpio_num  = _pinA;
    cfgB.channel         = PCNT_CHANNEL_1;
    cfgB.unit            = _unit;
    cfgB.pos_mode        = PCNT_COUNT_DEC;
    cfgB.neg_mode        = PCNT_COUNT_INC;
    cfgB.lctrl_mode      = PCNT_MODE_REVERSE;
    cfgB.hctrl_mode      = PCNT_MODE_KEEP;
    cfgB.counter_h_lim   = PCNT_HIGH_LIMIT;
    cfgB.counter_l_lim   = PCNT_LOW_LIMIT;
    pcnt_unit_config(&cfgB);

    // Glitch filter: ignore pulses shorter than ~1.25µs
    pcnt_set_filter_value(_unit, 100);
    pcnt_filter_enable(_unit);

    // Overflow interrupt
    pcnt_event_enable(_unit, PCNT_EVT_H_LIM);
    pcnt_event_enable(_unit, PCNT_EVT_L_LIM);
    pcnt_isr_service_install(0);
    pcnt_isr_handler_add(_unit, overflowISR, this);

    pcnt_counter_pause(_unit);
    pcnt_counter_clear(_unit);
    pcnt_counter_resume(_unit);

    _lastRpmTime = millis();
    _lastPosition = 0;

    Serial.printf("[Encoder] Initialized on GPIO %d/%d, %d CPR\n", _pinA, _pinB, _cpr);
}

void IRAM_ATTR Encoder::overflowISR(void* arg) {
    auto* self = static_cast<Encoder*>(arg);
    int16_t count;
    pcnt_get_counter_value(self->_unit, &count);

    if (count >= PCNT_HIGH_LIMIT)
        self->_overflowCount += PCNT_HIGH_LIMIT;
    else if (count <= PCNT_LOW_LIMIT)
        self->_overflowCount += PCNT_LOW_LIMIT;

    pcnt_counter_clear(self->_unit);
}

int32_t Encoder::getPosition() {
    int16_t count;
    pcnt_get_counter_value(_unit, &count);
    return _overflowCount + count + _positionOffset;
}

void Encoder::resetPosition() {
    pcnt_counter_pause(_unit);
    pcnt_counter_clear(_unit);
    _overflowCount = 0;
    _positionOffset = 0;
    pcnt_counter_resume(_unit);
}

void Encoder::setPosition(int32_t pos) {
    pcnt_counter_pause(_unit);
    pcnt_counter_clear(_unit);
    _overflowCount = 0;
    _positionOffset = pos;
    pcnt_counter_resume(_unit);
}

void Encoder::update() {
    uint32_t now = millis();
    uint32_t dt = now - _lastRpmTime;
    if (dt < 100) return;

    int32_t pos = getPosition();
    int32_t delta = pos - _lastPosition;

    _rpm = (float)delta / _cpr * 60000.0f / dt;
    // Exponential smoothing
    static float smoothed = 0;
    smoothed = smoothed * 0.7f + _rpm * 0.3f;
    _rpm = smoothed;

    if (!_detected && abs(pos) >= ENCODER_DETECT_PULSES)
        _detected = true;

    _lastPosition = pos;
    _lastRpmTime = now;
}
