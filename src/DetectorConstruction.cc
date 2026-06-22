#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

DetectorConstruction::DetectorConstruction()
{}

// This function builds the geometry.
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // ------------------------------------------------------------
    // 1. Get materials from the NIST database
    // ------------------------------------------------------------

    auto nist = G4NistManager::Instance();

    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");

    // ------------------------------------------------------------
    // 2. Build the world volume
    // ------------------------------------------------------------
    //
    // Geant4 boxes use half-lengths.
    // So this is a 1 m x 1 m x 1 m box.

    G4double worldSizeX = 0.5 * m;
    G4double worldSizeY = 0.5 * m;
    G4double worldSizeZ = 0.5 * m;

    auto solidWorld = new G4Box(
        "WorldSolid",
        worldSizeX,
        worldSizeY,
        worldSizeZ
    );

    auto logicWorld = new G4LogicalVolume(
        solidWorld,
        air,
        "WorldLogical"
    );

    auto physWorld = new G4PVPlacement(
        nullptr,              // no rotation
        G4ThreeVector(),      // position at origin
        logicWorld,           // logical volume
        "WorldPhysical",      // name
        nullptr,              // no mother volume
        false,                // no boolean operation
        0,                    // copy number
        true                  // check overlaps
    );

    // ------------------------------------------------------------
    // 3. Build the cylinder
    // ------------------------------------------------------------
    //
    // G4Tubs arguments:
    //
    // inner radius
    // outer radius
    // half-length in z
    // starting angle
    // spanning angle
    //
    // This cylinder has:
    // radius = 5 cm
    // full length = 10 cm

    G4double innerRadius = 0.0 * cm;
    G4double outerRadius = 5.0 * cm;
    G4double halfLengthZ = 5.0 * cm;

    auto solidCylinder = new G4Tubs(
        "CylinderSolid",
        innerRadius,
        outerRadius,
        halfLengthZ,
        0.0 * deg,
        360.0 * deg
    );

    auto logicCylinder = new G4LogicalVolume(
        solidCylinder,
        aluminum,
        "CylinderLogical"
    );

    new G4PVPlacement(
        nullptr,              // no rotation
        G4ThreeVector(),      // centered in world
        logicCylinder,        // logical volume
        "CylinderPhysical",   // name
        logicWorld,           // mother volume
        false,                // no boolean operation
        0,                    // copy number
        true                  // check overlaps
    );

    // Return the physical world volume.
    return physWorld;
}