// ============================================================
// Motor Side Wall — Test Print
// Quick prototype to verify hole pattern and fit
// ============================================================

$fn = 60;

// Motor parameters (same as main platform)
motor_d        = 37;
motor_hex_r    = 15.5;

// Cradle dimensions (to match motor center height)
cradle_wall      = 5;
cradle_clearance = 1;
cradle_r         = motor_d / 2 + cradle_clearance;
cradle_total_w   = motor_d + 2 * cradle_wall;
cradle_total_h   = cradle_r + cradle_wall;

// Side wall
side_wall_t  = 3.5;
motor_hole_d = 3.4;      // M3 through-hole

// Base plate for test print stability
base_t = 1;
base_w = cradle_total_w + 10;

// --- Base ---
translate([-base_t, -(base_w - cradle_total_w) / 2, 0])
    cube([side_wall_t + base_t, base_w, base_t]);

// --- Side Wall ---
translate([0, 0, base_t])
    difference() {
        cube([side_wall_t, cradle_total_w, cradle_total_h - 2]);

        // 2x M3 mounting holes (bottom 2 of hex pattern)
        for (i = [0:5]) {
            angle = i * 60 + 90;
            z_pos = cradle_total_h + motor_hex_r * cos(angle);
            if (z_pos < cradle_total_h)
                translate([-1,
                           cradle_total_w / 2 + motor_hex_r * sin(angle),
                           z_pos])
                    rotate([0, 90, 0])
                        cylinder(d = motor_hole_d, h = side_wall_t + 2);
        }
    }

echo(str("Wall size: ", side_wall_t, " x ", cradle_total_w, " x ", cradle_total_h - 2, " mm"));
echo(str("Holes: 2x M3 through-hole (", motor_hole_d, "mm), radius ", motor_hex_r, " mm"));
