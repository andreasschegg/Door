// ============================================================
// Buck Converter Socket — Test Print v2
// Quick prototype to verify fit before printing full platform
// ============================================================

$fn = 60;

// Buck Converter Board
buck_l  = 37;
buck_w  = 17;
buck_t  = 1.5;

// Socket parameters (same as main platform)
buck_socket_h    = 6;       // Reduced height for quick test print
buck_socket_wall = 3;
buck_clearance   = -3;      // 14mm inner gap

groove_d   = buck_socket_wall / 2;   // 1.5mm
groove_gap = 0.3;
groove_h   = buck_t + groove_gap;    // 1.8mm
lip_h      = 1.5;
ledge_h    = buck_socket_h - groove_h - lip_h;
inner_w    = buck_w + buck_clearance; // 14mm
w          = buck_socket_wall;
lip_len    = 5;  // Lip only on right end

// Base plate
base_t = 1;

// --- Base ---
cube([buck_l, inner_w + 2 * w, base_t]);

// --- Front rail ---
translate([0, 0, base_t]) {
    // Base up to groove
    cube([buck_l, w, ledge_h]);
    // Groove area (no lip, except right 5mm)
    translate([0, 0, ledge_h])
        difference() {
            cube([buck_l - lip_len, w, groove_h]);
            translate([-0.1, w - groove_d, 0])
                cube([buck_l - lip_len + 0.2, groove_d + 0.1, groove_h + 0.1]);
        }
    // Right end with lip + groove
    translate([buck_l - lip_len, 0, 0])
        difference() {
            cube([lip_len, w, buck_socket_h]);
            translate([-0.1, w - groove_d, ledge_h])
                cube([lip_len + 0.2, groove_d + 0.1, groove_h + 0.1]);
        }
}

// --- Back rail ---
translate([0, w + inner_w, base_t]) {
    cube([buck_l, w, ledge_h]);
    translate([0, 0, ledge_h])
        difference() {
            cube([buck_l - lip_len, w, groove_h]);
            translate([-0.1, -0.1, 0])
                cube([buck_l - lip_len + 0.2, groove_d + 0.1, groove_h + 0.1]);
        }
    translate([buck_l - lip_len, 0, 0])
        difference() {
            cube([lip_len, w, buck_socket_h]);
            translate([-0.1, -0.1, ledge_h])
                cube([lip_len + 0.2, groove_d + 0.1, groove_h + 0.1]);
        }
}

echo(str("Socket size: ", buck_l, " x ", inner_w + 2*w, " x ", base_t + buck_socket_h, " mm"));
echo(str("Inner gap: ", inner_w, " mm (board: ", buck_w, " mm)"));
echo(str("Lip: ", lip_len, "mm on right end only"));
