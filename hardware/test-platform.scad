// ============================================================
// Door Controller — Test Platform v3
// For Prusa 3D Printer (fits 250x210mm bed)
// ============================================================
//
// Layout (top view, user sits at bottom):
//   +-------------------------------------------+
//   |  [IBT_2 on 25mm standoffs]  [Motor]→     |
//   |  40x40 holes, pins up      side-mount    |
//   |                                           |
//   |      [Buck Converter 37x17]              |
//   |       snap-in socket, 10mm               |
//   |                                           |
//   |  [Breadboard 165x55]  ← taped down       |
//   |  (Arduino Nano ESP32 plugs into it)      |
//   +-------------------------------------------+
//          ↑ user sits here
//
// Front row:   Breadboard (flat, glued with double-sided tape).
// Middle row:  Buck Converter in snap-in socket (10mm tall).
// Back row:    IBT_2 on 25mm standoffs + Motor in cradle.
//

$fn = 60;

// ============================================================
// Component Dimensions (adjust to match your parts)
// ============================================================

// Breadboard
bb_w = 165;
bb_h = 55;

// IBT_2 (BTS7960) Motor Driver Module
ibt_hole_spacing = 40;   // Hole pattern 40x40mm
ibt_standoff_h   = 23.5; // Printed sleeve (+ 6.5mm metal standoff = 30mm total)
ibt_standoff_d   = 8;    // Standoff outer diameter
ibt_hole_d       = 2.5;  // M3 self-tap pilot hole (2.5mm for M3 thread to grip PLA)

// Buck Converter Board
buck_l  = 37;             // PCB length
buck_w  = 17;             // PCB width
buck_t  = 1.5;            // PCB thickness
buck_socket_h  = 10;      // Socket height
buck_socket_wall = 3;     // Socket wall thickness
buck_clearance = 0;       // Walls at board width, PCB edges rest on 1.5mm groove ledge

// DFRobot FIT0185 Motor
motor_d_front  = 37;      // Gearbox diameter (near faceplate)
motor_d_rear   = 35;      // Motor body diameter (rear)
motor_d        = motor_d_front; // Used for cradle width calculation
motor_length   = 75;      // Motor body length (without shaft)
motor_hex_r    = 15.5;    // 6x M3 hex pattern circumradius
motor_shaft_d  = 14;      // Shaft + boss clearance (12mm housing + 2mm margin)

// ============================================================
// Platform Parameters
// ============================================================

margin   = 10;
gap      = 15;
plate_t  = 1;           // Minimum plate thickness

// Motor cradle
cradle_wall      = 5;
cradle_clearance = 1;

// Motor side wall (for screw mounting)
side_wall_t = 5;

// Breadboard: flat surface, glued with double-sided tape

// ============================================================
// Layout Calculation
// ============================================================

// Motor cradle dimensions
cradle_r       = motor_d / 2 + cradle_clearance;
cradle_total_w = motor_d + 2 * cradle_wall;
cradle_total_h = cradle_r + cradle_wall;

// Side wall height: motor center needs to be at cradle_total_h
// The side wall holds the motor faceplate at that height
side_wall_h = cradle_total_h + plate_t;

// Buck converter socket total dimensions (rails along full length, no end walls)
buck_socket_total_l = buck_l;
buck_socket_total_w = buck_w + buck_clearance + 2 * buck_socket_wall;

// Back row: IBT_2 (left) + Motor cradle (right)
ibt_area_w = ibt_hole_spacing + ibt_standoff_d;
ibt_area_h = ibt_hole_spacing + ibt_standoff_d;

// Middle row: Buck converter (between breadboard and back row)
buck_y = margin + bb_h + gap;
ibt_pos = [margin, buck_y + buck_socket_total_w + gap];

cradle_pos = [margin + ibt_area_w + gap,
              ibt_pos[1] + (ibt_area_h - cradle_total_w) / 2];

// Side wall at the right end of the cradle
side_wall_x = cradle_pos[0] + motor_length;

// Platform width
plate_w = max(bb_w + 2 * margin,
              side_wall_x + side_wall_t + margin);

// Front row: Breadboard centered (closest to user)
bb_pos = [(plate_w - bb_w) / 2, margin - 3];

