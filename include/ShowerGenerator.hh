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
/// \file ShowerGenerator.hh
/// \brief Definition of the ShowerGenerator class

#ifndef ShowerGenerator_h
#define ShowerGenerator_h 1

#include "G4VPrimaryGenerator.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>
#include <string>

class G4Event;

class ShowerGenerator : public G4VPrimaryGenerator
{
  public:
    ShowerGenerator();
    virtual ~ShowerGenerator();

    // static access method
    // static const ShowerGenerator* Instance();

    // method from the base class
    virtual void GeneratePrimaryVertex(G4Event*);

    double Distribution(double Energy, double CosTheta);

    inline double GetStartEnergy(){return start_energy; }
    inline double GetStartCosTheta(){return start_costheta; }


  private:
    TFile *inputfile;
    TTree *datatree;

   double        start_energy;    //muon start energy in surface GeV
   double        start_costheta;    //muon start angle

   std::vector<double>  *particle_energy;
   std::vector<double>  *particle_momentumX;  //GeV
   std::vector<double>  *particle_momentumY;  //GeV
   std::vector<double>  *particle_momentumZ;  //GeV
   std::vector<std::string>  *particle_name;

   int max_entries;

   G4ThreeVector particle_position;
   G4double particle_time;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


