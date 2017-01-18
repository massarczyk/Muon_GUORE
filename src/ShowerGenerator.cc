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
//////////////////////////////////////////////////////////////////////////////
/// \file ShowerGenerator.cc
/// \brief Implementation of the ShowerGenerator class

#include "ShowerGenerator.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include <math.h>

#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4Neutron.hh"
#include "G4Alpha.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4KaonZeroLong.hh"
#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4AntiNeutron.hh"
#include "G4Deuteron.hh"

#include "TFile.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>
#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ShowerGenerator::ShowerGenerator()
{
  const char* muon_path = std::getenv("G4MUONDATA");
  inputfile = new TFile(Form("%s/Muonbackground_shielding.root", muon_path) ,"READ");
  datatree = (TTree*)inputfile->Get("tree");

  max_entries = datatree->GetEntries();

  start_energy = 0;    //muon start energy in surface GeV
  start_costheta = 0;    //muon start angle
  particle_energy = NULL;
  particle_momentumX = NULL;  //GeV
  particle_momentumY = NULL;  //GeV
  particle_momentumZ = NULL;  //GeV
  particle_name = NULL;

   datatree->SetBranchAddress("start_energy", &start_energy);
   datatree->SetBranchAddress("start_costheta", &start_costheta);
   datatree->SetBranchAddress("particle_energy", &particle_energy);
   datatree->SetBranchAddress("particle_momentumX", &particle_momentumX);
   datatree->SetBranchAddress("particle_momentumY", &particle_momentumY);
   datatree->SetBranchAddress("particle_momentumZ", &particle_momentumZ);
   datatree->SetBranchAddress("particle_name", &particle_name);

	 G4cout << max_entries << " " << inputfile->GetName() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ShowerGenerator::~ShowerGenerator()
{
  delete inputfile;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ShowerGenerator::GeneratePrimaryVertex(G4Event* anEvent)
{
  // Select random entry within file
  int ev_ID = (int)(G4UniformRand()*max_entries);
  datatree->GetEntry(ev_ID);

  //for all shower particles
  int nRealGen = 0;
  int nEvents = particle_energy->size();

  for (int j=0; j < nEvents; j++) {
  particle_time = 0.0*s;

  // Generate on a square above the experiment
  G4double fXPosition = (1. - 2.*G4UniformRand())*15;
  G4double fYPosition = (1. - 2.*G4UniformRand())*15;

  particle_position.setX(fXPosition*m);
  particle_position.setY(fYPosition*m);
  particle_position.setZ(14.0*m);

	G4PrimaryVertex* vertex = new G4PrimaryVertex(particle_position,particle_time);

	G4ParticleDefinition* particle_definition = 0;
	if (!(particle_name->at(j).compare("mu-"))) particle_definition = G4MuonMinus::MuonMinusDefinition();
	if (!(particle_name->at(j).compare("neutron"))) particle_definition = G4Neutron::NeutronDefinition();
  if (!(particle_name->at(j).compare("alpha"))) particle_definition = G4Alpha::AlphaDefinition();
  if (!(particle_name->at(j).compare("gamma"))) particle_definition = G4Gamma::GammaDefinition();
  if (!(particle_name->at(j).compare("e-"))) particle_definition = G4Electron::ElectronDefinition();
  if (!(particle_name->at(j).compare("e+"))) particle_definition = G4Positron::PositronDefinition();
  if (!(particle_name->at(j).compare("mu+"))) particle_definition = G4MuonPlus::MuonPlusDefinition();
  if (!(particle_name->at(j).compare("anti_neutron"))) particle_definition = G4AntiNeutron::AntiNeutronDefinition();
  if (!(particle_name->at(j).compare("kaon+"))) particle_definition = G4KaonPlus::KaonPlusDefinition();
  if (!(particle_name->at(j).compare("kaon-"))) particle_definition = G4KaonMinus::KaonMinusDefinition();
  if (!(particle_name->at(j).compare("kaon0L"))) particle_definition = G4KaonZeroLong::KaonZeroLongDefinition();
  if (!(particle_name->at(j).compare("pi+"))) particle_definition = G4PionPlus::PionPlusDefinition();
  if (!(particle_name->at(j).compare("pi-"))) particle_definition = G4PionMinus::PionMinusDefinition();
  if (!(particle_name->at(j).compare("nu_e"))) particle_definition = G4NeutrinoE::NeutrinoEDefinition();
  if (!(particle_name->at(j).compare("nu_mu"))) particle_definition = G4NeutrinoMu::NeutrinoMuDefinition();
  if (!(particle_name->at(j).compare("anti_nu_e"))) particle_definition = G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  if (!(particle_name->at(j).compare("anti_nu_mu"))) particle_definition = G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
  if (!(particle_name->at(j).compare("deuteron"))) particle_definition = G4Deuteron::DeuteronDefinition();
  if (particle_definition == 0) continue;

  // Set momenta, no rotations done here (unlike in MaGe)
  double px_MJD = particle_momentumX->at(j);
  double py_MJD = particle_momentumY->at(j);
  double pz_MJD = particle_momentumZ->at(j);
  double pEnergy = particle_energy->at(j);

  // Set a 500 keV threshold if too many particles, reduces the number of particles generated
  // Do this because of memory issues, not the most elegant solution but works
  if(pEnergy < 1e-05)continue; // 10 keV cutoff
  if(nEvents > 100000 && pEnergy < 5E-04)continue;
  nRealGen++;

  G4ThreeVector momentum(px_MJD*GeV,py_MJD*GeV,pz_MJD*GeV);

	G4PrimaryParticle* thePrimaryParticle =
    new G4PrimaryParticle(particle_definition,
						  px_MJD*GeV,
						  py_MJD*GeV,
						  pz_MJD*GeV);
	vertex->SetPrimary(thePrimaryParticle);
  // vertex->SetWeight(Distribution(start_energy,start_costheta));  // Totally useless for now

  anEvent->AddPrimaryVertex(vertex);
  }

  // Output information, maybe not so useful
   G4cout << "Entry #: " << ev_ID << "\t Length: " << nEvents << "\t Generated:" << nRealGen << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//weighting with incident muon distribution
//https://escholarship.org/uc/item/6jm8g76d#page-3
double ShowerGenerator::Distribution(double Energy, double CosTheta)
{
  double CosThetaStar = sqrt(
	(pow(CosTheta,2) + pow(0.102573,2) -0.068287*pow(CosTheta,0.958633)+0.0407253*pow(CosTheta,0.817285) )/
	(1+pow(0.102573,2)-0.068287+0.0407253));

  double I;
  I = 0.14*pow(Energy*(1+3.64/(Energy*pow(CosThetaStar,1.29))),-2.7)
	  *((1./(1+(1.1*Energy*CosThetaStar)/115.))+(0.054/(1+(1.1*Energy*CosThetaStar)/850.)));

  return (I);
}


