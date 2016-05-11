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
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Cluster.h"
#include "lardata/RecoBase/Wire.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "larcore/CoreUtils/ServiceUtil.h"

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
#include "lardata/Utilities/AssociationUtil.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

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
#include "LaserObjects/LaserHits.h"


namespace {
  
  // This is a anonymous namespace (the one below, instead, has a name:
  // "LaserReco").
  // The stuff you declare in an anonymous namespace will not be visible
  // beyond this file (more technically, this "translation unit", that is
  // everything that gets included in the .cc file from now on).
  // In this way, you don't pollute the environment of other modules.
  
  // We will define this function at the end, but we at least declare it here
  // so that the module can freely use it.
  
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
    
    // Wire crossing finder
    std::vector< std::pair<geo::WireID, geo::WireID> > CrossingWireRanges(geo::WireID WireID);
    
    void CutRegionOfInterest();

  private:

    // The stuff below is the part you'll most likely have to change to
    // go from this custom example to your own task.

    // The parameters we'll read from the .fcl file.
    bool fWireMapGenerator;               ///< Decide if you want to produce a wire map (recomended if you don't have it)
    float fUPlaneThreshold;		  ///< U-plane threshold in ADC counts for the laser hit finder
    float fVPlaneThreshold;		  ///< V-plane threshold in ADC counts for the laser hit finder
    float fYPlaneThreshold;		  ///< Y-plane threshold in ADC counts for the laser hit finder
    art::InputTag fCalDataModuleLabel;    ///< CalData module label
    int fMinAllowedChanStatus;		  ///< Channel status number
    
    // Coppy thresholds into array
    std::array<float,3> fUVYThresholds;	  ///< U,V,Y-plane threshold in ADC counts for the laser hit finder
    
    // Other variables that will be shared between different methods.
    geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
    
    detinfo::DetectorProperties const* fDetProperties;  ///< pointer to detector properties provider
    
    std::string fFileName = "WireIndexMap.root";
    
    std::vector< std::map<unsigned int, unsigned int> > WireMaps;
    
