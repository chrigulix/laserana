// LaserReco_module.cc
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

#ifndef LaserReco_Module
#define LaserReco_Module

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


namespace {
  
  // This is a anonymous namespace (the one below, instead, has a name:
  // "LaserReco").
  // The stuff you declare in an anonymous namespace will not be visible
  // beyond this file (more technically, this "translation unit", that is
  // everything that gets included in the .cc file from now on).
  // In this way, you don't pollute the environment of other modules.
  
  // We will define this function at the end, but we at least declare it here
  // so that the module can freely use it.
  /// Utility function to get the diagonal of the detector
  float DetectorDiagonal(geo::GeometryCore const& geom);
  
} // local namespace


namespace LaserReco {

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
  class LaserReco : public art::EDProducer
  {
  public:
 
    // Standard constructor for an ART module; we don't need a special
    // destructor here.
    /// Default constructor
    explicit LaserReco(fhicl::ParameterSet const& parameterSet);

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
//     virtual void beginRun(const art::Run& run) override;

    // This method reads in any parameters from the .fcl files. This
    // method is called 'reconfigure' because it might be called in the
    // middle of a job; e.g., if the user changes parameter values in an
    // interactive event display.
    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void produce (art::Event& event) override;
    
    std::vector<recob::Hit> LaserHitFinder(recob::Wire const& SingleWire, raw::ChannelID_t const& Channel);
    
    std::vector<recob::Hit> UPlaneHitFinder(recob::Wire const& SingleWire, raw::ChannelID_t const& Channel);
    
    std::vector<recob::Hit> VPlaneHitFinder(recob::Wire const& SingleWire, raw::ChannelID_t const& Channel);

  private:

    // The stuff below is the part you'll most likely have to change to
    // go from this custom example to your own task.

    // The parameters we'll read from the .fcl file.
    bool fWireMapGenerator;               ///< Decide if you want to produce a wire map (recomended if you don't have it)
    float fUPlaneThreshold;		  ///< U-plane threshold in ADC counts for the laser hit finder
    float fVPlaneThreshold;		  ///< V-plane threshold in ADC counts for the laser hit finder
    float fYPlaneThreshold;		  ///< Y-plane threshold in ADC counts for the laser hit finder
    art::InputTag fCalDataModuleLabel;    ///< CalData module label
    //std::array<float,3> fUVYThresholds;	  ///< U,V,Y-plane threshold in ADC counts for the laser hit finder
    
    // Other variables that will be shared between different methods.
    geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
    
    std::string fFileName = "WireIndexMap.root";
    
    std::map<unsigned int, unsigned int> UMap;
    std::map<unsigned int, unsigned int> VMap;
    std::map<unsigned int, unsigned int> YMap;
    
    unsigned short fLCSNumber;
    
    // Threshold array
    std::array<float,3> fUVYThresholds;
    
    // Collection Hits Analysis
    TH2D* CollectionHits;
    
  }; // class LaserReco
  
  DEFINE_ART_MODULE(LaserReco)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  LaserReco::LaserReco(fhicl::ParameterSet const& pset)
//     : EDProducer()
  {
    // get a pointer to the geometry service provider
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
    
//     produces< std::vector<recob::Wire> >("blibla");
    produces< std::vector<recob::Hit> >("LaserHits");
  }

  
  //-----------------------------------------------------------------------
  void LaserReco::beginJob()
  {
    // Get the detector length, to determine the maximum bin edge of one
    // of the histograms.
    const double detectorLength = DetectorDiagonal(*fGeometry);
    
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
    
    // TODO: Temporary
    CollectionHits = new TH2D("Width vs. Amplitude","Width vs. Amplitude",1000,0,1000,3500,0,3500);
    
    // TODO: Change later
    fLCSNumber = 2;
    
    // Fill threshold
    fUVYThresholds = {fUPlaneThreshold, fVPlaneThreshold, fYPlaneThreshold};

    // Access ART's TFileService, which will handle creating and writing
    // histograms and n-tuples for us. 
//     art::ServiceHandle<art::TFileService> tfs;
    
    // Above is almost only useless shit!
    // Map of Y-WireIDs
//     std::map<>
    
  }
  
  
  