// Buck converter position (aligned with IBT_2 center)
buck_pos = [ibt_pos[0] + (ibt_area_w - buck_socket_total_l) / 2, buck_y];

// Platform height
plate_h = ibt_pos[1] + max(ibt_area_h, cradle_total_w) + margin;

// IBT_2 hole positions (centered in ibt_area)
ibt_hole_offset_x = ibt_pos[0] + (ibt_area_w - ibt_hole_spacing) / 2;
ibt_hole_offset_y = ibt_pos[1] + (ibt_area_h - ibt_hole_spacing) / 2;
ibt_holes = [
    [ibt_hole_offset_x,                       ibt_hole_offset_y],
    [ibt_hole_offset_x + ibt_hole_spacing,     ibt_hole_offset_y],
    [ibt_hole_offset_x,                       ibt_hole_offset_y + ibt_hole_spacing],
    [ibt_hole_offset_x + ibt_hole_spacing,     ibt_hole_offset_y + ibt_hole_spacing]
];

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
// IBT_2 Standoff Sleeves
// Solid pillars with 5mm blind hole on top for metal standoffs
// to self-tap into. No through-hole needed.
// ============================================================

ibt_tap_depth = 5;       // Blind hole depth for metal standoff to screw into

ibt_flat = 1.5;  // Flatten inner face of each standoff by 1.5mm for heatsink clearance

module ibt_standoffs() {
    // Center of the 4-hole pattern
    cx = ibt_hole_offset_x + ibt_hole_spacing / 2;
    cy = ibt_hole_offset_y + ibt_hole_spacing / 2;

    for (pos = ibt_holes) {
        // Direction from this hole toward center (for inner face cut)
        dx = (cx - pos[0]) > 0 ? 1 : -1;
        dy = (cy - pos[1]) > 0 ? 1 : -1;

        translate([pos[0], pos[1], plate_t]) {
            difference() {
                cylinder(d = ibt_standoff_d, h = ibt_standoff_h);
                // Blind hole at the top
                translate([0, 0, ibt_standoff_h - ibt_tap_depth])
                    cylinder(d = ibt_hole_d, h = ibt_tap_depth + 0.1);
                // Flatten inner face Y only (heatsink clearance front/back)
                translate([-ibt_standoff_d/2 - 1,
                           dy > 0 ? ibt_standoff_d/2 - ibt_flat : -ibt_standoff_d/2 - 1,
                           -0.1])
                    cube([ibt_standoff_d + 2, ibt_flat + 1, ibt_standoff_h + 0.2]);
            }
        }
    }

    // Outline on plate for reference
    translate([ibt_hole_offset_x - ibt_standoff_d / 2,
               ibt_hole_offset_y - ibt_standoff_d / 2,
               plate_t])
        difference() {
            cube([ibt_hole_spacing + ibt_standoff_d,
                  ibt_hole_spacing + ibt_standoff_d, 0.6]);
            translate([1.5, 1.5, -0.1])
                cube([ibt_hole_spacing + ibt_standoff_d - 3,
                      ibt_hole_spacing + ibt_standoff_d - 3, 0.8]);
        }
}

// ============================================================
// Motor Cradle (2 support walls) + Side Wall (screw mount)
// Motor lies on its side, shaft extends right past side wall
// ============================================================

module motor_cradle() {
    translate([cradle_pos[0], cradle_pos[1], plate_t]) {

        // --- Rear cradle wall (40% = 30mm from left = 45mm from faceplate, ø35mm) ---
        cradle_r_rear = motor_d_rear / 2 + cradle_clearance;
        translate([motor_length * 0.4 - cradle_wall / 2, 0, 0])
            difference() {
                cube([cradle_wall, cradle_total_w, cradle_total_h]);
                translate([-1, cradle_total_w / 2, cradle_total_h])
                    rotate([0, 90, 0])
                        cylinder(r = cradle_r_rear, h = cradle_wall + 2);
            }

        // --- Front cradle wall (80% = 60mm from left = 15mm from faceplate, ø37mm) ---
        translate([motor_length * 0.8 - cradle_wall / 2, 0, 0])
            difference() {
                cube([cradle_wall, cradle_total_w, cradle_total_h]);
                translate([-1, cradle_total_w / 2, cradle_total_h])
                    rotate([0, 90, 0])
                        cylinder(r = cradle_r, h = cradle_wall + 2);
            }

        // --- Side Wall (right end, motor screws to this) ---
        translate([motor_length, 0, 0]) {
            difference() {
                cube([side_wall_t, cradle_total_w, cradle_total_h]);

                // Motor shaft hole (center of wall)
                translate([-1, cradle_total_w / 2, cradle_total_h])
                    rotate([0, 90, 0])
                        cylinder(d = motor_shaft_d, h = side_wall_t + 2);

                // 6x M3 hex pattern mounting holes
                for (i = [0:5]) {
                    angle = i * 60;
                    translate([-1,
                               cradle_total_w / 2 + motor_hex_r * sin(angle),
                               cradle_total_h + motor_hex_r * cos(angle)])
                        rotate([0, 90, 0])
                            cylinder(d = ibt_hole_d, h = side_wall_t + 2);
                }
            }
        }
    }
}

