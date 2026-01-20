// ==============================================================================
// PROJECT: UDSLA "SILO" LENS CAP (Motorized Worm Drive)
// AUTHOR: Seattle Data Recovery (Prototype Lab)
// MECHANISM: Worm Gear driven "Hydraulic" Hinge
// ==============================================================================

$fn = 100;

// --- DIMENSIONS ---
chassis_od = 140;       // Fits the UDSLA Titanium Tube
wall_thick = 5;
cap_clearance = 2;      // Gap for movement
hinge_pin_diam = 6;
servo_size = [40, 20, 36]; // Standard MG996R Servo dimensions

// --- MODULES ---

module mounting_collar() {
    // Clamps onto the front of the UDSLA
    difference() {
        color("dimgray")
        union() {
            // Main Ring
            cylinder(h=30, d=chassis_od + (wall_thick*4));
            
            // Hinge/Motor Mount Block
            translate([chassis_od/2 + 5, -25, 0])
            cube([40, 50, 30]);
        }
        
        // Inner Hole (Fits onto lens)
        translate([0,0,-1])
        cylinder(h=32, d=chassis_od);
        
        // Bolt Clamp Split
        translate([0, -50, 0])
        cube([chassis_od + 20, 2, 32]);
    }
}

module armored_hatch_lid() {
    // The "Silo Door" itself
    // Designed to look like a heavy blast shield
    
    // Rotation point matches the hinge
    translate([chassis_od/2 + 25, 0, 35]) // Pivot Point
    rotate([0, -90, 0]) // Start Closed (Horizontal)
    translate([-(chassis_od/2 + 25), 0, -35])
    
    union() {
        // The Dome Cap
        translate([0,0,32])
        difference() {
            color("silver")
            union() {
                cylinder(h=15, d=chassis_od + 10);
                // Dome top
                translate([0,0,15]) sphere(d=chassis_od + 10);
            }
            // Hollow inside to not hit lens
            translate([0,0,-1])
            cylinder(h=50, d=chassis_od - 5);
        }
        
        // Hinge Arm
        translate([chassis_od/2 + 5, -15, 32])
        color("gold")
        difference() {
            cube([40, 30, 15]);
            // Axle Hole
            translate([20, 40, 7.5]) rotate([90,0,0]) cylinder(h=50, d=hinge_pin_diam);
        }
        
        // Gear Teeth (Worm Wheel Segment) on the Hinge
        // This meshes with the worm gear to lift the lid
        translate([chassis_od/2 + 25, 0, 39.5])
        rotate([90, 0, 0])
        color("red")
        difference() {
            cylinder(h=10, d=60, center=true); // The Gear Sector
            // Only need a segment
            translate([-30, -50, -10]) cube([60, 50, 20]);
        }
    }
}

module worm_gear_drive() {
    // The "Hydraulic" Actuator simulation
    // A worm screw driven by a servo/motor
    
    translate([chassis_od/2 + 45, 0, 15])
    union() {
        // The Worm Screw
        color("white")
        rotate([0,0,0])
        cylinder(h=50, d=12); // Threaded rod rep
        
        // Spiral Thread (Visual)
        color("white")
        for(i=[0:10:360*4]) {
            translate([6*cos(i), 6*sin(i), i/30])
            sphere(d=2);
        }
        
        // Motor Housing
        translate([0,0,-35])
        color("black")
        cube([25, 25, 40], center=true); // N20 Motor size
    }
}

module hinge_pin() {
    translate([chassis_od/2 + 25, -20, 39.5])
    rotate([-90,0,0])
    color("steelblue")
    cylinder(h=40, d=hinge_pin_diam);
}

// ==============================================================================
// ASSEMBLY
// ==============================================================================

// 1. Base
mounting_collar();

// 2. The Lid (Rotated slightly to show operation)
// Change 'open_angle' to animate: 0 = Closed, 90 = Open
open_angle = 25; 

translate([chassis_od/2 + 25, 0, 39.5]) // Pivot Origin
rotate([0, open_angle, 0])              // The Opening Action
translate([-(chassis_od/2 + 25), 0, -39.5])
armored_hatch_lid();

// 3. The Drive System
worm_gear_drive();

// 4. Pin
hinge_pin();

// --- LABELS ---
color("white")
translate([0, -80, 50])
rotate([90,0,0])
text("SILO CAP - WORM DRIVE", size=8, halign="center");
