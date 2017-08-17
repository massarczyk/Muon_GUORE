///////////////////////////////////////////////////////////////////////////
// This code implementation is the intellectual property of the
// ton-scale 0vbb in Germanium collaboration. It is based on Geant4, an
// intellectual property of the RD44 GEANT4 collaboration.
//
// *********************
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// Neither the authors of this software system, nor their employing
// institutes, nor the agencies providing financial support for this
// work make any representation or warranty, express or implied,
// regarding this software system or assume any liability for its use.
// By copying, distributing or modifying the Program (or any work based
// on the Program) you indicate your acceptance of this statement,
// and all its terms.
///////////////////////////////////////////////////////////////////////////
/// \file Muon_GUORE.cc
/// \brief Main program of the  example

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include <G4GeneralParticleSource.hh>

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "Shielding.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "Randomize.hh"


//V 1.02 requires a random seed to be input after the first macro argument
//It uses C++'s class system to safely pass variables between different methods
//This version also has an option for hits in the LAr as a channel. This is quite memory intensive.
//There is NO summing in this version. Cuts are implemented on a case by case basis.



//Some bare bones templates copied directly from the Geant4 Users' Guide
#include <G4UserRunAction.hh>
#include <G4UImessenger.hh>
#include <TFile.h>
#include <TTree.h>
#include <G4UserEventAction.hh>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4EventManager.hh"
#include <iostream>
#include <time.h>
#include<stdlib.h>

const int maxsteps=1000000;
int currentsteps=0; //global ints just to keep track of file size limits

//////////////////////////////////////////////////

//over-class IO that all action classes inherit from, to pass information for writing etc.

class IO : public G4UImessenger
{

public:

  //constructor and destructor
  IO();
  ~IO(){};

  //member functions
  void Open(char* inputseed);
  void Write(G4Track *track, int isadetector, int isentry);
  void Close();

  //output file
  TFile *fFile;
  TTree *fTree;

  //variables to write to output file
  int PID;
  double energy;
  double kineticenergy;
  double deltaenergy;
  double time;
  double x;
  double y;
  double z;
  double px;
  double py;
  double pz;
  int eventnumber;
  int stepnumber;
  int tracknumber;
  int detectornumber;
  int isentryevent;
  std::string creatorprocess;
  std::string material;
  double startx;
  double starty;
  double startz;
  
  //variables specifically to parse detector number from detector name (used in the Write method)
  G4String name;
  G4String num;
  int j;

  char outputfilename[100];
};



IO::IO(): G4UImessenger()  {    G4cout << "IO created" << G4endl; }


void IO::Open(char* inputseed)
{
  
  //Name output file based on the seed number given at the start of the program
  char prefix[10] = "output";
  char suffix[10] = ".root";
  
  strcat(outputfilename, prefix);
  strcat(outputfilename, inputseed);
  strcat(outputfilename, suffix);

  G4cout << outputfilename << G4endl;
  
  fFile = new TFile(outputfilename,"recreate");
  fTree = new TTree("fTree","fTree");

  //Branches to be filled
  fTree->Branch("PID",&PID,"PID/I");
  fTree->Branch("energy",&energy,"energy/D");
  fTree->Branch("kineticenergy", &kineticenergy, "kineticenergy/D");
  fTree->Branch("deltaenergy",&deltaenergy,"deltaenergy/D");
  fTree->Branch("time",&time,"time/D");
  fTree->Branch("x",&x,"x/D");
  fTree->Branch("y",&y,"y/D");
  fTree->Branch("z",&z,"z/D");
  fTree->Branch("px",&px,"px/D");
  fTree->Branch("py",&py,"py/D");
  fTree->Branch("pz",&pz,"pz/D");
  fTree->Branch("eventnumber",&eventnumber,"eventnumber/I");
  fTree->Branch("stepnumber",&stepnumber,"stepnumber/I");
  fTree->Branch("tracknumber",&tracknumber,"tracknumber/I");
  fTree->Branch("detectornumber",&detectornumber,"detectornumber/I");
  fTree->Branch("isentryevent", &isentryevent, "isentryevent/I");
  fTree->Branch("creatorprocess", &creatorprocess);
  fTree->Branch("material", &material);
  fTree->Branch("startx",&startx,"startx/D");
  fTree->Branch("starty",&starty,"starty/D");
  fTree->Branch("startz",&startz,"startz/D");
    
  
  G4cout << "Output file opened." << G4endl;

}


