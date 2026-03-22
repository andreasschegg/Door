# Schaltung — Door Controller

## Übersicht

Arduino Nano ESP32 steuert über einen BTS7960 H-Bridge einen DC-Getriebemotor zum Öffnen und Schliessen einer Tür. Zwei Endschalter erkennen die Endlagen. Optional liefert ein Encoder am Motor präzise Positionsdaten.

**Unterstützte Motoren:**
- **Ansatz B (aktiv):** DFRobot FIT0185 — 12V, 83 RPM, 45 kg·cm, Hall-Encoder 2096 CPR
- **Ansatz C (Reserve):** DFRobot FIT0186 — 12V, 251 RPM, 18 kg·cm, Hall-Encoder 700 CPR
- **Ansatz A (Legacy):** Waveshare L-Shaped 240 RPM (ohne Encoder)

---

## Schaltplan

```
    12V Netzteil (min. 5A)
        │
        ├───────────────────────────────────── BTS7960 B+
        │                                         │
        │    ┌────────────────────────────────┐    │
        │    │     470µF (Elko) ║ 100nF       │    │
        │    │        GND                     │    │
        │    │   Stützkondensatoren 12V-Rail  │    │
        │    └────────────────────────────────┘    │
        │                                         │
        └──► Buck Converter 12V → 5V              │
                  │                                │
             ┌────┤                                │
             │  ┌─┴──────────────────────┐         │
             │  │  100µF (Elko) ║ 100nF  │         │
             │  │     GND                │         │
             │  │  Stützkondensatoren 5V │         │
             │  └────────────────────────┘         │
             │                                     │
             ├──► Arduino Nano ESP32 VIN                        │
             │       │                             │
             │     100nF (Keramik, nah am Chip)    │
             │      GND                            │
             │                                     │
             ├──► BTS7960 VCC (Logik)              │
             │                                     │
             └──► Encoder VCC (5V)                 │
                                                   │
                                                   │
    ┌──────────────────────────────────────────────┐│
    │            Arduino Nano ESP32 DevKit                    ││
    │                                              ││
    │  Motor-Steuerung (mit Pull-Down)             ││
    │  ──────────────────────────────              ││
    │  GPIO  5 ──┬── 10kΩ ── GND ──────► RPWM     ││
    │  GPIO  6 ──┬── 10kΩ ── GND ──────► LPWM     ││
    │  GPIO  7 ──┬── 10kΩ ── GND ──────► R_EN     ││
    │  GPIO  8 ──┬── 10kΩ ── GND ──────► L_EN     ││
    │                                              ││
    │  Strom-Messung (mit Spannungsteiler + Zener) ││
    │  ────────────────────────────────────────     ││
    │  GPIO  9 ◄─ 18kΩ ─┬─ 10kΩ ◄──────── R_IS    ││
    │                 ZD 3.3V                      ││
    │                   GND                        ││
    │  GPIO 10 ◄─ 18kΩ ─┬─ 10kΩ ◄──────── L_IS    ││
    │                 ZD 3.3V                      ││
    │                   GND                        ││
    │                                              ││
    │  Encoder (mit Serienwiderstand + Filter)       ││
    │  ────────────────────────────────────────     ││
    │  GPIO  3 ◄── 1kΩ ──┬──────────────── Ch.A   ││
    │                   100nF                      ││
    │                    GND                       ││
    │  GPIO  4 ◄── 1kΩ ──┬──────────────── Ch.B   ││
    │                   100nF                      ││
    │                    GND                       ││
    │                                              ││
    │  Endschalter (mit Serienwiderstand + Filter) ││
    │  ────────────────────────────────────────     ││
    │  GPIO  1 ◄── 1kΩ ──┬──── /  ── GND  (OPEN)  ││
    │                   100nF                      ││
    │                    GND                       ││
    │  GPIO  2 ◄── 1kΩ ──┬──── /  ── GND  (CLOSE) ││
    │                   100nF                      ││
    │                    GND                       ││
    │                                              ││
    └──────────────────────────────────────────────┘│
                                                    │
    ┌───────────────────────────────────────────────┘
    │
    │  ┌──────────────┐      ┌──────────────────────┐
    │  │   BTS7960     │      │  DFRobot FIT0185     │
    │  │               │      │  DC-Getriebemotor    │
    │  │  M+ ──────────┼──────┤  12V / 83 RPM       │
    │  │  M- ──────────┼──────┤  45 kg·cm            │
    │  │  B- ── GND    │      │                      │
    │  │  GND ── GND   │      │  Encoder:            │
    │  └───────────────┘      │  VCC ◄── 5V Rail     │
    │                         │  GND ◄── GND         │
    │                         │  Ch.A ──► GPIO  3    │
    │                         │  Ch.B ──► GPIO  4    │
    │                         └──────────────────────┘
    │
   GND (alle Massen verbunden)
```

---

## Stückliste