    unsigned short fLCSNumber;
    
  }; // class LaserReco
  
  DEFINE_ART_MODULE(LaserReco)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  LaserReco::LaserReco(fhicl::ParameterSet const& pset)
  {
    // get a pointer to the geometry service provider
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    fDetProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();
    
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
    
//     produces< std::vector<recob::Wire> >("blibla");
    produces< std::vector<recob::Hit> >("UPlaneLaserHits");
    produces< std::vector<recob::Hit> >("VPlaneLaserHits");
    produces< std::vector<recob::Hit> >("YPlaneLaserHits");
  }

  
  //-----------------------------------------------------------------------
  void LaserReco::beginJob()
  {
    // Get the detector length, to determine the maximum bin edge of one
    // of the histograms.
    
    if(!fWireMapGenerator)
    {
      TFile* InputFile = new TFile(fFileName.c_str(), "READ");
      
      std::map<unsigned int, unsigned int>* pUMap;
      std::map<unsigned int, unsigned int>* pVMap;
      std::map<unsigned int, unsigned int>* pYMap;
      
      InputFile->GetObject("UMap",pUMap);
      InputFile->GetObject("VMap",pVMap);
      InputFile->GetObject("YMap",pYMap);
      
      WireMaps.push_back(std::move(*pUMap));
      WireMaps.push_back(std::move(*pVMap));
      WireMaps.push_back(std::move(*pYMap));
      
      delete pUMap;
      delete pVMap;
      delete pYMap;
    }
    
//     CollectionHits = new TH2D("PeakDist vs.HitWidth","PeakDist vs. HitWidth",3000,0,3000,3500,0,3500);
    
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
//     TCanvas* C2 = new TCanvas("Hits","You",1400,1000);
//     CollectionHits->Draw();
//     C2 -> Print("Hits.png","png");
//     delete C2;
//     delete CollectionHits;
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
    fMinAllowedChanStatus    = parameterSet.get< int 		     >("MinAllowedChannelStatus");
    
//     fUVYThresholds	     = parameterSet.get< std::array<float,3> >("UVYHitThresholds");
  }

  //-----------------------------------------------------------------------
  void LaserReco::produce(art::Event& event) 
  {
    // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)   
    art::ValidHandle< std::vector<raw::RawDigit> > DigitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>(fCalDataModuleLabel);
    
    // Prepairing the wire signal vector. It will be just the raw signal with subtracted pedestial
    std::vector<recob::Wire> WireVec;
    
    // Prepairing the hit vectors for all planes
    std::unique_ptr< std::vector<recob::Hit> > UHitVec(new std::vector<recob::Hit>);
    std::unique_ptr< std::vector<recob::Hit> > VHitVec(new std::vector<recob::Hit>);
    std::unique_ptr< std::vector<recob::Hit> > YHitVec(new std::vector<recob::Hit>);
    
    // Preparing WireID vector
    std::vector<geo::WireID> WireIDs;
    
    // Reserve space for wire vector
    WireVec.reserve(DigitVecHandle->size());
    
    // Get Service providers
    const lariov::DetPedestalProvider& PedestalRetrievalAlg = art::ServiceHandle<lariov::DetPedestalService>()->GetPedestalProvider();
    const lariov::ChannelStatusProvider& ChannelFilter = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();
    
    // Initialize raw time tick vectors
    std::vector<short> RawADC;
    std::vector<float> RawROI;
    RawADC.resize(DigitVecHandle->at(0).Samples());
    RawROI.resize(DigitVecHandle->at(0).Samples());
    
    // Prepare laser hits object
    LaserObjects::LaserHits YROIHits(fGeometry,fUVYThresholds);
    
    // Set region of interest limits for first hit scan
    unsigned int StartROI = 4500;
    unsigned int EndROI = 5500;
    
    // Initialize Regions of interest
    recob::Wire::RegionsOfInterest_t RegionOfInterestFirst;
    recob::Wire::RegionsOfInterest_t RegionOfInterest;
    
    // Loop over downstream laser system
    if(fLCSNumber == 2)
    {
      TVector3 LaserPos(100,0,-25);
      TVector3 LaserDir(0,0,1);
      
      auto aa = fGeometry->TPC().GetIntersections(LaserPos, LaserDir);
      
      aa.at(0).Print();
      
      aa.at(1).Print();
      
      // Loop over N collection wires at the edge of the TPC based on the wire map
      for(unsigned int WireIndex = WireMaps.back().size()-1; WireIndex >= WireMaps.back().size() - 13; WireIndex--)
      {
        // Get the raw data for this particular wire
        auto RawDigit = DigitVecHandle->at(WireMaps.back().at(WireIndex));
	
	// Get Channel number
	raw::ChannelID_t channel = RawDigit.Channel();
	
	// Skip channel if dead or noisy
	if( ChannelFilter.Status(channel) < fMinAllowedChanStatus || !ChannelFilter.IsPresent(channel) )
	{
	  continue;
	}
	
	// Uncompress raw digits into RawADC vector
	raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());
        
        // copy only the region of interest into the the RawROI vector TODO Watch commented stuff
	std::copy(RawADC.begin()+StartROI, RawADC.begin()+EndROI, RawROI.begin());

        // Subtract pedestal from samples in ROI
	for(auto & RawSample : RawROI)
	{
	  RawSample -= PedestalRetrievalAlg.PedMean(channel);
	}
	
        // Create a wire from the selected ROI (1. copy raw into ROI container, 2. copy the ROI vector into a wire container)
        RegionOfInterestFirst.add_range(StartROI,RawROI.begin(),RawROI.end());
	
	// Search for hits with the created wire
	YROIHits.AddHitsFromWire(recob::WireCreator( std::move(RegionOfInterestFirst), RawDigit ).move());
	
      }// end for over endge collection wires
      
      // Check if there are more than x hits in the selected area and jump event if there are less
      if( YROIHits.NumberOfWiresWithHits().at(2) < 7 ) 
      {
	event.put(std::move(UHitVec), "UPlaneLaserHits");
	event.put(std::move(VHitVec), "VPlaneLaserHits");
	event.put(std::move(YHitVec), "YPlaneLaserHits");
	return;
      }
    }// end if laser system number two
    

    // Loop over all raw digit entries
    for(auto const & RawDigit : *DigitVecHandle)
    {
      // Get channel ID
      raw::ChannelID_t channel = RawDigit.Channel();
      
      // Skip channel if dead or noisy
      if( ChannelFilter.Status(channel) < fMinAllowedChanStatus || !ChannelFilter.IsPresent(channel) )
      {
	continue;// jump to next iterator in RawDigit loop
      }
      
      // Extract data into RawADC vector and uncompress it
      raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());
      
      // Copy the Raw ADC digit (short) into the signal vector (float)
      std::copy(RawADC.begin(), RawADC.end(),RawROI.begin());
      
      // subtract pedestial
      for(auto & RawSample : RawROI)
      {
	RawSample -= PedestalRetrievalAlg.PedMean(channel);
      }
      
      // Create the region of interest (in this case the whole wire)
      RegionOfInterest.add_range(0,RawROI.begin(),RawROI.end());
      
      // Create a Wire object with the raw signal
      WireVec.emplace_back(recob::WireCreator(std::move(RegionOfInterest),RawDigit).move());
      
    } // end loop over raw digit entries
    
    // Create Laser Hits out of Wires
    LaserObjects::LaserHits AllLaserHits(WireVec,fGeometry,fUVYThresholds);
    
    // Filter for time matches of at least two planes
