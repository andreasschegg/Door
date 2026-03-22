// ============================================================
// Door Controller — Test Platform v2
// For Prusa 3D Printer (fits 250x210mm bed)
// ============================================================
//
// Layout (top view):
//   +-------------------------------------------+
//   |                                           |
//   |  [Breadboard 165x55]                     |
//   |  (Arduino Nano ESP32 plugs into it)      |
//   |                                           |
//   |  [IBT_2 heatsink-down]   [===Motor===]→  |
//   |  pins up                  lying on side   |
//   +-------------------------------------------+
//
// IBT_2: heatsink faces down onto platform, pins face up.
// Motor: lies on its side in a cradle, shaft extends right.
//

$fn = 60;

// ============================================================
// Component Dimensions (adjust to match your parts)
// ============================================================

// Breadboard
bb_w = 165;
bb_h = 55;

// IBT_2 (BTS7960) Motor Driver Module
ibt_w     = 50;         // PCB width
ibt_h     = 50;         // PCB height
ibt_holes = [            // M3 mounting holes from bottom-left
    [ 4,  4],
    [46,  4],
    [ 4, 46],
    [46, 46]
];

// DFRobot FIT0185 Motor
motor_d        = 37;     // Motor body diameter
motor_length   = 75;     // Total motor body length (without shaft)

// ============================================================
// Platform Parameters
// ============================================================

margin   = 15;
gap      = 15;
plate_t  = 3;

// IBT_2 mounting (heatsink-down, screws from above)
ibt_hole_d = 3.2;       // M3 through-hole

// Motor cradle
cradle_wall   = 5;       // Wall thickness
cradle_clearance = 1;    // Diameter clearance for motor fit

// Breadboard rails
rail_h = 5;
rail_w = 2.5;

// Zip-tie slot dimensions
zip_w = 4;               // Slot width
zip_h = 3;               // Slot height

// ============================================================
// Layout Calculation
// ============================================================

// Motor cradle dimensions
cradle_r     = motor_d / 2 + cradle_clearance;
cradle_total_w = motor_d + 2 * cradle_wall;
cradle_total_h = cradle_r + cradle_wall;

// Bottom row positions
ibt_pos     = [margin, margin];
cradle_pos  = [margin + ibt_w + gap, margin + (ibt_h - cradle_total_w) / 2];

// Platform width: fits breadboard or bottom row, whichever is larger
plate_w = max(bb_w + 2 * margin,
              cradle_pos[0] + motor_length + margin);

// Breadboard above bottom row
bb_pos = [(plate_w - bb_w) / 2,
           margin + max(ibt_h, cradle_total_w) + gap];

// Platform height
plate_h = bb_pos[1] + bb_h + margin;

// ============================================================
// Modules
// ============================================================

module rounded_plate(w, h, t, r = 5) {
    hull() {
        for (x = [r, w - r], y = [r, h - r])
            translate([x, y, 0])
                cylinder(r = r, h = t);
    }
}

// ============================================================
// Base Plate
// ============================================================

module base() {
    rounded_plate(plate_w, plate_h, plate_t);
}

// ============================================================
// IBT_2 Mount (heatsink-down, pins-up)
// Through-holes for M3 screws, heatsink rests on plate surface
// ============================================================

module ibt_mount() {
    // Through-holes in plate
    for (hole = ibt_holes) {
        translate([ibt_pos[0] + hole[0],
                   ibt_pos[1] + hole[1],
                   -0.1])
            cylinder(d = ibt_hole_d, h = plate_t + 0.2);
    }

    // Outline embossed on plate surface
    translate([ibt_pos[0], ibt_pos[1], plate_t])
        difference() {
            cube([ibt_w, ibt_h, 0.6]);
            translate([1.5, 1.5, -0.1])
                cube([ibt_w - 3, ibt_h - 3, 0.8]);
        }
}

// ============================================================
// Motor Cradle (motor lying on its side, shaft extends right)
// Two end walls with semicircular cutout + base + zip-tie slots
// ============================================================

