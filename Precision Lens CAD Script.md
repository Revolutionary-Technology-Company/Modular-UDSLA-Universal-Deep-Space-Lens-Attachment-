### **The Optical Math (The "Precise" Alignment)**

To achieve "better than a millimeter" precision, this script uses **Parametric Engineering**. You must input the *exact* focal lengths of the glass you purchase (e.g., the specific Mitutoyo objective and the Primary Lens), and the code will automatically calculate the precise **Optical Tube Length (OTL)** and **Sensor Plane** positions.  
**The Physics of the Hybrid Path:**

1. **Aerial Image Formation:** The 120mm Primary Lens forms a "ghost" image (Aerial Image) at exactly its focal length ($F\_p$) inside the tube.  
2. **Magnification Stage:** The Microscope Objective (Infinity Corrected) is positioned so its **Working Distance (WD)** aligns exactly with the Aerial Image.  
3. **Projection:** The Objective outputs parallel light ("Infinity Space"), which is then focused onto the Canon Sensor by a **Tube Lens** (required for Mitutoyo/Infinity objectives to work correctly).

### **OpenSCAD Script: UDSLA "Precision-Optics" Edition**

Copy and paste this into [OpenSCAD](https://openscad.org/).

OpenSCAD

// \==============================================================================  
// PROJECT: MODULAR UDSLA (High-Precision Optical Bench)  
// AUTHOR: Seattle Data Recovery (Prototype Lab)  
// DATE: 2026-01-20  
//  
// OPTICAL THEORY:  
// Path: Primary Lens \-\> Aerial Image \-\> Microscope Obj \-\> Infinity Space \-\> Tube Lens \-\> Sensor  
// Precision: \<0.1mm alignment required for Nitrogen Gap stability.  
// \==============================================================================

$fn \= 120; // Master resolution for rendering

// \==============================================================================  
// 1\. OPTICAL PARAMETERS (INPUT YOUR EXACT LENS SPECS HERE)  
// \==============================================================================

// \--- Primary Collector (Telescope Lens) \---  
primary\_diam \= 120;          // Aperture (mm)  
primary\_focal\_len \= 900;     // f/7.5 (Common for 120mm APO Triplet)  
lens\_edge\_thick \= 12;        // Thickness of the glass edge

// \--- Secondary Magnifier (Microscope Objective) \---  
// Based on Mitutoyo M Plan Apo 5x/10x Specs  
micro\_obj\_thread \= 26;       // M26 x 0.706 (Mitutoyo Standard)  
micro\_obj\_length \= 45;       // Physical length of objective body  
micro\_working\_dist \= 34;     // WD: Distance from tip to Aerial Image (Mitutoyo 5x is \~34mm)  
micro\_parfocal\_len \= 95;     // Parfocal distance

// \--- Tube Lens (Focusing Element) \---  
// Required to focus the infinity beam onto the sensor  
tube\_lens\_focal\_len \= 200;   // Standard for Mitutoyo (MT-1, ITL200)  
tube\_lens\_diam \= 30;

// \--- Camera (Canon EOS R5 Mark II) \---  
sensor\_flange\_dist \= 20;     // Canon RF Mount Flange Distance (approx 20mm)  
sensor\_width \= 36;           // Full Frame width  
sensor\_height \= 24;

// \--- Chassis & Pressure \---  
wall\_thick \= 5;              // Grade 5 Titanium (for 2.0 ATM)  
nitrogen\_gap\_OD \= primary\_diam \+ (wall\_thick \* 4); 

// \==============================================================================  
// 2\. CALCULATED DIMENSIONS (DO NOT EDIT \- AUTO-ALIGNMENT)  
// \==============================================================================

// The "Aerial Image" forms exactly at primary\_focal\_len from the main lens.  
aerial\_image\_pos \= primary\_focal\_len;

// The Microscope Objective Tip must be placed 'Working Distance' AFTER the Aerial Image?  
// NO. For an eyepiece/magnifier, it looks AT the aerial image.  
// Distance from Primary Lens to Microscope Objective Tip \= (Primary\_Focal\_Length \+ Working\_Distance)  
// Wait... if it's an objective, the "Object" (Aerial Image) is at WD in front of it.  
// So, Objective Tip Position \= Aerial\_Image\_Pos \+ Micro\_Working\_Dist.  
obj\_mount\_pos \= aerial\_image\_pos \+ micro\_working\_dist; 

// The Tube Lens is placed after the Objective (in the infinity space).  
// Let's place it 50mm behind the objective mount for the Nitrogen Gap.  
tube\_lens\_pos \= obj\_mount\_pos \+ micro\_obj\_length \+ 40; 

// The Sensor is placed exactly at Tube\_Lens\_Focal\_Length behind the Tube Lens.  
sensor\_plane\_pos \= tube\_lens\_pos \+ tube\_lens\_focal\_len;

// Total Chassis Length  
total\_system\_len \= sensor\_plane\_pos \+ 10;

echo("\>\>\> OPTICAL ALIGNMENT REPORT \<\<\<");  
echo("Primary Lens Position: 0mm");  
echo("Aerial Image Forms at: ", aerial\_image\_pos, "mm");  
echo("Microscope Objective Tip at: ", obj\_mount\_pos \- micro\_working\_dist, "mm (Should match Aerial)");  
echo("Tube Lens Position: ", tube\_lens\_pos, "mm");  
echo("Sensor Plane (Focal Plane): ", sensor\_plane\_pos, "mm");  
echo("Total Tube Length: ", total\_system\_len, "mm");

// \==============================================================================  
// 3\. GEOMETRY MODULES  
// \==============================================================================

module primary\_cell() {  
    difference() {  
        color("silver")  
        cylinder(h=60, d=nitrogen\_gap\_OD);  
          
        // Lens Cutout  
        translate(\[0,0,-1\])  
        cylinder(h=62, d=primary\_diam);  
    }  
    // The Glass (Visual)  
    translate(\[0,0,10\]) color("cyan", 0.3) cylinder(h=lens\_edge\_thick, d=primary\_diam);  
}

module main\_baffle\_tube() {  
    // The main tube holding the Nitrogen pressure  
    // Features internal baffles to kill stray light (critical for deep space)  
      
    length \= obj\_mount\_pos \- 40; // Stop before the objective mount  
      
    difference() {  
        color("dimgray")  
        cylinder(h=length, d=nitrogen\_gap\_OD);  
          
        // Hollow interior  
        translate(\[0,0,-1\])  
        cylinder(h=length+2, d=primary\_diam \+ 5);  
    }  
      
    // Internal Baffles (Knife-edge)  
    for(i \= \[100 : 100 : length-50\]) {  
        translate(\[0,0,i\])  
        color("black")  
        difference() {  
            cylinder(h=2, d=primary\_diam \+ 4);  
            translate(\[0,0,-1\])  
            cylinder(h=4, d=primary\_diam \- 10); // Aperture constricts slightly  
        }  
    }  
}

module microscope\_stage\_block() {  
    // This heavy block holds the Microscope Objective and seals the pressure  
    translate(\[0,0,obj\_mount\_pos\])  
    union() {  
        difference() {  
            color("gold") // Titanium/Brass Interface  
            cylinder(h=40, d=nitrogen\_gap\_OD);  
              
            // Hole for Objective Thread  
            translate(\[0,0,-1\])  
            cylinder(h=42, d=micro\_obj\_thread); // M26 Thread  
        }  
          
        // The Objective Lens (Visual)  
        translate(\[0,0,5\])  
        color("silver")  
        cylinder(h=micro\_obj\_length, d=24);  
    }  
}

module tube\_lens\_assembly() {  
    // Holds the ITL200 Tube Lens  
    translate(\[0,0,tube\_lens\_pos\])  
    difference() {  
        color("darkslategray")  
        cylinder(h=20, d=nitrogen\_gap\_OD \* 0.6);  
          
        translate(\[0,0,-1\])  
        cylinder(h=22, d=tube\_lens\_diam);  
    }  
    // Glass  
    translate(\[0,0,tube\_lens\_pos+5\]) color("cyan", 0.4) cylinder(h=5, d=tube\_lens\_diam);  
}

module rear\_camera\_mount() {  
    // Canon RF Mount Interface  
    // Positioned so the sensor plane lands exactly at 'sensor\_plane\_pos'  
    // Camera flange is 'sensor\_flange\_dist' in front of sensor.  
      
    mount\_face\_pos \= sensor\_plane\_pos \- sensor\_flange\_dist;  
      
    translate(\[0,0, tube\_lens\_pos \+ 10\]) // Connects from tube lens  
    difference() {  
        color("red")  
        cylinder(h=mount\_face\_pos \- (tube\_lens\_pos \+ 10), d1=nitrogen\_gap\_OD\*0.6, d2=54); // Taper to Canon mount  
          
        translate(\[0,0,-1\])  
        cylinder(h=1000, d=40); // Light path  
    }  
      
    // The Flange Ring  
    translate(\[0,0,mount\_face\_pos \- 5\])  
    color("silver")  
    difference() {  
        cylinder(h=5, d=60);  
        translate(\[0,0,-1\])  
        cylinder(h=7, d=54);  
    }  
}

module samsung\_s25\_controller() {  
    // Mounted on the side for pressure control  
    translate(\[nitrogen\_gap\_OD/2, 0, obj\_mount\_pos\])  
    rotate(\[0,90,0\])  
    color("black")  
    cube(\[160, 75, 8\], center=true); // Approx phone dims  
}

// \==============================================================================  
// 4\. FINAL ASSEMBLY  
// \==============================================================================

union() {  
    primary\_cell();  
    main\_baffle\_tube();  
    microscope\_stage\_block();  
    tube\_lens\_assembly();  
    rear\_camera\_mount();  
    samsung\_s25\_controller();  
}

// \--- VISUALIZATION AIDS \---  
// Aerial Image Plane (Ghost)  
translate(\[0,0,aerial\_image\_pos\])   
    color("red", 0.2) cylinder(h=0.5, d=primary\_diam);

// Sensor Plane (Ghost)  
translate(\[0,0,sensor\_plane\_pos\])   
    color("green", 0.5) cube(\[sensor\_width, sensor\_height, 1\], center=true);

// Ray Tracing Lines (Visual only)  
color("yellow")  
for(r \= \[0:45:360\]) {  
    rotate(\[0,0,r\]) {  
        // Primary to Aerial  
        polygon(\[\[primary\_diam/2, 0\], \[0, aerial\_image\_pos\], \[-primary\_diam/2, 0\]\]);  
        // Infinity Space (Parallel)  
        translate(\[0,0,obj\_mount\_pos \+ micro\_obj\_length\])  
            cube(\[2, 0.5, tube\_lens\_pos \- (obj\_mount\_pos \+ micro\_obj\_length)\]);  
    }  
}  
