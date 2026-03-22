// ============================================================
// Door Controller — Test Platform
// For Prusa 3D Printer (fits 250x210mm bed)
// ============================================================
//
// Layout (top view):
//   +-------------------------------------------+
//   |                                           |
//   |       [Breadboard 165x55]                |
//   |                                           |
//   |  [Uno 69x54]  [BTS7960 50x43]  [Motor]  |
//   |                                           |
//   +-------------------------------------------+
//
// Motor shaft points DOWN through the platform.
// Platform stands on 15mm corner legs for shaft clearance.
//

$fn = 60;

// ============================================================
// Component Dimensions
// ============================================================

// Arduino Uno
uno_w     = 69;
uno_h     = 53.5;
uno_holes = [       // M3 mounting holes, relative to board bottom-left
    [14.0,  2.54],
    [15.24, 50.8],
    [66.04,  7.62],
    [66.04, 35.56]
];

// Breadboard
bb_w = 165;
bb_h = 55;

// BTS7960 Motor Driver Module
bts_w     = 50;
bts_h     = 43;
bts_holes = [       // M3 holes, approx 3mm from edges
    [ 3,  3],
    [47,  3],
    [ 3, 40],
    [47, 40]
];

// DFRobot FIT0185 Motor
motor_hex_r  = 15.5;    // Circumradius of 6x M3 hex hole pattern
motor_shaft_d = 10;     // Shaft clearance hole (6mm shaft + margin)

// ============================================================
// Platform Parameters
// ============================================================

margin   = 15;
gap      = 15;
plate_t  = 3;           // Base plate thickness
corner_r = 5;           // Rounded corner radius

// Standoff dimensions
standoff_h     = 8;     // Height above plate
standoff_d     = 7;     // Outer diameter
standoff_hole  = 3.2;   // M3 through-hole

// Corner legs (for shaft clearance below)
leg_h = 15;
leg_d = 12;

// Breadboard rail
rail_h = 5;
rail_w = 2.5;

// Motor mount pad
motor_pad_h = 5;

// ============================================================
// Layout Calculation
// ============================================================

// Bottom row positions (component bottom-left)
uno_pos   = [margin, margin];
bts_pos   = [margin + uno_w + gap,
             margin + (uno_h - bts_h) / 2];
motor_ctr = [bts_pos[0] + bts_w + gap + motor_hex_r + 12,
             margin + uno_h / 2];

// Platform width based on motor position
plate_w = motor_ctr[0] + motor_hex_r + 15 + margin;

// Top row: breadboard centered
bb_pos  = [(plate_w - bb_w) / 2,
            margin + uno_h + gap];

// Platform height based on breadboard
plate_h = bb_pos[1] + bb_h + margin;

// ============================================================
// Modules
// ============================================================

module rounded_plate(w, h, t, r) {
    hull() {
        for (x = [r, w - r], y = [r, h - r])
            translate([x, y, 0])
                cylinder(r = r, h = t);
    }
}

module standoff(h = standoff_h, d = standoff_d, hole = standoff_hole) {
    difference() {
        cylinder(d = d, h = h);
        translate([0, 0, -0.1])
            cylinder(d = hole, h = h + 0.2);
    }
}

module corner_leg(h = leg_h, d = leg_d) {
    difference() {
        cylinder(d = d, h = h);
        // Optional screw hole for table mounting
        translate([0, 0, -0.1])
            cylinder(d = 4, h = h + 0.2);
    }
}

// ============================================================
// Base Plate
// ============================================================

module base() {
    difference() {
        rounded_plate(plate_w, plate_h, plate_t, corner_r);

        // Motor shaft hole through plate
        translate([motor_ctr[0], motor_ctr[1], -0.1])
            cylinder(d = motor_shaft_d, h = plate_t + 0.2);

        // Motor M3 holes through plate
        for (i = [0:5]) {
            angle = i * 60;
            translate([motor_ctr[0] + motor_hex_r * cos(angle),
                       motor_ctr[1] + motor_hex_r * sin(angle),
                       -0.1])
                cylinder(d = standoff_hole, h = plate_t + 0.2);
        }
    }
}

// ============================================================
// Arduino Uno Standoffs
// ============================================================

module uno_mount() {
    for (hole = uno_holes) {
        translate([uno_pos[0] + hole[0],
                   uno_pos[1] + hole[1],
                   plate_t])
            standoff();
    }

    // Outline for visual reference (embossed border)
    translate([uno_pos[0], uno_pos[1], plate_t])
        difference() {
            cube([uno_w, uno_h, 0.6]);
            translate([1, 1, -0.1])
                cube([uno_w - 2, uno_h - 2, 0.8]);
        }
}

