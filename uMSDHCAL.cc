#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "HistoManager.hh"
#include "PhysicsList.hh"


#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "G4VisExecutive.hh"
#include "G4VisManager.hh"


//#ifdef G4UI_USE
#include "G4UIExecutive.hh"
//#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {

  G4long seed = (long) time(NULL);
  seed = G4UIcommand::ConvertToInt(argv[1]);
  G4cout << "Using user random seed = " <<  seed  << G4endl;

  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
  //G4long seed = (long) time(NULL);
  CLHEP::HepRandom::setTheSeed(seed);
  G4cout << "Seed: " << CLHEP::HepRandom::getTheSeed() << G4endl;  

  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);

  /******RUN MANAGER************/
  G4RunManager * runManager = new G4RunManager;


  /*******DETECTOR******/
  DetectorConstruction* detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);

  G4cout << "Nlayer = " <<numOfLayer << G4endl;


  detector->PrintCalorParameters();

  /*******PHYSIC LIST******/
  G4PhysListFactory factory;
  G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("QGSP_BERT_EMZ");
  
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);
  

  TString rootfilename = "uMSDHCAL.root";
  if (argc == 5)
    {
      rootfilename = argv[4];
    }
  if (argc == 6)
    {
      rootfilename = argv[5];
    }


  /*******HISTOGRAMS**********/
  HistoManager* histo = new HistoManager(rootfilename);

 
  /*********PRIMARY GENERATOR**************/
  PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector,histo);
  runManager->SetUserAction(primary);


  /********ACTIONS*************/
  RunAction*      runAct = new RunAction(detector,primary,histo);
  EventAction*    evtAct = new EventAction(detector,runAct,histo);
  TrackingAction* trkAct = new TrackingAction(detector,runAct,evtAct,histo);
  SteppingAction* stpAct = new SteppingAction(detector,runAct,evtAct,histo);

  runManager->SetUserAction(runAct);
  runManager->SetUserAction(evtAct);
  runManager->SetUserAction(trkAct);
  runManager->SetUserAction(stpAct);


  /********EVENT DISPLAY********/
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  G4UImanager* UI = G4UImanager::GetUIpointer();  


  G4cout<<""<<G4endl;
  G4cout<<"INITIALISATION"<<G4endl;
  UI->ApplyCommand("/run/initialize");  




  if (argc == 1)
    {
      G4cout<<""<<G4endl;
      G4cout<<"PARTICLES"<<G4endl;
  
      UI->ApplyCommand("/gun/particle pi-");
      UI->ApplyCommand("/gun/energy 10 GeV");
      UI->ApplyCommand("/run/beamOn 1000");
    }

  if (argc == 2)
    {
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
      ui->SessionStart();
      delete ui;
    }

  if (argc == 5)
    {
      TString energy = argv[1];
      TString bfield = argv[2];
      TString nevent = argv[3];

      TString command0 = "/random/resetEngineFrom currentRun.rndm";
      TString command1 = "/gun/energy " + energy + " GeV";
      TString command2 = "/Bfield/setField " + bfield;
      TString command3 = "/run/beamOn " + nevent;

      G4cout<<"*************************"<<G4endl;
      G4cout<<"  ENERGY  "<<energy<<G4endl;
      G4cout<<"  BFIELD  "<<bfield<<G4endl;
      G4cout<<"  NEVENT  "<<nevent<<G4endl;
      G4cout<<"  ROOTFILE "<<rootfilename<<G4endl;
      G4cout<<"*************************"<<G4endl;

      UI->ApplyCommand("/gun/particle mu-");
      UI->ApplyCommand(command0);
      UI->ApplyCommand(command1);
      UI->ApplyCommand(command2);
      UI->ApplyCommand(command3);
    }
  if (argc == 6)
    {
      TString energy = argv[2];
      TString bfield = '0';
      TString nevent = argv[3];
      TString beamparticle = argv[4];

      TString command_gun = "/gun/particle " + beamparticle + "-";
      TString command0 = "/random/resetEngineFrom currentRun.rndm";
      TString command1 = "/gun/energy " + energy + " GeV";
      TString command2 = "/Bfield/setField " + bfield;
      TString command3 = "/run/beamOn " + nevent;
     
      G4cout<<"*************************"<<G4endl;
      G4cout<<"  ENERGY  "<<energy<<G4endl;
      G4cout<<"  BFIELD  "<<bfield<<G4endl;
      G4cout<<"  NEVENT  "<<nevent<<G4endl;
      G4cout<<"  ROOTFILE "<<rootfilename<<G4endl;
      G4cout<<"*************************"<<G4endl;

      UI->ApplyCommand(command_gun);
      UI->ApplyCommand(command0);
      UI->ApplyCommand(command1);
      UI->ApplyCommand(command2);
      UI->ApplyCommand(command3);

    }



  // job termination
  G4cout<<""<<G4endl;
  G4cout<<"JOB TERMINATION"<<G4endl;
  delete histo;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