//     AllLaserHits.TimeMatchFilter();
    
    // Fill plane specific hit vectors
    UHitVec = AllLaserHits.GetPlaneHits(0);
    VHitVec = AllLaserHits.GetPlaneHits(1);
    YHitVec = AllLaserHits.GetPlaneHits(2);
    
    
    
    std::cout << fDetProperties->ConvertXToTicks(100,2,0,0) << std::endl;
    
    std::cout << UHitVec->size() << std::endl;
    std::cout << VHitVec->size() << std::endl;
    std::cout << YHitVec->size() << std::endl;
    
    // Fill Hits of all planes into the new
    event.put(std::move(UHitVec), "UPlaneLaserHits");
    event.put(std::move(VHitVec), "VPlaneLaserHits");
    event.put(std::move(YHitVec), "YPlaneLaserHits");
  } // LaserReco::analyze()
  
  // Gives out a vector of WireIDs which cross a certain input wire
  std::vector< std::pair<geo::WireID, geo::WireID> > LaserReco::CrossingWireRanges(geo::WireID WireID)
  {
    // Initialize the return pair vector
    std::vector< std::pair<geo::WireID, geo::WireID> > CrossingWireRangeVec;
    
    // Initialize start and end point of wire
    double Start[3], End[3];
    
    // Fill the wire End points
    fGeometry->WireEndPoints(WireID, Start, End);
    
    // Loop over target plane number
    for(unsigned int plane_no = 0; plane_no < 3; plane_no++)
    {
      // Search for crossing wires only if the aren't on the same plane
      if(plane_no != WireID.Plane)
      {
	// Generate PlaneID for microboone (cryostatID = 0, TPCID = 0, plane number)
	auto TargetPlaneID = geo::PlaneID(0,0,plane_no);
	
	// Get the closest wire on the target plane to the wire start position
	auto FirstWireID = fGeometry->NearestWireID(Start, TargetPlaneID);
	
	// Get the closest wire on the target plane to the wire end postion
	auto LastWireID = fGeometry->NearestWireID(End, TargetPlaneID);
	
	// Check if wires are in right order
	if(FirstWireID.Wire > LastWireID.Wire)
	{
	  // Swap them if order is decending
	  std::swap(FirstWireID, LastWireID);
	}
	
	// Generate a dummy intersection coordinate, because shit only works that way (eye roll)
	geo::WireIDIntersection DummyMcDumbFace;
	
	// Check if first and last wires are crossing, if not take one next to them
	if( !fGeometry->WireIDsIntersect(WireID, FirstWireID, DummyMcDumbFace) )
	{
	  ++(FirstWireID.Wire);
	}
	if( !fGeometry->WireIDsIntersect(WireID, LastWireID, DummyMcDumbFace) )
	{
	  --(LastWireID.Wire);
	}
	CrossingWireRangeVec.push_back( std::make_pair(FirstWireID,LastWireID) );
      }
    }
    return CrossingWireRangeVec;
  }

} // namespace LaserReco

#endif // LaserReco_Module
