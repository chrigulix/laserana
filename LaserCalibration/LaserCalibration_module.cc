// LaserCalibration_module.cc
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

#ifndef LaserCalibration_Module
#define LaserCalibration_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "Simulation/SimChannel.h"
#include "Simulation/LArG4Parameters.h"
#include "RawData/RawDigit.h"
#include "RawData/raw.h"
#include "RecoBaseArt/WireCreator.h"
#include "RecoBase/Hit.h"
#include "RecoBase/Cluster.h"
#include "RecoBase/Wire.h"
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

// C++ Includes
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <typeinfo>
#include <utility>
#include <memory>


namespace {
  
  // This is a anonymous namespace (the one below, instead, has a name:
  // "LaserCalibration").
  // The stuff you declare in an anonymous namespace will not be visible
  // beyond this file (more technically, this "translation unit", that is
  // everything that gets included in the .cc file from now on).
  // In this way, you don't pollute the environment of other modules.
  
  // We will define this function at the end, but we at least declare it here
  // so that the module can freely use it.
  /// Utility function to get the diagonal of the detector
  float DetectorDiagonal(geo::GeometryCore const& geom);
  
} // local namespace


namespace LaserCalibration {

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
  class LaserCalibration : public art::EDProducer
  {
  public:
 
    // Standard constructor for an ART module; we don't need a special
    // destructor here.
    /// Default constructor
    explicit LaserCalibration(fhicl::ParameterSet const& parameterSet);

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
    virtual void beginJob() /*override*/;
    
    virtual void endJob() /*override*/;

    // This method is called once, at the start of each run. It's a
    // good place to read databases or files that may have
    // run-dependent information.
//     virtual void beginRun(const art::Run& run) override;

    // This method reads in any parameters from the .fcl files. This
    // method is called 'reconfigure' because it might be called in the
    // middle of a job; e.g., if the user changes parameter values in an
    // interactive event display.
    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) /*override*/;

    // The analysis routine, called once per event. 
    virtual void produce (art::Event& event) /*override*/;

  private:

    // The stuff below is the part you'll most likely have to change to
    // go from this custom example to your own task.

    // The parameters we'll read from the .fcl file.
    std::string fSimulationProducerLabel; ///< The name of the producer that tracked simulated particles through the detector
    std::string fHitProducerLabel;        ///< The name of the producer that created hits
    std::string fClusterProducerLabel;    ///< The name of the producer that created clusters
    int fSelectedPDG;                     ///< PDG code of particle we'll focus on
    double fBinSize;                      ///< For dE/dx work: the value of dx. 

    // Pointers to the histograms we'll create. 
    TH1D* fPDGCodeHist;     ///< PDG code of all particles
    TH1D* fMomentumHist;    ///< momentum [GeV] of all selected particles
    TH1D* fTrackLengthHist; ///< true length [cm] of all selected particles

    // The n-tuples we'll create.
    TTree* fSimulationNtuple;     ///< tuple with simulated data
    TTree* fReconstructionNtuple; ///< tuple with reconstructed data

    // The variables that will go into the n-tuple.
    int fEvent;     ///< number of the event being processed
    int fRun;       ///< number of the run being processed
    int fSubRun;    ///< number of the sub-run being processed
    
    /// @name Variables used in the simulation tree
    /// @{
    int fSimPDG;       ///< PDG ID of the particle begin processed
    int fSimTrackID;   ///< GEANT ID of the particle begin processed
    
    // Arrays for 4-vectors: (x,y,z,t) and (Px,Py,Pz,E).
    // Note: old-style C++ arrays are considered obsolete. However,
    // to create simple n-tuples, we still need to use them. 
    double fStartXYZT[4]; ///< (x,y,z,t) of the true start of the particle
    double fEndXYZT[4];   ///< (x,y,z,t) of the true end of the particle
    double fStartPE[4];   ///< (Px,Py,Pz,E) at the true start of the particle
    double fEndPE[4];     ///< (Px,Py,Pz,E) at the true end of the particle
    
    /// Number of dE/dx bins in a given track.
    int fSimNdEdxBins;
    
    /// The vector that will be used to accumulate dE/dx values as function of range.
    std::vector<double> fSimdEdxBins;
    /// @}
    
    /// @name Variables used in the simulation tree
    /// @{
    int fRecoPDG;       ///< PDG ID of the particle begin processed
    int fRecoTrackID;   ///< GEANT ID of the particle begin processed
    
    /// Number of dE/dx bins in a given track.
    int fRecoNdEdxBins;
    
    /// The vector that will be used to accumulate dE/dx values as function of range.
    std::vector<double> fRecodEdxBins;
    
    /// @}

    // Other variables that will be shared between different methods.
    geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
    double                   fElectronsToGeV; ///< conversion factor
    