| Bauteil | Anzahl | Wert | Bauform | Zweck |
|---|---|---|---|---|
| Widerstand | 4 | 10kΩ | 0805 / THT | Pull-Down Motor-Pins |
| Widerstand | 2 | 10kΩ | 0805 / THT | Spannungsteiler IS (oben) |
| Widerstand | 2 | 18kΩ | 0805 / THT | Spannungsteiler IS (unten) |
| Widerstand | 4 | 1kΩ | 0805 / THT | Serienschutz Endschalter + Encoder |
| Zener-Diode | 2 | 3.3V | SOD-323 / THT | ADC-Überspannungsschutz |
| Elko | 1 | 470µF / 25V | Radial | 12V-Puffer |
| Elko | 1 | 100µF / 10V | Radial | 5V-Puffer |
| Keramikkondensator | 7 | 100nF | 0805 / THT | Entkopplung (12V, 5V, Arduino Nano ESP32, 2× Endschalter, 2× Encoder) |
| Mikroschalter | 2 | – | – | Endlagen-Erkennung |

---

## Begründung der Schutzbeschaltung

### Pull-Down Widerstände (10kΩ) auf RPWM, LPWM, R_EN, L_EN

**Problem:** Während des Arduino Nano ESP32-Bootprozesses (ca. 100-300 ms) sind die GPIOs nicht konfiguriert und floaten. Der BTS7960 interpretiert ein floatendes HIGH-Signal als aktiv — der Motor könnte beim Einschalten unkontrolliert anlaufen.

**Lösung:** 10kΩ Pull-Down Widerstände halten die Leitungen zuverlässig auf LOW, bis der Arduino Nano ESP32 die Pins konfiguriert hat. 10kΩ ist hoch genug, um den Arduino Nano ESP32-Ausgang nicht merklich zu belasten (0.33 mA bei 3.3V), aber niedrig genug, um sicher gegen Störeinkopplungen zu sein.

**Warum nicht im BTS7960 integriert?** Das BTS7960-Modul hat keine definierten Pull-Downs auf den Logikeingängen. Der Zustand bei offenem Eingang ist undefiniert.

### Spannungsteiler (10kΩ / 18kΩ) + Zener-Diode (3.3V) auf IS-Pins

**Problem:** Die Current-Sense Ausgänge des BTS7960 können bis zu 5V ausgeben (bei hohem Motorstrom). Der Arduino Nano ESP32 ADC verträgt maximal 3.3V — höhere Spannungen beschädigen den Eingangs-Pin dauerhaft.

**Lösung — Spannungsteiler:**
```
V_adc = V_is × R_bottom / (R_top + R_bottom)
V_adc = V_is × 18kΩ / (10kΩ + 18kΩ)
V_adc = V_is × 0.643
```
Bei maximalen 5V am IS-Pin kommen 3.21V am ADC an — sicher unter dem 3.3V Limit.

**Lösung — Zener-Diode:**
Die 3.3V Zener-Diode ist ein Sicherheitsnetz: Falls der Spannungsteiler allein nicht ausreicht (z.B. durch Toleranzen oder Spannungsspitzen), clippt die Zener zuverlässig bei 3.3V und schützt den ADC.

**Auswirkung auf die Software:**
Der `CURRENT_THRESHOLD` in `Config.h` ist auf `482` gesetzt. Bei der FIT0185 (7A Stall) löst dies bei ~5.1A aus:
```
V_IS = 5.1A / 8.5 = 0.6V → nach Teiler (×0.643) 0.386V → ADC ≈ 479
```

### Encoder-Schutz (1kΩ + 100nF)

**Problem:** Der DFRobot FIT0185 Encoder gibt 5V-Signale aus. Die Arduino Nano ESP32-GPIO-Eingänge sind offiziell für max. 3.3V spezifiziert, tolerieren aber de facto 5V auf Eingängen über die internen Schutzdioden.

**Lösung:** Der 1kΩ Serienwiderstand begrenzt den Strom in die interne Clamp-Diode auf ~1.7mA — weit unter dem kritischen Bereich. Bei der Einsatzhäufigkeit dieses Projekts (30-40 Zyklen pro Abend) ist Langzeit-Degradation kein Thema. Ein Level-Shifter wäre erst bei 24/7-Dauerbetrieb empfehlenswert.

**Zusätzlich:** Der 100nF Kondensator bildet zusammen mit dem 1kΩ Widerstand einen RC-Tiefpass, der EMV-Störungen vom Motorkabel und Encoder-Glitches filtert.

### Stützkondensatoren (Elkos + Keramik)

**Problem:** Der Motor erzeugt beim Anlaufen, Abbremsen und unter Last starke Stromschwankungen. Diese verursachen Spannungseinbrüche (Brownouts) und hochfrequente Störungen (EMV) auf der Versorgungsleitung. Der Arduino Nano ESP32 ist besonders empfindlich — ein kurzer Spannungseinbruch unter ~2.8V löst einen Brownout-Reset aus.