  void  LaserReco::endJob()
  {
    TFile* OFile = new TFile("HitHist.root", "RECREATE");
    CollectionHits->Write();
    
    TCanvas* C2 = new TCanvas("Hits","You",1400,1000);
    CollectionHits->Draw();
    C2 -> Print("Hits.png","png");
    delete C2;
//     delete CollectionHits;
    
    if(fWireMapGenerator)
    {
      TFile* OutputFile = new TFile(fFileName.c_str(), "RECREATE");
    
      OutputFile->WriteObject(&UMap,"UMap");
      OutputFile->WriteObject(&VMap,"VMap");
      OutputFile->WriteObject(&YMap,"YMap");
    }
  }
   
  //-----------------------------------------------------------------------
//   void LaserReco::beginRun(const art::Run& /*run*/)
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
  void LaserReco::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameters from the .fcl file. The names in the arguments
    // to p.get<TYPE> must match names in the .fcl file.
    fWireMapGenerator        = parameterSet.get< bool        	     >("GenerateWireMap");
    fUPlaneThreshold	     = parameterSet.get< float        	     >("UPlaneHitThreshold");
    fVPlaneThreshold	     = parameterSet.get< float        	     >("VPlaneHitThreshold");
    fYPlaneThreshold	     = parameterSet.get< float        	     >("YPlaneHitThreshold");
    fCalDataModuleLabel      = parameterSet.get< art::InputTag	     >("CalDataModuleLabel");
//     fUVYThresholds	     = parameterSet.get< std::array<float,3> >("UVYHitThresholds");
  }

  //-----------------------------------------------------------------------
  void LaserReco::produce(art::Event& event) 
  {
    // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)   
    art::ValidHandle< std::vector<raw::RawDigit> > DigitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>(fCalDataModuleLabel);
    
    // Prepairing the wire signal vector. It will be just the raw signal with subtracted pedestial
    std::unique_ptr< std::vector<recob::Wire> > WireVec(new std::vector<recob::Wire>);
    
    // Prepairing the hit vector
    std::unique_ptr< std::vector<recob::Hit> > HitVec(new std::vector<recob::Hit>);
    
    // Preparing WireID vector
    std::vector<geo::WireID> WireIDs;
    
    
    WireVec->reserve(DigitVecHandle->size());
    
    unsigned RawIndex = 1477;
    unsigned UWireNumber = 1026;
    unsigned VWireNumber = 196;
    
    const lariov::IDetPedestalProvider& PedestalRetrievalAlg = art::ServiceHandle<lariov::IDetPedestalService>()->GetPedestalProvider();
//     TH1F* SingleWire = new TH1F("You","Fuck",DigitVecHandle->at(UMap.at(UWireNumber)).Samples(),0,DigitVecHandle->at(UMap.at(UWireNumber)).Samples()-1);
    
    std::vector<short> RawADC;
    std::vector<float> RawROI;
    
    unsigned int StartROI = 4500;
    unsigned int EndROI = 5500;
    
    float CollectionThreshold = 10.0;

    
    RawADC.resize(DigitVecHandle->at(0).Samples());
    RawROI.resize(DigitVecHandle->at(0).Samples());
    
    recob::Wire::RegionsOfInterest_t RegionOfInterest;
    
    // Loop over downstream laser system
//     if(fLCSNumber == 2)
//     {
      // Loop over 10 collection wires at the edge of the TPC based on the wire map
//       for(unsigned int WireIndex = YMap.size()-1; WireIndex >= YMap.size() - 10; WireIndex--)
//       {
        // Get the raw data for this particular wire
//         auto RawDigit = DigitVecHandle->at(YMap.at(WireIndex));
// 	raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());
        
        // copy only the region of interest into the the RawROI vector TODO Watch commented stuff
// 	std::copy(RawADC.begin()/*+StartROI*/, RawADC.begin()/*+EndROI*/, RawROI.begin());

        // Subtract pedestal from samples in ROI
// 	for(auto & RawSample : RawROI)
// 	{
// 	  RawSample -= PedestalRetrievalAlg.PedMean(RawDigit.Channel());
// 	}
	
        // Create a wire from the selected ROI (1. copy raw into ROI container, 2. copy the ROI vector into a wire container)
//         RegionOfInterest.add_range(StartROI,RawROI.begin(),RawROI.end());
// 	WireVec->emplace_back(recob::WireCreator( std::move(RegionOfInterest), RawDigit ).move());
	
        // Now look for hits in the ROI
// 	std::vector<recob::Hit> HitVector = LaserHitFinder(WireVec->back(), RawDigit.Channel());
	
// 	for(auto const& hit : HitVector)
// 	{
// 	  std::cout << "ADC count per Hit " << WireIndex << " " << hit.SummedADC() << std::endl;
// 	}
//       }      
//     }
    
//     WireVec->clear();
    
//     for(size_t rdIter = 0; rdIter < DigitVecHandle->size(); ++rdIter)

    unsigned int Index = 0;
    // Loop over all physical readout channels
    for(auto const & RawDigit : *DigitVecHandle)
    {
//       art::Ptr<raw::RawDigit> RawDigit(DigitVecHandle, rdIter);
      raw::ChannelID_t channel = RawDigit.Channel();
      
//       std::cout  << channel << std::endl;
      
      WireIDs = fGeometry->ChannelToWire(channel);
      unsigned int thePlane = WireIDs.front().Plane;
      unsigned int theWire = WireIDs.front().Wire;
      
      // Extract data into RawADC vector and uncompress it
      raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());
//       std::cout << "Fuuuuuck" << std::endl;
      
      // Move the Raw ADC digit (short) into the signal vector (float)
//       std::copy(RawADC.begin(), RawADC.end(), RawSignal.begin());
      std::copy(RawADC.begin(), RawADC.end(),RawROI.begin());
//       std::vector<float> RawSignal(RawADC.begin()+StartROI, RawADC.begin()+EndROI);
      
      // subtract pedestial
      for(auto & RawSample : RawROI)
      {
	RawSample -= PedestalRetrievalAlg.PedMean(channel);
// 	std::cout << RawSample << std::endl;
      }
//       for(unsigned int sample = StartROI; sample < EndROI; sample++ )
//       {
// 	RegionOfInterest.add_range(StartROI)
//       }
//       std::cout << "Fuuuuuck" << std::endl;
      RegionOfInterest.add_range(StartROI,RawROI.begin(),RawROI.end());
//       std::cout << "Fuuuuuck" << std::endl;
      
      // Create a Wire object with the raw signal
      WireVec->emplace_back(recob::WireCreator(std::move(RegionOfInterest),RawDigit).move());
      
//       std::cout << "Fuuuuuck" << std::endl;
      
//       if(Index==RawIndex)
//       {
// 	std::vector<recob::Hit> HitVector = LaserHitFinder(WireVec->back(), CollectionThreshold, channel);
// 	
// 	for(auto const& hit : HitVector)
// 	{
// // 	  std::cout << hit.SummedADC() << std::endl;
// 	}
//       }
//       std::cout << "Fuuuuuck" << std::endl;
      if(fGeometry->ChannelToWire(channel).front().Plane == 2) // If wire plane is Y-plane
      {
	std::vector<recob::Hit> YPlaneHits = LaserHitFinder(WireVec->back(), RawDigit.Channel());
	
	HitVec->insert(HitVec->end(), YPlaneHits.begin(), YPlaneHits.end());
	
	if(fWireMapGenerator) YMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
      }
      else if(fGeometry->ChannelToWire(channel).front().Plane == 1) // If wire plane is V-plane
      {
	std::vector<recob::Hit> VPlaneHits = VPlaneHitFinder(WireVec->back(), RawDigit.Channel());
	
	HitVec->insert(HitVec->end(), VPlaneHits.begin(), VPlaneHits.end());
	
	if(fWireMapGenerator) VMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
      }
      else if(fGeometry->ChannelToWire(channel).front().Plane == 0) // If wire plane is U-plane 
      {
	std::vector<recob::Hit> UPlaneHits = UPlaneHitFinder(WireVec->back(), RawDigit.Channel());
	
	HitVec->insert(HitVec->end(), UPlaneHits.begin(), UPlaneHits.end());
	
	if(fWireMapGenerator) UMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
      }
      
      Index++;
      
    }
    
    