    art::InputTag fCalDataModuleLabel;
    
  }; // class LaserCalibration
  
  DEFINE_ART_MODULE(LaserCalibration)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  LaserCalibration::LaserCalibration(fhicl::ParameterSet const& pset)
//     : EDProducer()
  {
    // get a pointer to the geometry service provider
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
    
    produces< std::vector<recob::Wire> >("blibla");
  }

  
  //-----------------------------------------------------------------------
  void LaserCalibration::beginJob()
  {
    // Get the detector length, to determine the maximum bin edge of one
    // of the histograms.
    const double detectorLength = DetectorDiagonal(*fGeometry);

    // Access ART's TFileService, which will handle creating and writing
    // histograms and n-tuples for us. 
    art::ServiceHandle<art::TFileService> tfs;
  
    // The arguments to 'make<whatever>' are the same as those passed
    // to the 'whatever' constructor, provided 'whatever' is a ROOT
    // class that TFileService recognizes. 

    // Define the histograms. Putting semi-colons around the title
    // causes it to be displayed as the x-axis label if the histogram
    // is drawn (the format is "title;label on abscissae;label on ordinates").
    fPDGCodeHist     = tfs->make<TH1D>("pdgcodes",";PDG Code;",                  5000, -2500, 2500);
    fMomentumHist    = tfs->make<TH1D>("mom",     ";particle Momentum (GeV);",    100, 0.,    10.);
    fTrackLengthHist = tfs->make<TH1D>("length",  ";particle track length (cm);", 200, 0, detectorLength);

    // Define our n-tuples, which are limited forms of ROOT
    // TTrees. Start with the TTree itself.
    fSimulationNtuple     = tfs->make<TTree>("LaserCalibrationSimulation",    "LaserCalibrationSimulation");
    fReconstructionNtuple = tfs->make<TTree>("LaserCalibrationReconstruction","LaserCalibrationReconstruction");

    // Define the branches (columns) of our simulation n-tuple. When
    // we write a variable, we give the address of the variable to
    // TTree::Branch.
    fSimulationNtuple->Branch("Event",       &fEvent,          "Event/I");
    fSimulationNtuple->Branch("SubRun",      &fSubRun,         "SubRun/I");
    fSimulationNtuple->Branch("Run",         &fRun,            "Run/I");
    fSimulationNtuple->Branch("TrackID",     &fSimTrackID,     "TrackID/I");
    fSimulationNtuple->Branch("PDG",         &fSimPDG,         "PDG/I");
    // When we write arrays, we give the address of the array to
    // TTree::Branch; in C++ this is simply the array name.
    fSimulationNtuple->Branch("StartXYZT",   fStartXYZT,       "StartXYZT[4]/D");
    fSimulationNtuple->Branch("EndXYZT",     fEndXYZT,         "EndXYZT[4]/D");
    fSimulationNtuple->Branch("StartPE",     fStartPE,         "StartPE[4]/D");
    fSimulationNtuple->Branch("EndPE",       fEndPE,           "EndPE[4]/D");
    // For a variable-length array: include the number of bins.
    fSimulationNtuple->Branch("NdEdx",       &fSimNdEdxBins,   "NdEdx/I");
    // ROOT can understand fairly well vectors of numbers (and little more)
    fSimulationNtuple->Branch("dEdx",        &fSimdEdxBins);

    // A similar definition for the reconstruction n-tuple. Note that we
    // use some of the same variables in both n-tuples.
    fReconstructionNtuple->Branch("Event",   &fEvent,          "Event/I");
    fReconstructionNtuple->Branch("SubRun",  &fSubRun,         "SubRun/I");
    fReconstructionNtuple->Branch("Run",     &fRun,            "Run/I");
    fReconstructionNtuple->Branch("TrackID", &fRecoTrackID,    "TrackID/I");
    fReconstructionNtuple->Branch("PDG",     &fRecoPDG,        "PDG/I");
    fReconstructionNtuple->Branch("NdEdx",   &fRecoNdEdxBins,  "NdEdx/I");
    fReconstructionNtuple->Branch("dEdx",    &fRecodEdxBins);
  }
  
  void  LaserCalibration::endJob()
  {  
  }
   
  //-----------------------------------------------------------------------