// ============================================================
// Buck Converter Snap-In Socket
// Two rails with groove for PCB, one end wall, open other end
// ============================================================

module buck_socket() {
    // PCB slides in from LEFT or RIGHT (along the 37mm length).
    // Two rails run along the full 37mm length (front + back),
    // each with a groove cut into the INNER face only.
    //
    // Cross-section of one rail:
    //   outer ←  → inner
    //   ┌────────┐
    //   │        │  ← lip (holds PCB from above)
    //   │   ┌────┘  ← groove cut 1.5mm into 3mm wall
    //   │   │ PCB   ← 1.8mm tall slot for PCB
    //   │   └────┐  ← ledge (PCB rests here)
    //   │        │
    //   └────────┘  ← base

    ledge_h    = buck_socket_h - buck_t - buck_clearance;
    groove_d   = buck_socket_wall / 2;    // Groove depth: half the wall
    inner_w    = buck_w + buck_clearance;
    w          = buck_socket_wall;

    translate([buck_pos[0], buck_pos[1], plate_t]) {
        // Front rail — groove on inner face (y = w - groove_d .. w)
        difference() {
            cube([buck_l, w, buck_socket_h]);
            translate([-0.1, w - groove_d, ledge_h])
                cube([buck_l + 0.2, groove_d + 0.1, buck_t + buck_clearance]);
        }

        // Back rail — groove on inner face (y = 0 .. groove_d)
        translate([0, w + inner_w, 0])
            difference() {
                cube([buck_l, w, buck_socket_h]);
                translate([-0.1, -0.1, ledge_h])
                    cube([buck_l + 0.2, groove_d + 0.1, buck_t + buck_clearance]);
            }
    }
}

// ============================================================
// Breadboard Area (flat surface for double-sided tape)
// ============================================================

module breadboard_area() {
    // Embossed outline showing where to stick the breadboard
    translate([bb_pos[0], bb_pos[1], plate_t])
        difference() {
            cube([bb_w, bb_h, 0.4]);
            translate([1.5, 1.5, -0.1])
                cube([bb_w - 3, bb_h - 3, 0.6]);
        }
}

// ============================================================
// Labels
// ============================================================

module labels() {
    translate([cradle_pos[0] + motor_length / 2, plate_h / 2 + 2, plate_t])
        linear_extrude(0.5)
            text("Door Controller", size = 8, halign = "center",
                 valign = "center", font = "Arial:style=Bold");
    translate([cradle_pos[0] + motor_length / 2 + 38, plate_h / 2 - 8, plate_t])
        linear_extrude(0.4)
            text("by MondayManiacs", size = 3.5, halign = "right",
                 valign = "center", font = "Arial");
}

// ============================================================
// Assembly
// ============================================================

color("SlateGray") base();
color("DimGray")   ibt_standoffs();
color("DimGray")   motor_cradle();
color("DimGray")   buck_socket();
color("White")     breadboard_area();
color("White")     labels();

// ============================================================
// Info
// ============================================================

echo(str("Platform size: ", plate_w, " x ", plate_h, " mm"));
echo(str("IBT_2 total height: ", ibt_standoff_h, " + 6.5mm metal standoff = ", ibt_standoff_h + 6.5, " mm"));
echo(str("Motor center height: ", cradle_total_h + plate_t, " mm"));
echo(str("Fits Prusa bed: ", plate_w <= 250 && plate_h <= 210 ? "YES" : "NO"));