void IO::Write(G4Track *track, int isadetector, int isentry)
{
  
  PID = track->GetDefinition()->GetPDGEncoding();
  energy = track->GetStep()->GetTotalEnergyDeposit()/CLHEP::keV;
  kineticenergy = track->GetDynamicParticle()->GetKineticEnergy()/CLHEP::keV;
  deltaenergy = track->GetStep()->GetDeltaEnergy()/CLHEP::keV;
  time = track->GetGlobalTime()/CLHEP::ns;

  if(isentry==0) //Step did NOT start in LAr and end up in Ge
    {
      x = track->GetStep()->GetPreStepPoint()->GetPosition().x()/CLHEP::mm;
      y = track->GetStep()->GetPreStepPoint()->GetPosition().y()/CLHEP::mm;
      z = track->GetStep()->GetPreStepPoint()->GetPosition().z()/CLHEP::mm;
      material = track->GetStep()->GetPreStepPoint()->GetMaterial()->GetName();
    }

  if(isentry==1) //Step started in LAr and ended in Ge; so called 'entry event'
    {
      x = track->GetStep()->GetPostStepPoint()->GetPosition().x()/CLHEP::mm;
      y = track->GetStep()->GetPostStepPoint()->GetPosition().y()/CLHEP::mm;
      z = track->GetStep()->GetPostStepPoint()->GetPosition().z()/CLHEP::mm;
      material = track->GetStep()->GetPostStepPoint()->GetMaterial()->GetName();
    }
  px = track->GetMomentumDirection().x();
  py = track->GetMomentumDirection().y();
  pz = track->GetMomentumDirection().z();
  eventnumber = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  stepnumber = currentsteps;
  tracknumber = track->GetTrackID();
  isentryevent = isentry;
  if(PID!=13) //muons have no creator process and cause a seg fault
  creatorprocess = track->GetCreatorProcess()->GetProcessName();
  startx = track->GetVertexPosition().x()/CLHEP::mm;
  starty = track->GetVertexPosition().y()/CLHEP::mm;
  startz = track->GetVertexPosition().z()/CLHEP::mm;
  
  if(isadetector==1)
    {
      //below steps just used to parse detector number from detector name
      if(isentry==0)
	name = track->GetStep()->GetPreStepPoint()->GetPhysicalVolume()->GetName();
      if(isentry==1)
	name = track->GetStep()->GetPostStepPoint()->GetPhysicalVolume()->GetName();
      
      j=0;
      num ="";  
      for(std::string::size_type i = 0; i < name.size(); ++i)
	{

	  if(isdigit(name[i]))
	    {
	      num+=name[i];
	      j++;
	    }//if(isdigit...
           
	}//for(std...
 
      detectornumber = atoi(num);

    }//if(isadetector==1

  if(isadetector==0)
    {
      detectornumber = -1; //Used in implementation of LAr as its own detector
      
    }//if(isadetector==0


  fTree->Fill();
  
} //Io::Write


void IO::Close()
{

  fTree->Write();
  fFile->Close();
  G4cout << "Output file written and closed." << G4endl;
  
}

//////////////////////////////////////////////////

 class RunAction: public G4UserRunAction 
{
     
public:
     
  // Constructor and destructor
  RunAction(IO *io=0, char* inputseed="0") : G4UserRunAction(), fio(io), inputseed(inputseed){};
  ~RunAction(){};

  //Member functions
  IO* fio;
  double t1;
  char* inputseed;
  