//   void LaserCalibration::beginRun(const art::Run& /*run*/)
//   {
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
//   }

  //-----------------------------------------------------------------------
  void LaserCalibration::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameters from the .fcl file. The names in the arguments
    // to p.get<TYPE> must match names in the .fcl file.
    fSimulationProducerLabel = parameterSet.get< std::string >("SimulationLabel");
    fHitProducerLabel        = parameterSet.get< std::string >("HitLabel");
    fClusterProducerLabel    = parameterSet.get< std::string >("ClusterLabel");
    fSelectedPDG             = parameterSet.get< int         >("PDGcode");
    fBinSize                 = parameterSet.get< double      >("BinSize");
    fCalDataModuleLabel      = parameterSet.get<art::InputTag>("CalDataModuleLabel");
  }

  //-----------------------------------------------------------------------
  void LaserCalibration::produce(art::Event& event) 
  {
    // Start by fetching some basic event information for our n-tuple.
    fEvent  = event.id().event(); 
    fRun    = event.run();
    fSubRun = event.subRun();
    
    // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)   
    art::ValidHandle< std::vector<raw::RawDigit> > digitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>(fCalDataModuleLabel);
    
    // Prepairing the wire signal vector. It will be just the raw signal with subtracted pedestial
    std::unique_ptr< std::vector<recob::Wire> > WireVec(new std::vector<recob::Wire>); 
//     std::vector<recob::Wire> WireVec;
    
    // Preparing WireID vector
    std::vector<geo::WireID> WireIDs;
    
    
    WireVec->reserve(digitVecHandle->size());
    
    unsigned RawIndex = 1477;
    
//     raw::Uncompress a();
    
    const lariov::IDetPedestalProvider& PedestalRetrievalAlg = art::ServiceHandle<lariov::IDetPedestalService>()->GetPedestalProvider();
    TH1S* SingleWire = new TH1S("You","Fuck",digitVecHandle->at(RawIndex).Samples(),0,digitVecHandle->at(RawIndex).Samples()-1);
    
    std::vector<short> RawADC;
    
//     digitVecHandle
    
    for(auto const & RawDigit : *digitVecHandle)
    {
      raw::ChannelID_t channel = RawDigit.Channel();
    
      WireIDs = fGeometry->ChannelToWire(channel);
      unsigned int thePlane = WireIDs.front().Plane;
      unsigned int theWire = WireIDs.front().Wire;
      
      RawADC.resize(RawDigit.Samples());
      
      raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());
      
      for(auto & RawSample : RawADC)
      {
	RawSample -= PedestalRetrievalAlg.PedMean(channel);
      }
      
      recob::Wire::RegionsOfInterest_t ROIVec;
      
      ROIVec.add_range(0,std::move(RawADC));
      
//       recob::WireCreator
      
      WireVec->emplace_back(recob::WireCreator(std::move(ROIVec),RawDigit).move());
      
//       std::cout << "Plane: " << thePlane << std::endl;
//       std::cout << "Wire: " << theWire << std::endl;
//       std::cout << "Compresson: " << RawDigit.Compression() << std::endl; 
      
//       float Pedestal = pedestalRetrievalAlg.PedMean(channel);
    
//       std::cout << "Pedestal: " << Pedestal << std::endl;
      
//       for(unsigned ADC_ticks = 0; ADC_ticks < RawDigit.Samples(); ADC_ticks++)
//       {
// 	RawDigit.ADC(ADC_ticks) -= Pedestal;
//       }
      
//       for(auto & RawDigitADC : RawDigit.ADC())
//       {
// 	RawDigitADC -= Pedestal;
//       }
    }
    
    
    float Pedestal = PedestalRetrievalAlg.PedMean(digitVecHandle->at(RawIndex).Channel());
    for(unsigned samples = 0; samples < digitVecHandle->at(RawIndex).Samples(); samples++)
      SingleWire->SetBinContent(samples,digitVecHandle->at(RawIndex).ADC(samples) - Pedestal);
    
    TCanvas* C1 = new TCanvas("Fuck","You",1400,1000);
    SingleWire->Draw();
    C1 -> Print("Fuck.png","png");
    delete SingleWire;
    delete C1;
    
    event.put(std::move(WireVec), "blibla");
    
//     auto const& theWire = Wires[5];
    
//     raw::ChannelID_t theChannel = theWire.Channel();
//     std::vector<geo::WireID> wids = fGeometry->ChannelToWire(theChannel);
//     unsigned short thePlane = wids[0].Plane;
//     unsigned short theWireNum = wids[0].Wire;
    
//     std::cout << "FUUUUUUUUUUUUUUUUUUUUUUUUCK " << theWireNum << std::endl;


  } // LaserCalibration::analyze()
  
  
  // This macro has to be defined for this module to be invoked from a
  // .fcl file; see LaserCalibration.fcl for more information.

} // namespace LaserCalibration


namespace {
  // time to define that function...
  float DetectorDiagonal(geo::GeometryCore const& geom) {
    const double length = geom.DetLength();
    const double width = 2. * geom.DetHalfWidth();
    const double height = 2. * geom.DetHalfHeight();
    
    return std::sqrt(length*length + width*width + height*height);
  } // DetectorDiagonal()
} // local namespace


#endif // LaserCalibration_Module