//     float Pedestal = PedestalRetrievalAlg.PedMean(DigitVecHandle->at(UMap.at(UWireNumber)).Channel());
//     for(unsigned samples = 0; samples < DigitVecHandle->at(UMap.at(UWireNumber)).Samples(); samples++)
//       SingleWire->SetBinContent(samples,DigitVecHandle->at(UMap.at(UWireNumber)).ADC(samples) - Pedestal);
//     for(unsigned samples = 0; samples < WireVec->at(RawIndex).NSignal(); samples++)
//       SingleWire->SetBinContent(samples,WireVec->at(RawIndex).Signal().at(samples));
//     std::cout << "Pedestal " << PedestalRetrievalAlg.PedMean(DigitVecHandle->at(UMap.at(UWireNumber)).Channel()) << " " << PedestalRetrievalAlg.PedRms(DigitVecHandle->at(UMap.at(UWireNumber)).Channel()) << std::endl;
//     std::cout << "Errors " << PedestalRetrievalAlg.PedMeanErr(DigitVecHandle->at(UMap.at(UWireNumber)).Channel()) << " " << PedestalRetrievalAlg.PedRmsErr(DigitVecHandle->at(UMap.at(UWireNumber)).Channel()) << std::endl;
//     TCanvas* C1 = new TCanvas("Fuck","You",1400,1000);
//     SingleWire->Draw();
//     C1 -> Print("Fuck.png","png");
//     delete SingleWire;
//     delete C1;
//     event.put(std::move(WireVec), "blibla");
    event.put(std::move(HitVec), "LaserHits");
    