  void BeginOfRunAction(const G4Run*)
  {
    G4cout << G4endl <<  "Beginning run, starting timer" << G4endl;
    t1=double(clock());
    fio->Open(inputseed);
  } //Initialize TTree, open TFile, get clock going

  void EndOfRunAction(const G4Run*)
  {
    fio->Close();
    t1=(double(clock())-t1);
    G4cout << G4endl << "Time:   " << t1/1000000 << G4endl << "steps saved: " << currentsteps << G4endl << G4endl;
  } //Write TTree, close TFile, stop clock

 };
       
//////////////////////////////////////////////////

class EventAction: public G4UserEventAction 
//Not currently implemented for anything, but this structure should work in case we need it later.
{
     
  //--------
public:
  //--------
     
  // Constructor and destructor
  EventAction(){};
  ~EventAction(){};

  //Member functions
  void UserEventAction();

};
       

void EventAction::UserEventAction(){}
//maybe changing event number should go here? May be extraneous

//////////////////////////////////////////////////

class SteppingAction: public G4UserSteppingAction 
{
     
public:

  // Constructor and destructor

  SteppingAction(IO *io=0) : G4UserSteppingAction(), fio(io){};
  ~SteppingAction(){};

  //Member functions
  IO* fio;
  void UserSteppingAction(const G4Step* step);

}; //class steppingaction
       

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  //cuts go here
  
  if(currentsteps<maxsteps)
    {//hits inside detector detection

      G4String whatgeometry = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
      G4String thatgeometry = "";
      const G4String detectornames = "phy_Det";
      const G4String fillgasname = "phy_FillGas"; //LAr surrounding detector

      
      if(strstr(whatgeometry.c_str(),detectornames.c_str())) //if pre step point originates in a detector
	{
	fio->Write(step->GetTrack(), 1, 0); //non-entry event
	currentsteps++;
	}//if(strstr
      
      if(strstr(whatgeometry.c_str(),fillgasname.c_str()))//if pre step point is in LAr
	{
	  thatgeometry = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
	  if(strstr(thatgeometry.c_str(),detectornames.c_str()))//but post step point is in Ge
	    {
	      fio->Write(step->GetTrack(), 1, 1); //entry event
	      currentsteps++;
	    }

	  // else //LAr as a channel, comment out these 5 lines to not write LAr hits 
	  //{
	  //fio->Write(step->GetTrack(),0, 0); //comment to not write LAr hits to a channel
	  //currentsteps++;
	  //}

	  
	} //else if(strstr


    } //if maxsteps

  if(currentsteps>=maxsteps)
    {
      G4cout << G4endl << "I NEED AN ADULT" << G4endl;
      return;
    }
	 
} //void SteppingAction
 

//////////////////////////////////////////////////
//////////////////////////////////////////////////


int main(int argc,char** argv)
{

  // Choose the Random engine
  //
  char* inputseed;
  inputseed= argv[2]; //This is not an optional argument. A null value for this crashes the program
  G4cout << G4endl << "Input seed: " <<  inputseed << G4endl;
  G4String s = inputseed;
  int ranengine = atoi(s);
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(ranengine);

  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  //
  // Detector construction
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Physics list
  runManager->SetUserInitialization(new Shielding);

  // Primary generator action
  PrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction();
  runManager->SetUserAction(gen_action);

  //Custom implemented classes
  IO *io = new IO;

  runManager->SetUserAction(new RunAction(io, inputseed));
  runManager->SetUserAction(new EventAction());  
  runManager->SetUserAction(new SteppingAction(io));

  
  // Initialize G4 kernel
  //
  runManager->Initialize();

#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
   G4UImanager* UImanager = G4UImanager::GetUIpointer(); 

  if (argc!=1) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];

    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode : define UI session
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    //    UImanager->ApplyCommand("/control/execute vis.mac");
#endif
    ui->SessionStart();
    delete ui;
#endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
