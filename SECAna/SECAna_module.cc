// SECAna_module.cc
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

#ifndef SECAna_Module
#define SECAna_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
// #include "Simulation/SimChannel.h"
// #include "Simulation/LArG4Parameters.h"
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Cluster.h"
#include "lardata/RecoBase/Wire.h"
#include "lardata/RecoBase/Track.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"

#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCTruth.h"
#include "lardata/MCBase/MCTrack.h"

// Framework includes
#include "art/Utilities/Exception.h"
// #include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/EDAnalyzer.h"
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
#include "lardata/Utilities/AssociationUtil.h"
// #include "uboone/Utilities/SignalShapingServiceMicroBooNE.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"

// ROOT includes. Note: To look up the properties of the ROOT classes,
// use the ROOT web site; e.g.,
// <http://root.cern.ch/root/html532/ClassIndex.html>
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVectorT.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"

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
#include <fstream>

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace {
  
  // This is a anonymous namespace (the one below, instead, has a name:
  // "SECAna").
  // The stuff you declare in an anonymous namespace will not be visible
  // beyond this file (more technically, this "translation unit", that is
  // everything that gets included in the .cc file from now on).
  // In this way, you don't pollute the environment of other modules.
  
  // We will define this function at the end, but we at least declare it here
  // so that the module can freely use it.
  
} // local namespace


namespace SECAna {

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class definition

  // This is in-source documentation in Doxygen format.
  // A web page is automatically updated with documentation from the latest
  // LArSoft; that page is linked from LArSoft wiki page "Using LArSoft":
  // https://cdcvs.fnal.gov/redmine/projects/larsoft/wiki/Using_LArSoft#LArSoft-Doxygen
  // 
  // In general, comments that start with "///" or "/** */" blocks will land
  // into Doxygen documentation; code with "//!" will, too, but ROOT also
  // interprets that type of comment in some specific situations, so it's
  // better to stick to the other one to avoid confusion (and bugs).
  // 
  // Again in general, a comment starting with "///" will be sticking to the
  // next code line, while one starting with "///<" will be sticking to the
  // same code line, or the last one.
  /**
   * @brief Example analyser
   * 
   * This class extracts information from the generated and reconstructed
   * particles.
   * It produces histograms for the simulated particles in the input file:
   * - PDG ID (flavour) of all particles
   * - momentum of the primary particles selected to have a specific PDG ID
   * - length of the selected particle trajectory
   * 
   * If also produces two ROOT trees.
   * The first ROOT tree contains information on the simulated particles,
   * including in "dEdx" a binned histogram of collected charge as function
   * of track range.
   * 
   * The second ROOT tree contains information on the reconstructed hits.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *SimulationLabel* (string, default: "largeant"): label of the input data
   *   product with the detector simulation information (typically an instance
   *   of LArG4 module)
   * - *HitLabel* (string, mandatory): label of the input data product with
   *   reconstructed hits
   * - *ClusterLabel* (string, mandatory): label of the input data product with
   *   reconstructed clusters
   * - *PDGcode* (integer, mandatory): particle type (PDG ID) to be selected;
   *   only primary particles of this type will be considered
   * - *BinSize* (real, mandatory): dx [cm] used for the dE/dx calculation
   * 
   */
  class SECAna : public art::EDAnalyzer
  {
  public:
 
    // Standard constructor for an ART module; we don't need a special
    // destructor here.
    /// Default constructor
    explicit SECAna(fhicl::ParameterSet const& parameterSet);

    // the following methods have a standard meaning and a standard signature
    // inherited from the framework (art::EDAnalyzer class).
    // The "virtual" keyword is a reminder that the function we are dealing with
    // is, in fact, virtual. You don't need to understand it now, but it's very
    // important when you write a new algorithm.
    // The "override" keyword, new in C++ 2011, is an important safety measure
    // that ensures that the method we are going to write will override a
    // matching method in the base class. For example, if we mistyped it as
    // virtual void beginJob() const;
    // (adding "const"), the compiler will be very happy with it, but art will
    // not touch it, because art needs a "void beginJob()" (non-const) and
    // it will find one in the base class (void art::EDAnalyzer::beginJob())
    // and will happily and silently use that one instead. Using:
    // virtual void beginJob() const override;
    // instead, the compiler will immediately complain with us that this method
    // is overriding nothing, hinting to some mistake (the spurious "const"
    // in this case).
    
    // This method is called once, at the start of the job. In this
    //  example, it will define the histograms and n-tuples we'll write.
    // Define the histograms and n-tuples
    virtual void beginJob() override;
    
    virtual void endJob() override;

    // This method is called once, at the start of each run. It's a
    // good place to read databases or files that may have
    // run-dependent information.
    virtual void beginRun(const art::Run& run) override;

    // This method reads in any parameters from the .fcl files. This
    // method is called 'reconfigure' because it might be called in the
    // middle of a job; e.g., if the user changes parameter values in an
    // interactive event display.
    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void analyze(const art::Event& event) override;
    
//     virtual TGraph* FillGraph(const std::vector<float>& WireNumber, const std::vector<float>& PeakTime);
    

  private:

    // The stuff below is the part you'll most likely have to change to
    // go from this custom example to your own task.

