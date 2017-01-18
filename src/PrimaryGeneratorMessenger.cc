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
#include "globals.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"

// Include all generators here
#include "ShowerGenerator.hh"
#include "G4GeneralParticleSource.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* generator)
: fPrimaryGenerator(generator)
{
  genDir = new G4UIdirectory("/GUORE/generator/");
  genDir->SetGuidance("Primary generator selection");
  genDir->SetGuidance("/GUORE/generator/select: Select generator.");

  // /GUORE/generator/select command
  fSelectCmd = new G4UIcmdWithAString("/GUORE/generator/select", this);
  fSelectCmd->SetGuidance("Selects generator for events.");
  fSelectCmd->SetGuidance("Options are:");
  fSelectCmd->SetGuidance("Muon");
  fSelectCmd->SetGuidance("GPS");
  fSelectCmd->SetCandidates("Muon GPS");
  fSelectCmd->SetDefaultValue("Muon");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete genDir;
  delete fSelectCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == fSelectCmd )
  {
      if(newValue == "Muon")
      {
        fPrimaryGenerator->SetGenerator(new ShowerGenerator);
      }
      else if(newValue == "GPS")
      {
        fPrimaryGenerator->SetGenerator(new G4GeneralParticleSource);
      }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......