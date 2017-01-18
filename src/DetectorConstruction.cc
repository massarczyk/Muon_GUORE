///////////////////////////////////////////////////////////////////////////
// This code implementation is the intellectual property of the
// ton-scale 0vbb in Germanium collaboration. It is based on Geant4, an
// intellectual property of the RD44 GEANT4 collaboration.
//
// *********************
//
// Neither the authors of this software system, nor their employing
// institutes, nor the agencies providing financial support for this
// work make any representation or warranty, express or implied,
// regarding this software system or assume any liability for its use.
// By copying, distributing or modifying the Program (or any work based
// on the Program) you indicate your acceptance of this statement,
// and all its terms.
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"


   // use of stepping action to set the accounting volume

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Torus.hh"
#include "G4UnionSolid.hh"

#include "G4String.hh"
#include "math.h"

#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4Colour.hh"

#include "G4Material.hh"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{
  detectorMessenger = new DetectorMessenger(this);
	detector_type = "GERDA";
	innerVessel_FillMaterial = "NitrogenGas";
	checkOverlaps = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
 delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

	#include "Detector_Materials.icc"
	////////////////////////////////////////////////////////////////////////////////////////
	//
  // World
  //
  G4Box* solid_World = new G4Box("sol_World",50*m,50*m,30*m);
  G4LogicalVolume* logical_World = new G4LogicalVolume(solid_World,mat_air,"log_World");
	logical_World->SetVisAttributes (G4VisAttributes::Invisible);
  G4VPhysicalVolume* physical_World = new G4PVPlacement(0,G4ThreeVector(),logical_World,"phy_World",0,false,0,checkOverlaps);



	//
	G4Box* solid_Rock = new G4Box("sol_Rock",50*m,50*m,30*m);
	G4Box* solid_Lab = new G4Box("sol_Lab",35*m,10*m,4*m);
	G4SubtractionSolid *solid_Rock2 = new G4SubtractionSolid("sol_Rock2", solid_Rock, solid_Lab ,0 , G4ThreeVector(-25*m,0,10.5*m));
	G4Tubs* solid_CutOut = new G4Tubs("sol_CutOut",0, 6.50001*m ,6.50001*m, 0, 2*M_PI);
	G4SubtractionSolid *solid_Rock3 = new G4SubtractionSolid("sol_Rock2", solid_Rock2, solid_CutOut ,0 , G4ThreeVector(0,0,0));


  G4LogicalVolume* logical_Rock = new G4LogicalVolume(solid_Rock3,mat_Rock,"log_Rock");
	logical_Rock->SetVisAttributes ( new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.5) )); //grey 50% transparent
  G4VPhysicalVolume* physical_Rock = new G4PVPlacement(0,G4ThreeVector(),logical_Rock,"phy_Rock",logical_World,false,0,checkOverlaps);


	//fill gas
	G4Tubs* solid_FillGas = new G4Tubs("sol_FillGas", 0, 0.95*m ,0.95*m, 0, 2*M_PI);
	G4LogicalVolume* logical_FillGas = new G4LogicalVolume(solid_FillGas, mat_FillGas, "log_FillGas" );
	logical_FillGas->SetVisAttributes ( new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.5) )); //grey 50% transparent
	G4VPhysicalVolume* physical_FillGas =
		new G4PVPlacement(0,G4ThreeVector(0,0,0),logical_FillGas,"phy_FillGas",logical_World,false,0,checkOverlaps);

	// one detector
	G4double detector_radius = 35*mm;
	G4double detector_height = 50*mm;

	G4Tubs* solid_DetGeCrystal = new G4Tubs("sol_DetGeCrystal", 0*mm, detector_radius ,detector_height/2., 0, 2*M_PI);
	G4LogicalVolume* logical_DetGeCrystal = new G4LogicalVolume(solid_DetGeCrystal, mat_Detector, "log_DetGeCrystal" );
	logical_DetGeCrystal->SetVisAttributes ( new G4VisAttributes(G4Colour(0.1,0.9,0.1)) );


	int layer;
	int tower;
	int crystal;

	G4double detector_verticalGap        = 2*cm;
	G4double detector_horizontalGapSmall = 0.5*cm;  //in between one tower
	G4double detector_horizontalGapBig   = 4*cm;  //between the towers
	G4double detector_dX;
	G4double detector_dY;
	G4double detector_dZ;
	G4String detector_name;
	int tower_indexX[30] = {-4, 0, 4, -6,-2,2,6, -8,-4,0,4,8, -10,-6,-2,2,6,10, -8,-4, 0, 4, 8, -6,-2, 2, 6,  -4,  0,  4};
	int tower_indexY[30] = {12,12,12,  8, 8,8,8,  4, 4,4,4,4,   0, 0, 0,0,0, 0, -4,-4,-4,-4,-4, -8,-8,-8,-8, -12,-12,-12};

	//30 towers a 4 crystals in one layer !
	for (int i = 0; i<1080; i++){
	//for (int i = 0; i<2; i++){
		layer = 4-i/120;
		tower = (i%120)/4;
		crystal = (i%120)%4;
		detector_dX = tower_indexX[tower]*detector_radius + (2*tower_indexX[tower]/4)*detector_horizontalGapBig;
		detector_dY = tower_indexY[tower]*detector_radius + (tower_indexY[tower]/4)*detector_horizontalGapBig;
		detector_dZ = layer*(2*detector_height+detector_verticalGap);

		//in tower:
		//
		// 		0  1
		//		3  2
		//

		switch(crystal){
			case 0:
				detector_dX += -detector_radius-detector_horizontalGapSmall;
				detector_dY += +detector_radius+detector_horizontalGapSmall;
				break;
			case 1:
				detector_dX += +detector_radius+detector_horizontalGapSmall;
				detector_dY += +detector_radius+detector_horizontalGapSmall;
				break;
			case 2:
				detector_dX += +detector_radius+detector_horizontalGapSmall;
				detector_dY += -detector_radius-detector_horizontalGapSmall;
				break;
			case 3:
				detector_dX += -detector_radius-detector_horizontalGapSmall;
				detector_dY += -detector_radius-detector_horizontalGapSmall;
				break;
		}
		detector_name = "phy_DetGeCrystal_";
		detector_name += G4UIcommand::ConvertToString(i);
		//G4cout << i << " " << tower << " "  << detector_dX << " " << detector_dY<< " " << detector_dZ<< G4endl;
		G4ThreeVector detector_position = G4ThreeVector(detector_dX,detector_dY,detector_dZ) ;
		new G4PVPlacement(0,detector_position,logical_DetGeCrystal,detector_name,logical_FillGas,false,i,checkOverlaps);

	}


	//inner Vessel
	G4double innerVessel_Z[6] = {1*m,0.95001*m,0.95*m,-0.95*m,-0.95001*m,-1*m};
	G4double innerVessel_RMin[6] = {0*m,0*m,0.95*m,0.95*m,0*m,0*m};
	G4double innerVessel_RMax[6] = {1*m,1*m,1*m,1*m,1*m,1*m};

	G4Polycone* solid_innerVessel = new G4Polycone("sol_innerVessel", 0, 2*M_PI,6,innerVessel_Z,innerVessel_RMin,innerVessel_RMax);
	G4LogicalVolume* logical_innerVessel = new G4LogicalVolume(solid_innerVessel, mat_Cu, "log_innerVessel" );
	logical_innerVessel->SetVisAttributes ( new G4VisAttributes(G4Colour(0.62, 0.3, 0.2,0.7) ));
	G4VPhysicalVolume* physical_innerVessel =
		new G4PVPlacement(0,G4ThreeVector(0,0,0),logical_innerVessel,"phy_innerVessel",logical_World,false,0,checkOverlaps);

//shielding
	if (detector_type.contains("GERDA")){
		#include "Detector_GERDAStyle.icc"
	}
	else {
		#include "Detector_MJDStyle.icc"
	}





  return physical_World;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetOverlapsCheck(G4bool f_check)
{
	checkOverlaps = f_check;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetFillGas(G4String f_gas)
{
	innerVessel_FillMaterial = f_gas;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetShieldStyle(G4String f_type)
{
	detector_type = f_type;
}