module motor_cradle() {
    translate([cradle_pos[0], cradle_pos[1], plate_t]) {
        // Base plate of cradle
        cube([motor_length, cradle_total_w, 2]);

        // Left end wall (closed — stops motor from sliding out)
        difference() {
            cube([cradle_wall, cradle_total_w, cradle_total_h]);
            translate([-1, cradle_total_w / 2, cradle_total_h])
                rotate([0, 90, 0])
                    cylinder(r = cradle_r, h = cradle_wall + 2);
        }

        // Right end wall (with shaft slot — motor shaft exits here)
        translate([motor_length - cradle_wall, 0, 0])
            difference() {
                cube([cradle_wall, cradle_total_w, cradle_total_h]);
                // Semicircular cutout
                translate([-1, cradle_total_w / 2, cradle_total_h])
                    rotate([0, 90, 0])
                        cylinder(r = cradle_r, h = cradle_wall + 2);
                // Shaft slot (open at bottom of cutout for shaft)
                translate([-1, cradle_total_w / 2 - motor_d / 4,
                           cradle_total_h - cradle_r])
                    cube([cradle_wall + 2, motor_d / 2, cradle_r]);
            }

        // Middle support wall
        translate([motor_length / 2 - cradle_wall / 2, 0, 0])
            difference() {
                cube([cradle_wall, cradle_total_w, cradle_total_h]);
                translate([-1, cradle_total_w / 2, cradle_total_h])
                    rotate([0, 90, 0])
                        cylinder(r = cradle_r, h = cradle_wall + 2);
            }

        // Zip-tie bridges (arch over the motor for securing)
        for (x_offset = [motor_length * 0.25, motor_length * 0.7]) {
            translate([x_offset - zip_w / 2, 0, 0]) {
                difference() {
                    // Bridge block
                    cube([zip_w, cradle_total_w, cradle_total_h + 5]);
                    // Motor clearance (slightly larger than motor)
                    translate([-1, cradle_total_w / 2, cradle_total_h])
                        rotate([0, 90, 0])
                            cylinder(r = cradle_r + 2, h = zip_w + 2);
                    // Zip-tie channel (slot through the bridge)
                    translate([-1, cradle_wall - 1, cradle_total_h + 1])
                        cube([zip_w + 2, cradle_total_w - 2 * cradle_wall + 2, zip_h]);
                }
            }
        }
    }
}

// ============================================================
// Breadboard Rails (side clips + lips)
// ============================================================

module breadboard_rails() {
    // Left rail
    translate([bb_pos[0] - rail_w, bb_pos[1], plate_t])
        cube([rail_w, bb_h, rail_h]);
    // Right rail
    translate([bb_pos[0] + bb_w, bb_pos[1], plate_t])
        cube([rail_w, bb_h, rail_h]);
    // Front lip
    translate([bb_pos[0] - rail_w, bb_pos[1] - rail_w, plate_t])
        cube([bb_w + 2 * rail_w, rail_w, rail_h * 0.6]);
    // Back lip
    translate([bb_pos[0] - rail_w, bb_pos[1] + bb_h, plate_t])
        cube([bb_w + 2 * rail_w, rail_w, rail_h * 0.6]);
}

// ============================================================
// Labels (embossed text)
// ============================================================

module labels() {
    translate([ibt_pos[0] + ibt_w / 2,
               ibt_pos[1] + ibt_h / 2, plate_t])
        linear_extrude(0.5)
            text("IBT_2", size = 7, halign = "center",
                 valign = "center", font = "Arial:style=Bold");

    translate([cradle_pos[0] + motor_length / 2,
               cradle_pos[1] - 8, plate_t])
        linear_extrude(0.5)
            text("MOTOR", size = 6, halign = "center",
                 valign = "center", font = "Arial:style=Bold");

    translate([bb_pos[0] + bb_w / 2,
               bb_pos[1] + bb_h / 2, plate_t])
        linear_extrude(0.5)
            text("BREADBOARD + NANO", size = 6, halign = "center",
                 valign = "center", font = "Arial:style=Bold");
}

// ============================================================
// Assembly
// ============================================================

difference() {
    union() {
        color("SlateGray") base();
        color("DimGray")   motor_cradle();
        color("DimGray")   breadboard_rails();
        color("White")     labels();
    }
    // IBT_2 through-holes (cut from assembled base)
    ibt_mount();
}

// IBT_2 outline (added after difference)
translate([ibt_pos[0], ibt_pos[1], plate_t])
    color("White")
        difference() {
            cube([ibt_w, ibt_h, 0.6]);
            translate([1.5, 1.5, -0.1])
                cube([ibt_w - 3, ibt_h - 3, 0.8]);
        }

// ============================================================
// Info
// ============================================================

echo(str("Platform size: ", plate_w, " x ", plate_h, " mm"));
echo(str("Cradle total width: ", cradle_total_w, " mm"));
echo(str("Cradle total height: ", cradle_total_h + plate_t, " mm above base"));
echo(str("Fits Prusa bed: ", plate_w <= 250 && plate_h <= 210 ? "YES" : "NO"));