//     auto const& theWire = Wires[5];
    
//     raw::ChannelID_t theChannel = theWire.Channel();
//     std::vector<geo::WireID> wids = fGeometry->ChannelToWire(theChannel);
//     unsigned short thePlane = wids[0].Plane;
//     unsigned short theWireNum = wids[0].Wire;


  } // LaserReco::analyze()
  
  //------------------------------------------------------------------------
  std::vector<recob::Hit> LaserReco::LaserHitFinder(recob::Wire const& SingleWire, raw::ChannelID_t const& Channel)
  {
    std::vector<recob::Hit> LaserHits;
    
    int HitEnd = -9999;
    int HitStart = - 9999;
    float Peak = - 9999;
    int PeakTime = -9999;
    int HitIdx = 0;

    bool AboveThreshold = false;
 
    auto Signal = SingleWire.Signal();

    // loop over wire
    for(unsigned int sample = 0; sample < Signal.size(); sample++ )
    {
      if( Signal.at(sample) >= fYPlaneThreshold)
      {
        // If we go over the threshold the first time, save the time tick
        if (!AboveThreshold)
        {
          HitStart = sample;
          AboveThreshold = true;
          Peak = Signal.at(sample);
        }
        if (Signal.at(sample) > Peak) 
        {
          Peak = Signal.at(sample);
          PeakTime = sample;
        }
      }
      else if( AboveThreshold && Signal.at(sample) < fYPlaneThreshold)
      {
	HitEnd = sample;
        AboveThreshold = false;
	
// 	if(HitEnd-HitStart > 10.) CollectionHits->Fill(HitEnd-HitStart, Peak);
// 	std::cout << HitEnd - HitStart << " " << Peak << std::endl;

        LaserHits.push_back(recob::HitCreator(SingleWire, fGeometry->ChannelToWire(Channel).front(), HitStart, HitEnd, 
			  (float) (HitStart - HitEnd)/2, (float) PeakTime, 0.5, Peak, sqrt(Peak), 
			  (float)0., (float)0., (short int)1, HitIdx, (float)1., 0).move());
        HitIdx++;
      }
    }
        
    return LaserHits;
  }
  
  // U-Plane hit finder
  std::vector<recob::Hit> LaserReco::UPlaneHitFinder(recob::Wire const& SingleWire, raw::ChannelID_t const& Channel)
  {
    std::vector<recob::Hit> LaserHits;
    
    int HitEnd = -9999;
    int HitStart = - 9999;
    float Peak = - 9999;
    int PeakTime = -9999;
    int HitIdx = 0;

    bool BelowThreshold = false;
 
    auto Signal = SingleWire.Signal();

    // loop over wire
    for(unsigned int sample = 0; sample < Signal.size(); sample++ )
    {
      if( Signal.at(sample) <= fUPlaneThreshold)
      {
        // If we go over the threshold the first time, save the time tick
        if (!BelowThreshold)
        {
          HitStart = sample;
          BelowThreshold = true;
          Peak = Signal.at(sample);
        }
        if (Signal.at(sample) < Peak) 
        {
          Peak = Signal.at(sample);
          PeakTime = sample;
        }
      }
      else if( BelowThreshold && Signal.at(sample) > fUPlaneThreshold)
      {
	HitEnd = sample;
        BelowThreshold = false;
	
//         std::cout << "Hit " << HitIdx << "\n"
//                   << " Time: Start/Stop: " << HitStart << "/" << HitEnd << "\n" 
//                   << " Peak: Value/Tick: " << Peak << "/" << PeakTime << std::endl;

        LaserHits.push_back(recob::HitCreator(SingleWire, fGeometry->ChannelToWire(Channel).front(), HitStart, HitEnd, 
			  (float) (HitStart - HitEnd), (float) PeakTime, 0.5, Peak, sqrt(Peak), 
			  (float)0., (float)0., (short int)1, HitIdx, (float)1., 0).move());
        HitIdx++;
      }
    }
        
    return LaserHits;
  }
  
  //------------------------------------------------------------------------------
  
  std::vector<recob::Hit> LaserReco::VPlaneHitFinder(recob::Wire const& SingleWire, raw::ChannelID_t const& Channel)
  {
    std::vector<recob::Hit> LaserHits;
    
    int HitEnd = -9999;
    int HitStart = - 9999;
    float Peak = - 9999;
    float Dip = -9999;
    int PeakTime = -9999;
    int DipTime = -9999;
    int HitIdx = 0;

    bool AboveThreshold = false;
    bool BelowThreshold = false;
    bool Handover_flag = false;
 
    auto Signal = SingleWire.Signal();

    // loop over wire
    for(unsigned int sample = 0; sample < Signal.size(); sample++ )
    {
      if(!BelowThreshold && Signal.at(sample) >= fVPlaneThreshold)
      {
        // If we go over the threshold the first time, save the time tick
        if(!AboveThreshold)
        {
	  AboveThreshold = true;
	  Handover_flag = false;
          HitStart = sample;
          Peak = Signal.at(sample);
	  PeakTime = sample;
        }
        if(Signal.at(sample) > Peak) 
        {
          Peak = Signal.at(sample);
          PeakTime = sample;
        }
      }
      else if(AboveThreshold && Signal.at(sample) < fVPlaneThreshold)
      {
        AboveThreshold = false;
	Handover_flag = true;
      }
      
      if(Handover_flag && !AboveThreshold && Signal.at(sample) <= -fVPlaneThreshold)
      {
	if (!BelowThreshold)
        {
          HitStart = sample;
          BelowThreshold = true;
          Dip = Signal.at(sample);
	  DipTime = sample;
        }
        if (Signal.at(sample) < Dip) 
        {
          Dip = Signal.at(sample);
          DipTime = sample;
        }
      }
      else if(Handover_flag && BelowThreshold && Signal.at(sample) > -fVPlaneThreshold)
      {
	HitEnd = sample;
        BelowThreshold = false;
	Handover_flag = false;
	
	CollectionHits->Fill(DipTime-PeakTime, Peak);

        LaserHits.push_back(recob::HitCreator(SingleWire, fGeometry->ChannelToWire(Channel).front(), HitStart, HitEnd, 
			  (float) (HitEnd - HitStart)/2, (float) PeakTime, 0.5, Peak, sqrt(Peak), 
			  (float)0., (float)0., (short int)1, HitIdx, (float)1., 0).move());
        HitIdx++;
      }
      
    }
        
    return LaserHits;
  }
  
  
  
  // This macro has to be defined for this module to be invoked from a
  // .fcl file; see LaserReco.fcl for more information.

} // namespace LaserReco


namespace {
  // time to define that function...
  float DetectorDiagonal(geo::GeometryCore const& geom) {
    const double length = geom.DetLength();
    const double width = 2. * geom.DetHalfWidth();
    const double height = 2. * geom.DetHalfHeight();
    
    return std::sqrt(length*length + width*width + height*height);
  } // DetectorDiagonal()
} // local namespace


#endif // LaserReco_Module
