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
///////////////////////////////////////////////////////////////////////////
#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det)
: Detector(Det)
{
  detDir = new G4UIdirectory("/exp/");
  detDir->SetGuidance("experiment control");

  UpdateCmd = new G4UIcmdWithoutParameter("/exp/update",this);
  UpdateCmd->SetGuidance("Update geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);

	OverlapsCmd = new G4UIcmdWithABool("/exp/checkOverlaps",this);
  OverlapsCmd->SetGuidance("switch on check overlaps");
	OverlapsCmd->SetParameterName("checkOverlaps",false);
	OverlapsCmd->SetDefaultValue(false);
	OverlapsCmd->AvailableForStates(G4State_Idle);

	StyleCmd = new G4UIcmdWithAString("/exp/setShieldingStyle",this);
  StyleCmd->SetGuidance("switch shielding style (MJD / GERDA)");
	StyleCmd->SetGuidance("candidates: MJD, GERDA");
	StyleCmd->SetParameterName("ShieldStyle",false);
	StyleCmd->SetCandidates("MJD GERDA");
	StyleCmd->AvailableForStates(G4State_Idle);

	FillGasCmd = new G4UIcmdWithAString("/exp/setFillGas",this);
  FillGasCmd->SetGuidance("set material around detectors in the inner vessel");
	FillGasCmd->SetGuidance("candidates: Vacuum ArgonLiquid ArgonGas NitrogenLiquid NitrogenGas");
	FillGasCmd->SetParameterName("FillGas",false);
	FillGasCmd->SetCandidates("Vacuum ArgonLiquid ArgonGas NitrogenLiquid NitrogenGas");
	FillGasCmd->AvailableForStates(G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete UpdateCmd;
	delete OverlapsCmd;
	delete StyleCmd;
	delete FillGasCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == UpdateCmd ) Detector->UpdateGeometry();
	else if( command == OverlapsCmd ){
		Detector->SetOverlapsCheck(OverlapsCmd->GetNewBoolValue(newValue));
	}
	else if( command == StyleCmd ){
		Detector->SetShieldStyle(newValue);
	}
	else if( command == FillGasCmd ){
		Detector->SetFillGas(newValue);
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......