#include "Materials.hh"

#include "G4MaterialPropertiesTable.hh"
#include "G4SystemOfUnits.hh"

Materials::Materials()
{
  nistMan = G4NistManager::Instance();

  nistMan->SetVerbose(2);

  CreateMaterials();
}

Materials::~Materials()
{
  delete    PMMA;
  delete    PolystyreneFiber;
}

Materials* Materials::instance = NULL;

Materials* Materials::GetInstance()
{
  if(instance == 0) instance = new Materials();
  return instance;
}

G4Material* Materials::GetMaterial(const G4String material)
{
  G4Material* mat =  nistMan->FindOrBuildMaterial(material);

  if (!mat) mat = G4Material::GetMaterial(material);
  if (!mat)
  {
     std::ostringstream o;
     o << "Material " << material << " not found!";
     G4Exception("Materials::GetMaterial","", FatalException,o.str().c_str());
  }

  return mat;
}

void Materials::CreateMaterials()
{
  G4double density;
  std::vector<G4int> natoms;
  std::vector<G4String> elements;

  // Materials Definitions
  // =====================

  //--------------------------------------------------
  // Vacuum
  //--------------------------------------------------

  nistMan->FindOrBuildMaterial("G4_Galactic");

  //--------------------------------------------------
  // Air
  //--------------------------------------------------

  Air = nistMan->FindOrBuildMaterial("G4_AIR");

  //--------------------------------------------------
  // PMMA, C5H8O2
  //--------------------------------------------------

  elements.push_back("C");     natoms.push_back(5);
  elements.push_back("H");     natoms.push_back(8);
  elements.push_back("O");     natoms.push_back(2);

  density = 1.190*g/cm3;

  PMMA = nistMan->ConstructNewMaterial("PMMA", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------
  // Polystyrene, C8H8
  //--------------------------------------------------

  elements.push_back("C");     natoms.push_back(8);
  elements.push_back("H");     natoms.push_back(8);

  density = 1.050*g/cm3;

  PolystyreneFiber =
      nistMan->ConstructNewMaterial("PolystyreneFiber",
                                    elements,
                                    natoms,
                                    density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------
  // Optical properties
  //--------------------------------------------------

  const G4int nEntries = 2;

  //425 nm -> 2.92 eV so 2-4 eV is good range
  G4double photonEnergy[nEntries] = {2.0*eV, 4.0*eV};

  //--------------------------------------------------
  // Air
  //--------------------------------------------------

  G4double rIndexAir[nEntries] = {1.00, 1.00};

  auto mptAir = new G4MaterialPropertiesTable();
  mptAir->AddProperty("RINDEX", photonEnergy, rIndexAir, nEntries);

  Air->SetMaterialPropertiesTable(mptAir);

  //--------------------------------------------------
  // PMMA
  //--------------------------------------------------

  G4double rIndexPMMA[nEntries] = {1.49, 1.49};

  auto mptPMMA = new G4MaterialPropertiesTable();
  mptPMMA->AddProperty("RINDEX", photonEnergy, rIndexPMMA, nEntries);

  PMMA->SetMaterialPropertiesTable(mptPMMA);

  //--------------------------------------------------
  // Polystyrene
  //--------------------------------------------------

  G4double rIndexPolystyrene[nEntries] = {1.59, 1.59};

  auto mptPolystyrene = new G4MaterialPropertiesTable();
  mptPolystyrene->AddProperty("RINDEX", photonEnergy, rIndexPolystyrene, nEntries);

  PolystyreneFiber->SetMaterialPropertiesTable(mptPolystyrene);
}