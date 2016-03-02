// ApplyField_module.cc
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

#ifndef ApplyField_Module
#define ApplyField_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "Simulation/SimChannel.h"
#include "Simulation/LArG4Parameters.h"
#include "RawData/RawDigit.h"
#include "RawData/raw.h"
#include "RecoBaseArt/WireCreator.h"
#include "RecoBaseArt/HitCreator.h"
#include "RecoBase/Hit.h"
#include "RecoBase/Cluster.h"
#include "RecoBase/Wire.h"
#include "RecoBase/Track.h"
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
// #include "uboone/Utilities/SignalShapingServiceMicroBooNE.h"
#include "CalibrationDBI/Interface/IDetPedestalService.h"
#include "CalibrationDBI/Interface/IDetPedestalProvider.h"
#include "CalibrationDBI/Interface/IChannelStatusService.h"
#include "CalibrationDBI/Interface/IChannelStatusProvider.h"

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
#include <array>
#include <string>
#include <cmath>
#include <typeinfo>
#include <utility>
#include <memory>
#include <iterator>

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace 
{

} // local namespace


namespace ApplyField 
{

  class ApplyField : public art::EDProducer
  {
  public:
    /// Default constructor
    explicit ApplyField(fhicl::ParameterSet const& parameterSet);

    
    virtual void beginJob() override;
    
    virtual void endJob() override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    virtual void produce (art::Event& event) override;
    
  private:

    art::InputTag fTrackModuleLabel;    ///< CalData module label
  }; // class ApplyField
  
  DEFINE_ART_MODULE(ApplyField)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  ApplyField::ApplyField(fhicl::ParameterSet const& pset)
  {
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
    
//     produces< std::vector<recob::Wire> >("blibla");
//     produces< std::vector<recob::Hit> >("UPlaneLaserHits");
//     produces< std::vector<recob::Hit> >("VPlaneLaserHits");
//     produces< std::vector<recob::Hit> >("YPlaneLaserHits");
  }

  
  //-----------------------------------------------------------------------
  void ApplyField::beginJob()
  {
    
  }
  
  
  
  void  ApplyField::endJob()
  {
    
  }

  //-----------------------------------------------------------------------
  void ApplyField::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameter set
    fTrackModuleLabel      = parameterSet.get< art::InputTag	     >("TrackModuleLabel");
  }

  //-----------------------------------------------------------------------
  void ApplyField::produce(art::Event& event) 
  {
    // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)   
    art::ValidHandle< std::vector<recob::Track> > TrackVecHandle = event.getValidHandle<std::vector<recob::Track>>(fTrackModuleLabel);
    
    for(const auto& Track : *TrackVecHandle)
    {
      std::cout << Track.NumberTrajectoryPoints() << std::endl;
    }
    
    // Prepairing the wire signal vector. It will be just the raw signal with subtracted pedestial
//     std::unique_ptr< std::vector<recob::Wire> > WireVec(new std::vector<recob::Wire>);
    
    // Fill Hits of all planes into the new
//     event.put(std::move(UHitVec), "UPlaneLaserHits");
//     event.put(std::move(VHitVec), "VPlaneLaserHits");
//     event.put(std::move(YHitVec), "YPlaneLaserHits");
  } // ApplyField::analyze()
  
} // namespace ApplyField

#endif // ApplyField_Module
