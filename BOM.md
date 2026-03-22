# Stückliste (Bill of Materials) — Door Controller v2.0

## Hauptkomponenten

| # | Bauteil | Beschreibung | Anzahl | Bezugsquelle |
|---|---|---|---|---|
| 1 | Arduino Nano ESP32 | ESP32-S3, u-blox NORA-W106, USB-C | 1 | Arduino Store / Händler |
| 2 | BTS7960 H-Bridge | 43A Motor Driver Modul | 1 | |
| 3 | DFRobot FIT0185 | DC-Getriebemotor 12V, 83RPM, 45kg·cm, Hall-Encoder 2096 CPR | 1 | DFRobot / Schweizer Händler |
| 4 | 12V Netzteil | min. 5A (60W), DC-Hohlstecker | 1 | |
| 5 | Buck Converter | Step-Down 12V → 5V, min. 2A | 1 | |
| 6 | Mikroschalter | Endschalter, Schliesser (NO), Rollenhebel | 2 | |

## Schutzbeschaltung

| # | Bauteil | Wert | Anzahl | Zweck |
|---|---|---|---|---|
| 7 | Widerstand | 10kΩ | 6 | 4× Pull-Down Motor-Pins, 2× Spannungsteiler IS (oben) |
| 8 | Widerstand | 18kΩ | 2 | Spannungsteiler IS (unten) |
| 9 | Widerstand | 1kΩ | 4 | 2× Serienschutz Endschalter, 2× Serienschutz Encoder |
| 10 | Zener-Diode 3.3V | z.B. BZX84C3V3 | 2 | ADC-Überspannungsschutz IS-Pins |
| 11 | Elko 470µF / 25V | Radial | 1 | 12V-Rail Puffer |
| 12 | Elko 100µF / 10V | Radial | 1 | 5V-Rail Puffer |
| 13 | Keramikkondensator 100nF | 0805 / THT | 7 | Entkopplung: 1× 12V, 1× 5V, 1× ESP32, 2× Endschalter, 2× Encoder |

## Verkabelung / Montage

| # | Bauteil | Beschreibung | Anzahl |
|---|---|---|---|
| 14 | Litze 1.0mm² | Motorleitungen (M+/M-, B+/B-), rot + schwarz | ~2m |
| 15 | Litze 0.25mm² | Signalleitungen (Endschalter, Encoder, GPIO), diverse Farben | ~5m |
| 16 | Lochrasterplatine | Für Schutzbeschaltung, 5×7cm oder grösser | 1 |
| 17 | Stiftleisten | Zum Anschluss der Kabel an die Platine | nach Bedarf |
| 18 | Schraubklemmen | Für Motor- und Netzteil-Anschluss | 4-6 |
| 19 | Gehäuse | Für ESP32 + Platine, ggf. 3D-gedruckt | 1 |

## Zusammenfassung

| Kategorie | Anzahl Positionen | Geschätzte Kosten (CHF) |
|---|---|---|
| Hauptkomponenten | 6 | ~80-120 |
| Schutzbeschaltung | 7 | ~3-5 |
| Verkabelung / Montage | 6 | ~10-20 |
| **Total** | **19** | **~95-145** |

## Hinweise

- **DFRobot FIT0186** (251RPM, 18kg·cm) ist als Drop-in Alternative vorgesehen — gleicher Formfaktor, gleiche Verkabelung, nur Software-Parameter ändern
- **Waveshare L-Shaped 240RPM** (Ansatz A) ist ebenfalls kompatibel, hat aber keinen Encoder
- Alle Widerstände und Kondensatoren in THT (bedrahtet) für einfache Lochrasterplatinen-Montage
- Keramikkondensatoren so nah wie möglich an den jeweiligen ICs platzieren
