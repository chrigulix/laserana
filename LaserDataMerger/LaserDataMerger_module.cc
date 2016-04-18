// LaserDataMerger_module.cc
// A basic "skeleton" to read in art::Event records from a file,
// access their information, and do something with them. 

// See
// <https://cdcvs.fnal.gov/redmine/projects/larsoftsvn/wiki/Using_the_Framework>
// for a description of the ART classes used here.

// Almost everything you see in the code below may have to be changed
// by you to suit your task. The example task is to make histograms
// and n-tuples related to dE/dx of particle tracks in the detector.

// As you try to understand why things are done a certain way in this
// example ("What's all this stuff about 'auto const&'?"), it will help
// to read ADDITIONAL_NOTES.txt in the same directory as this file.

// Also note that, despite our efforts, the documentation and the practices in
// this code may fall out of date. In doubt, ask!
// The last revision of this code was done on July 2015 with LArSoft v04_17_00.

#ifndef LaserDataMerger_Module
#define LaserDataMerger_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "Simulation/SimChannel.h"
#include "Simulation/LArG4Parameters.h"
#include "RawData/RawDigit.h"
#include "RawData/raw.h"
#include "Geometry/Geometry.h"
#include "Geometry/GeometryCore.h"

#include "SimpleTypesAndConstants/geo_types.h"

// Framework includes
#include "art/Utilities/Exception.h"
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Persistency/Common/Ptr.h"

// uBooNE includes
#include "Utilities/AssociationUtil.h"

// ROOT includes. Note: To look up the properties of the ROOT classes,
// use the ROOT web site; e.g.,
// <http://root.cern.ch/root/html532/ClassIndex.html>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TFile.h"

// C++ Includes
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <typeinfo>
#include <utility>
#include <memory>
#include <iterator>

// Laser Module Classes
#include "LaserObjects/LaserBeam.h"


namespace LaserDataMerger {

  class LaserDataMerger : public art::EDProducer
  {
  public:
 
    explicit LaserDataMerger(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob();
        
    virtual void beginRun(art::Run& run);
    
    virtual void endJob();

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet);
    
    // The analysis routine, called once per event. 
    virtual void produce (art::Event& event);
    
  private:
    
    // All this goes into the root tree
    TTree* fTimeAnalysis;
    unsigned int fEvent;
    unsigned int time_s;
    unsigned int time_ms;
    
    bool fDebug = false;
    
    std::map<unsigned int, unsigned int > timemap; ///< Key value: idex of event, corresponding index in laser data file
    
    
    bool fReadTimeMap = false;
    bool fGenerateTimeInfo = false;
    std::string fTimemapFile;             ///< File containing information about timing
    
    unsigned int RunNumber = 0;
    
    unsigned short fLCSNumber;            ///< Laser Calibration System identifier ()
 
  }; // class LaserDataMerger
  

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  LaserDataMerger::LaserDataMerger(fhicl::ParameterSet const& pset)
  {

    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
    
    //produces< std::vector<recob::Wire> >("blibla");
  }

  
  //-----------------------------------------------------------------------
  void LaserDataMerger::beginJob()
  {
    
  }
  
  void LaserDataMerger::beginRun(art::Run& run) {
    
   art::ServiceHandle<art::TFileService> tfs;
    if (fGenerateTimeInfo) {
      std::cout << "GENERATING TIME OUTPUT" << std::endl;
      // Initialize time info root file
      fTimeAnalysis = tfs->make<TTree>("TimeAnalysis", "TimeAnalysis");
      fTimeAnalysis->Branch("event",     &fEvent);
      fTimeAnalysis->Branch("time_s",    &time_s);
      fTimeAnalysis->Branch("time_ms",   &time_ms);
      
     }
    else if (fReadTimeMap) {
      
           
      // TODO: Build the filename from the run number provided by the beginRun()
      // function. For now you have to provide the filename in the fhcl file.
      RunNumber = run.run();
      std::string TimemapFile = "TimeMap-" + std::to_string(RunNumber) + ".root";
      
      std::cout << "READING TIMEMAP FILE: " << TimemapFile << std::endl; 
      TFile* InputFile = new TFile(TimemapFile.c_str(), "READ");
      TTree *tree = (TTree*)InputFile->Get("tree");      
      
      unsigned int map_root;
      tree->SetBranchAddress("map", &map_root);
      Long64_t nentries = tree->GetEntries ();
      
      for (Long64_t idx = 0; idx < nentries; idx++) {
        tree->GetEntry(idx);
        
        timemap.insert( std::pair<unsigned int, unsigned int>(idx, map_root));
        
        std::cout << "entry: " << timemap.at(idx) << std::endl; 
        
       }
     }
    return;
  }

  
  void  LaserDataMerger::endJob()
  {   
  }
   
  void LaserDataMerger::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameters from the .fcl file. The names in the arguments
    // to p.get<TYPE> must match names in the .fcl file.
    fReadTimeMap        = parameterSet.get< bool >("ReadTimeMap");
    fGenerateTimeInfo   = parameterSet.get< bool >("GenerateTimeInfo");
    fTimemapFile        = parameterSet.get< std::string >("TimemapFile");
    //fLaserSystemFile        = parameterSet.get< bool        >("LaserSystemFile");
  }

  //-----------------------------------------------------------------------
  void LaserDataMerger::produce(art::Event& event) 
  {
   
   if (fGenerateTimeInfo) {
     // 
    fEvent = (unsigned int) event.id().event();
    time_s = (unsigned int) event.time().timeHigh();
    time_ms = (unsigned int) event.time().timeLow();
    
    if (fDebug) {
      std::cout << "Event ID: " << fEvent << std::endl;
      std::cout << "Event Time (low): " << time_s  << std::endl;
      std::cout << "Event Time (hig): " << time_ms << std::endl;
     }
    fTimeAnalysis->Fill();
    
    }
  } // LaserDataMerger::analyze()

DEFINE_ART_MODULE(LaserDataMerger)
  
} // namespace LaserDataMerger

#endif // LaserDataMerger_Module