// ============================================================
// BTS7960 Standoffs
// ============================================================

module bts_mount() {
    for (hole = bts_holes) {
        translate([bts_pos[0] + hole[0],
                   bts_pos[1] + hole[1],
                   plate_t])
            standoff();
    }

    // Outline
    translate([bts_pos[0], bts_pos[1], plate_t])
        difference() {
            cube([bts_w, bts_h, 0.6]);
            translate([1, 1, -0.1])
                cube([bts_w - 2, bts_h - 2, 0.8]);
        }
}

// ============================================================
// Motor Mount (raised pad with hex M3 pattern)
// ============================================================

module motor_mount() {
    translate([motor_ctr[0], motor_ctr[1], plate_t]) {
        difference() {
            // Raised circular pad
            cylinder(d = motor_hex_r * 2 + 16, h = motor_pad_h);

            // Shaft hole
            translate([0, 0, -0.1])
                cylinder(d = motor_shaft_d, h = motor_pad_h + 0.2);

            // 6x M3 holes (hex pattern)
            for (i = [0:5]) {
                angle = i * 60;
                translate([motor_hex_r * cos(angle),
                           motor_hex_r * sin(angle),
                           -0.1])
                    cylinder(d = standoff_hole, h = motor_pad_h + 0.2);
            }
        }
    }
}

// ============================================================
// Breadboard Rails (side clips)
// ============================================================

module breadboard_rails() {
    // Left rail
    translate([bb_pos[0] - rail_w, bb_pos[1], plate_t])
        cube([rail_w, bb_h, rail_h]);

    // Right rail
    translate([bb_pos[0] + bb_w, bb_pos[1], plate_t])
        cube([rail_w, bb_h, rail_h]);

    // Front lip (small stop)
    translate([bb_pos[0] - rail_w, bb_pos[1] - rail_w, plate_t])
        cube([bb_w + 2 * rail_w, rail_w, rail_h * 0.6]);

    // Back lip
    translate([bb_pos[0] - rail_w, bb_pos[1] + bb_h, plate_t])
        cube([bb_w + 2 * rail_w, rail_w, rail_h * 0.6]);
}

// ============================================================
// Corner Legs (for motor shaft clearance)
// ============================================================

module legs() {
    leg_inset = 10;
    positions = [
        [leg_inset,            leg_inset],
        [plate_w - leg_inset,  leg_inset],
        [leg_inset,            plate_h - leg_inset],
        [plate_w - leg_inset,  plate_h - leg_inset]
    ];

    for (pos = positions) {
        translate([pos[0], pos[1], -leg_h])
            corner_leg();
    }
}

// ============================================================
// Labels (embossed text)
// ============================================================

module labels() {
    // Arduino Uno
    translate([uno_pos[0] + uno_w / 2,
               uno_pos[1] + uno_h / 2, plate_t])
        linear_extrude(0.5)
            text("UNO", size = 8, halign = "center",
                 valign = "center", font = "Arial:style=Bold");

    // BTS7960
    translate([bts_pos[0] + bts_w / 2,
               bts_pos[1] + bts_h / 2, plate_t])
        linear_extrude(0.5)
            text("BTS", size = 8, halign = "center",
                 valign = "center", font = "Arial:style=Bold");

    // Motor
    translate([motor_ctr[0],
               motor_ctr[1] - motor_hex_r - 14, plate_t])
        linear_extrude(0.5)
            text("MOTOR", size = 6, halign = "center",
                 valign = "center", font = "Arial:style=Bold");

    // Breadboard
    translate([bb_pos[0] + bb_w / 2,
               bb_pos[1] + bb_h / 2, plate_t])
        linear_extrude(0.5)
            text("BREADBOARD", size = 7, halign = "center",
                 valign = "center", font = "Arial:style=Bold");
}

// ============================================================
// Assembly
// ============================================================

color("SlateGray")  base();
color("DimGray")    uno_mount();
color("DimGray")    bts_mount();
color("DimGray")    motor_mount();
color("DimGray")    breadboard_rails();
color("DimGray")    legs();
color("White")      labels();

// ============================================================
// Info (shown in console)
// ============================================================

echo(str("Platform size: ", plate_w, " x ", plate_h, " x ", plate_t, " mm"));
echo(str("Total height with legs: ", leg_h + plate_t + max(standoff_h, motor_pad_h, rail_h), " mm"));
echo(str("Fits Prusa bed: ", plate_w <= 250 && plate_h <= 210 ? "YES" : "NO"));
