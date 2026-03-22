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
//   |  [Breadboard 165x55]  ← taped down       |
//   |  (Arduino Nano ESP32 plugs into it)      |
//   +-------------------------------------------+
//          ↑ user sits here
//
// Front row: Breadboard (flat, glued with double-sided tape).
// Back row:  IBT_2 on 25mm standoffs + Motor in cradle.
// Motor screwed to side wall via 6x M3 hex pattern.
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
ibt_standoff_h   = 25;   // Standoff sleeve height
ibt_standoff_d   = 8;    // Standoff outer diameter
ibt_hole_d       = 3.2;  // M3 through-hole

// DFRobot FIT0185 Motor
motor_d        = 37;      // Motor body diameter
motor_length   = 75;      // Motor body length (without shaft)
motor_hex_r    = 15.5;    // 6x M3 hex pattern circumradius
motor_shaft_d  = 10;      // Shaft clearance hole

// ============================================================
// Platform Parameters
// ============================================================

margin   = 15;
gap      = 15;
plate_t  = 3;

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

// Back row: IBT_2 (left) + Motor cradle (right)
ibt_area_w = ibt_hole_spacing + ibt_standoff_d;
ibt_area_h = ibt_hole_spacing + ibt_standoff_d;
ibt_pos    = [margin, margin + bb_h + gap];

cradle_pos = [margin + ibt_area_w + gap,
              ibt_pos[1] + (ibt_area_h - cradle_total_w) / 2];

// Side wall at the right end of the cradle
side_wall_x = cradle_pos[0] + motor_length;

// Platform width
plate_w = max(bb_w + 2 * margin,
              side_wall_x + side_wall_t + margin);

// Front row: Breadboard centered (closest to user, y=margin)
bb_pos = [(plate_w - bb_w) / 2, margin];

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
    difference() {
        rounded_plate(plate_w, plate_h, plate_t);

        // IBT_2 M3 through-holes
        for (pos = ibt_holes) {
            translate([pos[0], pos[1], -0.1])
                cylinder(d = ibt_hole_d, h = plate_t + 0.2);
        }
    }
}

// ============================================================
// IBT_2 Standoff Sleeves (25mm tall, M3 through-hole)
// Board screws on top, heatsink hangs below board in the air
// ============================================================

module ibt_standoffs() {
    for (pos = ibt_holes) {
        translate([pos[0], pos[1], plate_t]) {
            difference() {
                cylinder(d = ibt_standoff_d, h = ibt_standoff_h);
                translate([0, 0, -0.1])
                    cylinder(d = ibt_hole_d, h = ibt_standoff_h + 0.2);
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

        // --- 2 Cradle Support Walls ---
        for (x_offset = [motor_length * 0.2, motor_length * 0.65]) {
            translate([x_offset - cradle_wall / 2, 0, 0])
                difference() {
                    cube([cradle_wall, cradle_total_w, cradle_total_h]);
                    translate([-1, cradle_total_w / 2, cradle_total_h])
                        rotate([0, 90, 0])
                            cylinder(r = cradle_r, h = cradle_wall + 2);
                }
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
// Breadboard Area (flat surface for double-sided tape)
// ============================================================

module breadboard_area() {
    // Embossed outline showing where to stick the breadboard
    translate([bb_pos[0], bb_pos[1], plate_t])
        difference() {
            cube([bb_w, bb_h, 0.6]);
            translate([1.5, 1.5, -0.1])
                cube([bb_w - 3, bb_h - 3, 0.8]);
        }
}

// ============================================================
// Labels
// ============================================================

module labels() {
    translate([plate_w / 2,
               bb_pos[1] + bb_h / 2, plate_t])
        linear_extrude(0.5)
            text("Door Controller", size = 8, halign = "center",
                 valign = "center", font = "Arial:style=Bold");
}

// ============================================================
// Assembly
// ============================================================

color("SlateGray") base();
color("DimGray")   ibt_standoffs();
color("DimGray")   motor_cradle();
color("White")     breadboard_area();
color("White")     labels();

// ============================================================
// Info
// ============================================================

echo(str("Platform size: ", plate_w, " x ", plate_h, " mm"));
echo(str("IBT_2 standoff height: ", ibt_standoff_h, " mm"));
echo(str("Motor center height: ", cradle_total_h + plate_t, " mm"));
echo(str("Fits Prusa bed: ", plate_w <= 250 && plate_h <= 210 ? "YES" : "NO"));
