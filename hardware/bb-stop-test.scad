// ============================================================
// Breadboard Side Stop — Test Print
// One side wall with two corner profiles
// ============================================================

$fn = 60;

bb_h      = 55;       // Breadboard depth
bb_stop_h = 7;        // Stop height
bb_stop_w = 3;        // Wall thickness
bb_corner = 7;        // Corner profile toward board center
base_t    = 1;        // Base plate

// --- Base ---
translate([-bb_stop_w, -bb_stop_w, 0])
    cube([bb_stop_w + bb_corner + 5, bb_h + 2 * bb_stop_w, base_t]);

// --- Side wall ---
translate([-bb_stop_w, 0, base_t])
    cube([bb_stop_w, bb_h, bb_stop_h]);

// --- Front corner ---
translate([-bb_stop_w, -bb_stop_w, base_t])
    cube([bb_stop_w + bb_corner, bb_stop_w, bb_stop_h]);

// --- Back corner ---
translate([-bb_stop_w, bb_h, base_t])
    cube([bb_stop_w + bb_corner, bb_stop_w, bb_stop_h]);

echo(str("Test piece: ", bb_stop_w + bb_corner + 5, " x ", bb_h + 2 * bb_stop_w, " x ", base_t + bb_stop_h, " mm"));
