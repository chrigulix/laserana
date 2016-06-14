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
#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCTruth.h"
#include "SimpleTypesAndConstants/geo_types.h"

// Framework includes
#include "art/Utilities/Exception.h"
// #include "art/Framework/Core/EDAnalyzer.h"
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


namespace {
  
  
} // local namespace


namespace LaserDataMerger {

  class LaserDataMerger : public art::EDProducer
  {
  public:
 
    explicit LaserDataMerger(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() /*override*/;
    
    virtual void endJob() /*override*/;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) /*override*/;

    // The analysis routine, called once per event. 
    virtual void produce (art::Event& event) /*override*/;
    
  private:
    
    // ficl parameters
    art::InputTag fCalDataModuleLabel;    ///< Label which tells us what to load
    bool fWireMapGenerator;               ///< Decide if you want to produce a wire map (recomended if you don't have it)
    unsigned short fLCSNumber;            ///< Laser Calibration System identifier ()
    /// @}

    // Other variables that will be shared between different methods.
    geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
     
    std::string fFileName = "WireIndexMap.root";
    
    std::map<unsigned int, unsigned int> UMap;
    std::map<unsigned int, unsigned int> VMap;
    std::map<unsigned int, unsigned int> YMap;
    
    
  }; // class LaserDataMerger
  
  DEFINE_ART_MODULE(LaserDataMerger)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  LaserDataMerger::LaserDataMerger(fhicl::ParameterSet const& pset)
//     : EDProducer()
  {
    // get a pointer to the geometry service provider
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
    
    produces< std::vector<recob::Wire> >("blibla");
  }

  
  //-----------------------------------------------------------------------
  void LaserDataMerger::beginJob()
  {
    
    if(!fWireMapGenerator)
    {
      TFile* InputFile = new TFile(fFileName.c_str(), "READ");
      
      std::map<unsigned int, unsigned int>* pUMap;
      std::map<unsigned int, unsigned int>* pVMap;
      std::map<unsigned int, unsigned int>* pYMap;
      
      InputFile->GetObject("UMap",pUMap);
      InputFile->GetObject("VMap",pVMap);
      InputFile->GetObject("YMap",pYMap);
      
      UMap = *pUMap;
      VMap = *pVMap;
      YMap = *pYMap;
      
      delete pUMap;
      delete pVMap;
      delete pYMap;
    }
    
    // TODO: Change later
    fLCSNumber = 2; 
  }
  
  
  
  void  LaserDataMerger::endJob()
  {
    if(fWireMapGenerator)
    {
      TFile* OutputFile = new TFile(fFileName.c_str(), "RECREATE");
    
      OutputFile->WriteObject(&UMap,"UMap");
      OutputFile->WriteObject(&VMap,"VMap");
      OutputFile->WriteObject(&YMap,"YMap");
    }
  }
   
 
  void LaserDataMerger::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameters from the .fcl file. The names in the arguments
    // to p.get<TYPE> must match names in the .fcl file.
    
    fWireMapGenerator        = parameterSet.get< bool        >("GenerateWireMap");
    fCalDataModuleLabel      = parameterSet.get<art::InputTag>("CalDataModuleLabel");
  }

  //-----------------------------------------------------------------------
  void LaserDataMerger::produce(art::Event& event) 
  {

       // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)   
    art::ValidHandle< std::vector<raw::RawDigit> > DigitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>(fCalDataModuleLabel);
    
        TH1F* SingleWire = new TH1F("You","Fuck",DigitVecHandle->at(UMap.at(UWireNumber)).Samples(),0,DigitVecHandle->at(UMap.at(UWireNumber)).Samples()-1);
        
    unsigned int Index = 0;
    // Loop over all physical readout channels
    for(auto const & RawDigit : *DigitVecHandle)
    {
      raw::ChannelID_t channel = RawDigit.Channel();
      
      // Create a Wire object with the raw signal
      
      if(fGeometry->ChannelToWire(channel).front().Plane == 2) // If wire plane is Y-plane
      {
	if(fWireMapGenerator) YMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
      }
      else if(fGeometry->ChannelToWire(channel).front().Plane == 1) // If wire plane is V-plane
      {
	if(fWireMapGenerator) VMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
      }
      else if(fGeometry->ChannelToWire(channel).front().Plane == 0) // If wire plane is U-plane 
      {
	if(fWireMapGenerator) UMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
      }
      Index++;
    }
        
  } // LaserDataMerger::analyze()
  
  //------------------------------------------------------------------------  
  // This macro has to be defined for this module to be invoked from a
  // .fcl file; see LaserDataMerger.fcl for more information.

} // namespace LaserDataMerger

#endif // LaserDataMerger_Module