    // The parameters we'll read from the .fcl file.
    //art::InputTag fCalDataModuleLabel;    ///< CalData module label
    std::string fFileName;
    std::vector<std::string> fTrackModuleLabel;    ///< Track module label
    std::vector<std::string> fTrackInstanceLabel;    ///< Track instance label
    std::vector<art::InputTag> fTrackTag; ///< Track tags
    art::InputTag fMCTrackTag;
    //std::array<float,3> fUVYThresholds;	  ///< U,V,Y-plane threshold in ADC counts for the laser hit finder
    ofstream OFile;
    
  }; // class SECAna
  
  DEFINE_ART_MODULE(SECAna)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  SECAna::SECAna(fhicl::ParameterSet const& pset) : EDAnalyzer(pset)
  {
    // get a pointer to the geometry service provider
    
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
  }

  
  //-----------------------------------------------------------------------
  void SECAna::beginJob()
  {
    OFile.open(fFileName);
    OFile.close();
    // Initialize the Art TFile service
  }
  
  void  SECAna::endJob()
  {
//     TFile* OFile = new TFile("HitHist.root", "RECREATE");
//     CollectionHits->Write();
    
//     TCanvas* C2 = new TCanvas("Hits","You",1400,1000);
//     CollectionHits->Draw();
//     C2 -> Print("Hits.png","png");
//     delete C2;
//     delete CollectionHits;
  }
   
  //-----------------------------------------------------------------------
  void SECAna::beginRun(const art::Run& /*run*/)
  {
    // art expects this function to have a art::Run argument;
    // C++ expects us to use all the arguments we are given,
    // or it will warn that we could have forgotten to use it
    // (otherwise, why would be ever passing it around?).
    // 
    // But we don't actually need nor use it.
    // 
    // The trick to convince C++ that we know what we are doing is to omit
    // (in this case, commenting out) the name of the parameter,
    // still leaving it around. The argument will be still passed around,
    // but we don't have any mean to use it, and the compiler will be satisfied
    // with that.
    
    // How to convert from number of electrons to GeV.  The ultimate
    // source of this conversion factor is
    // ${LARCORE_INC}/SimpleTypesAndConstants/PhysicalConstants.h.
    // But sim::LArG4Parameters might in principle ask a database for it.
//     art::ServiceHandle<sim::LArG4Parameters> larParameters;
//     fElectronsToGeV = 1./larParameters->GeVToElectrons();
  }

  //-----------------------------------------------------------------------
  void SECAna::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    
// Read parameter set
    fFileName = parameterSet.get<std::string>("FileName");
    fTrackModuleLabel = parameterSet.get< std::vector<std::string> >("TrackModuleLabel");
    fTrackInstanceLabel = parameterSet.get< std::vector<std::string> >("TrackInstanceLabel");
    fMCTrackTag = parameterSet.get< art::InputTag >("MCTrackTag"); 
    
    
    for(unsigned int label_index = 0; label_index < fTrackModuleLabel.size(); label_index++)
    {
      if(label_index < fTrackInstanceLabel.size())
      {
	fTrackTag.push_back(art::InputTag(fTrackModuleLabel.at(label_index),fTrackInstanceLabel.at(label_index)));
      }
      else
      {
	fTrackTag.push_back(art::InputTag(fTrackModuleLabel.at(label_index)));
      }
    }
  }

  //-----------------------------------------------------------------------
  void SECAna::analyze(const art::Event& event) 
  {
    OFile.open(fFileName, std::ios_base::app);
    
    std::vector<art::ValidHandle< std::vector<recob::Track> >> TrackVecHandles;// = event.getValidHandle< std::vector<sim::MCTrack> >(fMCTrackTag);
   
    art::ValidHandle< std::vector<sim::MCTrack> > MCTrackVecHandles = event.getValidHandle< std::vector<sim::MCTrack> >(fMCTrackTag);
    float MCLength;
    
    for(const auto& MCTrack : *MCTrackVecHandles)
    {
      MCLength = pow(MCTrack.Start().X()-MCTrack.End().X(),2) + pow(MCTrack.Start().Y()-MCTrack.End().Y(),2)+pow(MCTrack.Start().Z()-MCTrack.End().Z(),2);
//       OFile << event.id() << " " << MCTrack.TrackID() << " "  << sqrt(length) << "\n" ;
    }
    
   for(const auto& InputTag : fTrackTag)
   {
     TrackVecHandles.push_back(event.getValidHandle< std::vector<recob::Track> >(InputTag));
   }
   
//    art::ValidHandle< std::vector<sim::MCTrack> > MCTrackVecHandles = event.getValidHandle< std::vector<recob::Track> >(fMCTrackTag);
   
   for(const auto& TrackVecHandle: TrackVecHandles)
   {
     for(const auto& Track : *TrackVecHandle)
     {
       float LengthDiff = 1 - fabs((Track.Length() - MCLength)/MCLength);
       if(Track.Length()>50) OFile << event.id() << " " << Track.ID() << " "  << LengthDiff << "\n" ;
     }
   }
   
   OFile.close();
   
  } // SECAna::analyze()
  
//   TGraph* SECAna::FillGraph(const std::vector<float>& WireNumber, const std::vector<float>& PeakTime)
//   {
//     TVectorT<float> TWireNumber(WireNumber.size(),WireNumber.data());
//     TVectorT<float> TTimeBin(PeakTime.size(),PeakTime.data());
    
//     return new TGraph(TWireNumber,TTimeBin);
//   }

} // namespace SECAna

#endif // SECAna_Module
