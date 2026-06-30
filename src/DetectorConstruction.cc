#include "DetectorConstruction.hh"
#include "Materials.hh"

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

G4VPhysicalVolume* DetectorConstruction::Construct()
{
/*
   //get NIST materials for air and core
    auto nist = G4NistManager::Instance();

    G4Material* air  = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* sty  = nist->FindOrBuildMaterial("G4_POLYSTYRENE");

    // Dummy material for cladding for now
    G4Material* clad = nist->FindOrBuildMaterial("G4_TEFLON");
*/
    auto materials = Materials::GetInstance();

    G4Material* sty = materials->GetMaterial("PolystyreneFiber");
    G4Material* clad = materials->GetMaterial("PMMA");
    G4Material* air = materials->GetMaterial("G4_AIR");

    // G4Box uses half-lengths so this gives a 2.5 m cube.
    G4double worldSizeX = 1.25 * m;
    G4double worldSizeY = 1.25 * m;
    G4double worldSizeZ = 1.25 * m;
    
    //creates the object
    auto solWorld = new G4Box(
        "WorldSol",
        worldSizeX,
        worldSizeY,
        worldSizeZ
    );
    
    //sets its logical properties (material)
    auto logWorld = new G4LogicalVolume(
        solWorld,
        air,
        "WorldLog"
    );

    //places it in the world
    auto physWorld = new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        logWorld,
        "WorldPhys",
        nullptr,
        false,
        0,
        true
    );

    //G4Tubs has inner and out radius and half-height
    G4double fiberDiam  = 1.8 * mm;
    G4double fiberRad  = 0.5 * fiberDiam;
    G4double coreRad    = 0.97 * fiberRad;
    G4double halfHeight = 500.5 * mm;

    //we can make the cladding first as a solid cylinder
    auto solClad = new G4Tubs(
        "CladSol",
        0.0 * mm, //inner radius argument
        fiberRad,
        halfHeight,
        0.0 * deg,
        360.0 * deg
    );

    auto logClad = new G4LogicalVolume(
        solClad,
        clad,
        "CladLog"
    );

    new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        logClad,
        "Cladding",
        logWorld,
        false,
        0,
        true
    );


    //the core will be a daughter volume to the cladding to not worry about overlaps
    auto solCore = new G4Tubs(
        "CoreSol",
        0.0 * mm,
        coreRad,
        halfHeight,
        0.0 * deg,
        360.0 * deg
    );

    auto logCore = new G4LogicalVolume(
        solCore,
        sty,
        "CoreLog"
    );
    
    //when we place it, any overlap with cladding gets replaced with core so effectively get solid core with cladding
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        logCore,
        "Core",
        logClad,
        false,
        0,
        true
    );

    return physWorld;
}