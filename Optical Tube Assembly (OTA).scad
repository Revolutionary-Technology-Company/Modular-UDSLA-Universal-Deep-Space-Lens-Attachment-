// --- TELESCOPE & LENS PARAMETERS ---
$fn = 80; // Smoothness

// OPTICS
lens_diameter = 100;      // Diameter of your main objective lens (mm)
lens_thickness = 15;      // Edge thickness of the lens
tube_length = 300;        // Main body length (approx focal length - focuser travel)
wall_thickness = 4;

// FOCUS MECHANISM
drawtube_travel = 60;     // How far the focuser moves
rail_diameter = 6;        // Smooth rods for linear tracking
motor_size = 42.3;        // NEMA 17 standard size

// CANON MOUNT (Approximate EF dimensions)
mount_diameter = 54;      // Canon EF internal diameter
flange_thickness = 5;

// --- CALCULATED VALUES ---
outer_radius = (lens_diameter / 2) + wall_thickness;
inner_radius = lens_diameter / 2;
drawtube_radius = inner_radius - 10; // Slightly smaller to fit inside

// --- MAIN ASSEMBLY ---
union() {
    // 1. MAIN TELESCOPE BODY
    color("White") 
        translate([0, 0, 0]) 
        main_tube();

    // 2. OBJECTIVE LENS CELL (Front)
    color("DimGray") 
        translate([0, 0, tube_length]) 
        lens_cell();
    
    // 3. FOCUSING DRAWTUBE (Rear - Moving Part)
    // Change '30' to animate the focus in preview
    color("Silver") 
        translate([0, 0, -30]) 
        drawtube_assembly();

    // 4. FOCUS MOTOR MOUNT (Attached to Main Body)
    color("Orange") 
        translate([outer_radius + 5, 0, 20]) 
        rotate([0, 0, 90])
        focus_motor_housing();
        
    // 5. INTERNAL TRACKS (Guide Rails)
    color("Blue") 
        internal_tracks();
}

// --- MODULES ---

module main_tube() {
    difference() {
        // Outer Shell
        cylinder(h=tube_length, r=outer_radius);
        
        // Inner Hollow (Optical Path)
        translate([0, 0, -1])
            cylinder(h=tube_length + 2, r=inner_radius);
        
        // Cutout for Focuser Gear interacting with Drawtube
        translate([inner_radius - 5, -10, 10])
            cube([20, 20, 30]); // Slot for pinion gear
    }
}

module lens_cell() {
    // Holds the main objective lens
    difference() {
        union() {
            // Lens Cap/Hood Threads area
            cylinder(h=40, r=outer_radius + 2);
            // Retaining Lip
            translate([0, 0, 0])
                cylinder(h=5, r=inner_radius - 2); 
        }
        
        // The Glass Slot
        translate([0, 0, 5])
            cylinder(h=lens_thickness, r=lens_diameter/2 + 0.5); // +tolerance
            
        // Light Path
        translate([0, 0, -1])
            cylinder(h=50, r=lens_diameter/2 - 2); // Stop down slightly
    }
}

module drawtube_assembly() {
    union() {
        difference() {
            // Drawtube Body
            cylinder(h=100, r=drawtube_radius);
            
            // Hollow interior (baffled/matte)
            translate([0, 0, -1])
                cylinder(h=102, r=drawtube_radius - 3);
        }
        
        // Integrated Rack Gear (Visual representation)
        translate([drawtube_radius - 2, -5, 10])
            cube([4, 10, 80]); 
            
        // Canon Mount Interface at the end
        translate([0, 0, -5])
            canon_mount();
    }
}

module internal_tracks() {
    // Smooth Rods for the drawtube to slide on without wobbling
    // Rod 1
    translate([outer_radius - wall_thickness*2, 0, 0])
        cylinder(h=100, r=rail_diameter/2);
    
    // Rod 2 (120 degrees apart)
    rotate([0, 0, 120])
    translate([outer_radius - wall_thickness*2, 0, 0])
        cylinder(h=100, r=rail_diameter/2);
        
    // Rod 3 (240 degrees apart)
    rotate([0, 0, 240])
    translate([outer_radius - wall_thickness*2, 0, 0])
        cylinder(h=100, r=rail_diameter/2);
}

module focus_motor_housing() {
    difference() {
        // Block to hold NEMA 17
        translate([-motor_size/2, -motor_size/2, 0])
            cube([motor_size + 10, motor_size, 10]);
            
        // Motor Shaft Hole
        cylinder(h=20, r=12, center=true);
        
        // Mounting Screw Holes
        for(i=[0:3]) {
            rotate([0, 0, 90*i])
            translate([15.5, 15.5, -10])
            cylinder(h=30, r=1.7);
        }
    }
}

module canon_mount() {
    // Simplified Canon EF Flange
    difference() {
        cylinder(h=flange_thickness, r=mount_diameter/2 + 4);
        translate([0, 0, -1])
            cylinder(h=flange_thickness + 2, r=mount_diameter/2);
    }
    
    // The Bayonet Lugs (3 tabs)
    intersection() {
        cylinder(h=2, r=mount_diameter/2);
        for(i=[0:2]) {
            rotate([0, 0, i*120])
            translate([mount_diameter/2 - 2, -5, 0])
            cube([5, 10, 2]);
        }
    }
}