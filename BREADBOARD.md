# Breadboard-Verdrahtung — Door Controller

## Nano ESP32 Position

Nano eingesteckt mit **D13 auf C60** und **TX/D1 auf G46**.
Linke Pins in Reihe C/D/E, rechte Pins in Reihe F/G/H.

```
Breadboard-Spalten:    46  47  48  49  50  51  52  53  54  55  56  57  58  59  60

Reihe A (frei)  ─────────────────────────────────────────────────────────────────
Reihe B (frei)  ── ── ── ── ── ── ── ── EB EA ES ES ── ── ──  ← Sensor-Kabel
                                         │  │  CL OP
═══════════════════════════════════════════════════════════════════════════════════
Reihe C         VIN GND RST 5V  A7  A6  A5  A4  A3  A2  A1  A0  AR 3V3 D13
                                            Enc Enc End End
                                             B   A  CL  OP
─── Nano ESP32 (linke Seite) ──────────────────────────────────────────────────────
Reihe E         ... ... ... ... ... ... ... ... ... ... ... ... ... ... ...
═══════════════════════════════════════════════════════════════════════════════════
Reihe F         ... ... ... ... ... ... ... ... ... ... ... ... ... ... ...
─── Nano ESP32 (rechte Seite) ────────────────────────────────────────────────────
Reihe G         TX  RX  RST GND D2  D3  D4  D5  D6  D7  D8  D9  D10 D11 D12
                                RPW LPW R_  L_  R_  L_
                                 M   M  EN  EN  IS  IS
═══════════════════════════════════════════════════════════════════════════════════
Reihe H (frei)  ── ── ── ── ── ── ── ── ── ── ── ── ── ── ──  ← BTS7960-Kabel
Reihe I (frei)  ── ── ── ── RP LP RE LE RI LI ── ── ── ── ──
                             WM WM EN EN IS IS
Reihe J (frei)  ─────────────────────────────────────────────────────────────────
```

## Verdrahtungstabelle

### BTS7960 Motor Driver (rechte Seite, Reihe H/I/J)

| Breadboard | Nano Pin | GPIO | Signal | BTS7960 Pin | Kabelfarbe |
|---|---|---|---|---|---|
| I50 | D2 | GPIO 5 | RPWM | RPWM | gelb |
| I51 | D3 | GPIO 6 | LPWM | LPWM | orange |
| I52 | D4 | GPIO 7 | R_EN | R_EN | grün |
| I53 | D5 | GPIO 8 | L_EN | L_EN | blau |
| I54 | D6 | GPIO 9 | R_IS | R_IS | weiss |
| I55 | D7 | GPIO 10 | L_IS | L_IS | grau |
| G49 | – | – | GND | GND | schwarz |

### Endschalter (linke Seite, Reihe A/B)

| Breadboard | Nano Pin | GPIO | Signal | Ziel | Kabelfarbe |
|---|---|---|---|---|---|
| B57 | A0 | GPIO 1 | Endstop OPEN | Schalter → GND | grün |
| B56 | A1 | GPIO 2 | Endstop CLOSE | Schalter → GND | rot |

### Encoder (linke Seite, Reihe A/B)

| Breadboard | Nano Pin | GPIO | Signal | Ziel | Kabelfarbe |
|---|---|---|---|---|---|
| B55 | A2 | GPIO 3 | Encoder Ch.A | Motor Encoder | violett |
| B54 | A3 | GPIO 4 | Encoder Ch.B | Motor Encoder | braun |

### Stromversorgung

| Breadboard | Nano Pin | Signal | Ziel |
|---|---|---|---|
| C49 / B49 | 5V | 5V Rail | BTS7960 VCC, Encoder VCC, Buck Converter Out |
| C47 / B47 | GND | GND | Gemeinsame Masse |
| G49 | GND | GND | BTS7960 GND (rechte Seite) |
| C46 | VIN | 12V (über Buck) | Nicht verwenden — 5V über C49 einspeisen |

## Schutzbeschaltung

Die Widerstände, Kondensatoren und Zener-Dioden der Schutzbeschaltung werden
auf dem Breadboard zwischen Nano-Pins und den Zielkomponenten platziert:

- **10kΩ Pull-Down** (RPWM, LPWM, R_EN, L_EN): Von Spalte 50-53 (Reihe I/J) nach GND-Rail
- **10kΩ + 18kΩ Spannungsteiler** (R_IS, L_IS): In Spalte 54-55 (Reihe I/J)
- **1kΩ + 100nF RC-Filter** (Endschalter): In Spalte 56-57 (Reihe A/B)
- **1kΩ + 100nF RC-Filter** (Encoder): In Spalte 54-55 (Reihe A/B)

## Übersicht

```
                    ┌─────────────────────────────────┐
                    │         BREADBOARD               │
                    │                                   │
  Endschalter ──────┤ A/B Reihe (links)                │
  Encoder     ──────┤ Spalten 54-57                    │
                    │                                   │
                    │    ┌─────────────────────┐       │
                    │    │  Arduino Nano ESP32  │       │
                    │    │  C46-C60 / G46-G60  │       │
                    │    └─────────────────────┘       │
                    │                                   │
  BTS7960 ──────────┤ H/I Reihe (rechts)               │
  (6 Signale)       │ Spalten 50-55                    │
                    │                                   │
                    │ 5V: C49    GND: C47, G49         │
                    └─────────────────────────────────┘
```
