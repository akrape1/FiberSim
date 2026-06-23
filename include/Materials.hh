#ifndef MATERIALS_HH
#define MATERIALS_HH 

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

class Materials
{
  public:

    ~Materials();
 
    static Materials* GetInstance();

    G4Material* GetMaterial(const G4String);
 
  private:
 
    Materials();

    void CreateMaterials();

  private:

    static Materials* instance;

    G4NistManager*     nistMan;

    G4Material*        Air;
    G4Material*        PMMA;
    G4Material*        PolystyreneFiber;
};

#endif 