**Lösung — 12V-Rail (470µF Elko + 100nF Keramik):**
- Der 470µF Elko puffert die grossen Stromspitzen des Motors (Anlaufstrom kann kurzzeitig 3-5× Nennstrom betragen)
- Der 100nF Keramik filtert hochfrequente Störungen, die der Elko aufgrund seiner Induktivität nicht abfangen kann
- Platzierung: So nah wie möglich am BTS7960 B+ Eingang

**Lösung — 5V-Rail (100µF Elko + 100nF Keramik):**
- Puffert die 5V-Versorgung für Arduino Nano ESP32, BTS7960-Logik und Encoder
- Verhindert, dass Motor-Spannungseinbrüche auf dem 12V-Rail durch den Buck Converter auf die 5V-Seite durchschlagen

**Lösung — 100nF direkt am Arduino Nano ESP32:**
- Entkopplung der digitalen Schaltung vom Rest
- Filtert kurze HF-Störungen direkt an der Versorgung des Mikrocontrollers
- Platzierung: So nah wie möglich an VIN/GND des Arduino Nano ESP32-Boards

**Warum zwei Typen?** Elkos haben hohe Kapazität aber schlechtes HF-Verhalten (parasitäre Induktivität). Keramikkondensatoren haben niedrige Kapazität aber exzellentes HF-Verhalten. Zusammen decken sie das gesamte Frequenzspektrum ab.

### Endschalter-Schutz (1kΩ + 100nF)

**Problem 1 — Fehlverdrahtung:** Wenn ein Endschalter-Kabel versehentlich an 12V oder 5V gerät (z.B. bei Wartungsarbeiten), fliesst ohne Schutz ein zerstörerischer Strom in den GPIO.

**Lösung:** Der 1kΩ Serienwiderstand begrenzt den Strom auf maximal 12 mA bei 12V Fehlspannung — sicher für den Arduino Nano ESP32 (max. 12 mA pro Pin). Im Normalbetrieb fliesst nur 3.3V / (1kΩ + interner Pull-up ~45kΩ) ≈ 0.07 mA — vernachlässigbar.

**Problem 2 — Prellen und Störungen:** Mechanische Endschalter prellen beim Betätigen (10-50 ms Kontaktspringen). Lange Kabel zum Endschalter wirken als Antennen und fangen elektromagnetische Störungen ein (besonders vom PWM-getakteten Motor).

**Lösung:** Der 100nF Kondensator bildet zusammen mit dem 1kΩ Widerstand einen RC-Tiefpass (τ = R×C = 0.1 ms, Grenzfrequenz ≈ 1.6 kHz). Dieser filtert:
- Kontaktprellen (typisch 1-10 kHz)
- HF-Störungen vom Motortreiber (20 kHz PWM und Oberwellen)

Die Software implementiert zusätzlich ein 50 ms Debouncing als zweite Schutzschicht.

---

## Verdrahtungshinweise

1. **GND-Führung:** Alle Massen (Netzteil, Buck Converter, Arduino Nano ESP32, BTS7960, Encoder) an einem gemeinsamen Punkt verbinden (Star-Ground). Keine Schleifen bilden — Motor-Rückstrom soll nicht über die Logik-Masse fliessen.

2. **Kabelquerschnitt:** Motorleitungen (M+/M-, B+/B-) mindestens 1.0 mm², Signalleitungen 0.25 mm² genügt.

3. **Kondensatoren-Platzierung:** Stützkondensatoren immer so nah wie möglich an den Verbraucher-Pins. Lange Leitungen zwischen Kondensator und IC machen die Entkopplung wirkungslos.

4. **Endschalter-Kabel:** Geschirmte oder paarweise verdrillte Leitungen verwenden, wenn die Kabelwege länger als 30 cm sind. Den Schirm einseitig (am Arduino Nano ESP32) auf GND legen.

5. **Motor-Kabel:** Kurz halten und von den Signalleitungen räumlich trennen. Idealerweise separat verlegen.

6. **Encoder-Kabel:** Der DFRobot FIT0185 hat ein 6-adriges Kabel (Motor + Encoder). Die Encoder-Signalleitungen (Ch.A, Ch.B) möglichst kurz halten und nicht parallel zu den Motorleitungen führen. Bei Kabellängen über 50 cm geschirmtes Kabel verwenden.

---

## Encoder-Kalibrierung

Beim ersten Start (oder nach Firmware-Update) muss die Tür kalibriert werden:

1. **Über das Web-Interface** (`http://door.local`) → Settings → "Calibrate"
2. Der Motor fährt die Tür zuerst ganz zu (Endschalter CLOSE → Position 0)
3. Dann fährt er die Tür ganz auf (Endschalter OPEN → Position wird gespeichert)
4. Die Kalibrierungsdaten werden im NVS (Non-Volatile Storage) des Arduino Nano ESP32 gespeichert und überleben Neustarts

Ohne Kalibrierung arbeitet der Controller ausschliesslich mit Endschaltern (wie Ansatz A).
