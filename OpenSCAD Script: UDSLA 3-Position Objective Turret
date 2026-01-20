// ==============================================================================
// PROJECT: UDSLA ROTARY TURRET (Triple Objective Changer)
// AUTHOR: Seattle Data Recovery (Prototype Lab)
// ==============================================================================

$fn = 100; // High resolution

// --- DIMENSIONS ---
turret_diam = 110;
turret_thickness = 12;
num_ports = 3;
thread_type = 26; // M26 (Mitutoyo) or 20.32 (RMS)
mount_bolt_circle = 125;
chassis_OD = 140; // Matches previous UDSLA Nitrogen Gap OD

// --- MODULES ---

module objective_thread_cutter() {
    // Standard M26 x 0.706 Thread for Mitutoyo
    cylinder(h=20, d=thread_type, center=true);
}

module turret_rotor() {
    difference() {
        // Main Disk
        color("dimgray")
        union() {
            cylinder(h=turret_thickness, d=turret_diam);
            // Knurled Grip Edge (Simulated)
            for(i=[0:10:360]) {
                rotate([0,0,i])
                translate([turret_diam/2, 0, turret_thickness/2])
                cylinder(h=turret_thickness, d=3, center=true);
            }
        }
        
        // Objective Ports
        for(i = [0 : 360/num_ports : 360]) {
            rotate([0,0,i])
            translate([turret_diam/2 - 25, 0, 0])
            objective_thread_cutter();
        }
        
        // Center Pivot Hole
        cylinder(h=50, d=10, center=true);
        
        // Detent Divots (Bottom side)
        for(i = [0 : 360/num_ports : 360]) {
            rotate([0,0,i])
            translate([turret_diam/2 - 25, 0, -turret_thickness/2])
            sphere(d=6); // Ball bearing detent
        }
    }
}

module stator_backplate() {
    // This part seals the Nitrogen Gap and holds the Turret
    difference() {
        color("gold") // Titanium Interface
        union() {
            cylinder(h=15, d=chassis_OD);
            // Pivot Shaft
            translate([0,0,15])
            cylinder(h=10, d=9.8); // Fits into rotor
        }
        
        // Light Path Pass-through (Only ONE hole active at a time)
        // Aligned with the "Top" position of the turret
        translate([turret_diam/2 - 25, 0, -10])
        cylinder(h=50, d=30); // Clear aperture for light
        
        // Mounting Bolt Holes (to Chassis)
        for(i=[0:60:360]) {
            rotate([0,0,i])
            translate([mount_bolt_circle/2, 0, 0])
            cylinder(h=50, d=6, center=true);
        }
    }
    
    // Detent Spring Mechanism (Visual rep)
    translate([turret_diam/2 - 25, 15, 15])
    color("red")
    sphere(d=5); // Represents the spring-loaded ball bearing
}

module objectives_visual() {
    // Visualization of lenses installed in the turret
    rotate([0,0,0]) { // Position 1 (Active)
        translate([turret_diam/2 - 25, 0, 6])
        color("silver")
        cylinder(h=40, d1=24, d2=20); // 5x Obj
    }
    
    rotate([0,0,120]) { // Position 2
        translate([turret_diam/2 - 25, 0, 6])
        color("black")
        cylinder(h=35, d1=24, d2=18); // 10x Obj
    }
    
    rotate([0,0,240]) { // Position 3
        translate([turret_diam/2 - 25, 0, 6])
        color("silver")
        cylinder(h=30, d1=24, d2=15); // 20x Obj
    }
}

// --- ASSEMBLY ---

// 1. The Stationary Plate (Bolts to UDSLA Tube)
translate([0,0,-16])
stator_backplate();

// 2. The Rotating Turret
rotate([0,0,40]) // Rotated slightly to show it moves
turret_rotor();

// 3. The Pivot Bolt
color("silver")
translate([0,0,12])
cylinder(h=5, d=20); // Cap

// 4. Installed Lenses (Visual Only)
rotate([0,0,40])
objectives_visual();
