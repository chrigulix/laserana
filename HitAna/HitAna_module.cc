// HitAna_module.cc
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

#ifndef HitAna_Module
#define HitAna_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
// #include "Simulation/SimChannel.h"
// #include "Simulation/LArG4Parameters.h"
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
#include "TGraphAsymmErrors.h"
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

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace {
  
  // This is a anonymous namespace (the one below, instead, has a name:
  // "HitAna").
  // The stuff you declare in an anonymous namespace will not be visible
  // beyond this file (more technically, this "translation unit", that is
  // everything that gets included in the .cc file from now on).
  // In this way, you don't pollute the environment of other modules.
  
  // We will define this function at the end, but we at least declare it here
  // so that the module can freely use it.
  
} // local namespace


namespace HitAna {

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
  class HitAna : public art::EDAnalyzer
  {
  public:
 
    // Standard constructor for an ART module; we don't need a special
    // destructor here.
    /// Default constructor
    explicit HitAna(fhicl::ParameterSet const& parameterSet);

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
    
    virtual TGraphAsymmErrors* FillGraph(const std::vector<float>& WireNumber, 
					 const std::vector<float>& PeakTime,
					 const std::vector<float>& LowerHitTimeErr,
					 const std::vector<float>& UpperHitTimeErr);
    

  private:

    // The stuff below is the part you'll most likely have to change to
    // go from this custom example to your own task.

    // The parameters we'll read from the .fcl file.
    //art::InputTag fCalDataModuleLabel;    ///< CalData module label
    std::string fHitModuleLabel;
    std::string fUPlaneInstanceLabel;  ///< Name of the U-plane hit data product
    std::string fVPlaneInstanceLabel;  ///< Name of the V-plane hit data product
    std::string fYPlaneInstanceLabel;  ///< Name of the Y-plane hit data product
    bool fDrawHits; ///< Determines if you draw hits or not
    
    // Initialize input tags for getting hit data
    art::InputTag fUPlaneTag;
    art::InputTag fVPlaneTag;
    art::InputTag fYPlaneTag;
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
    
    // Hits Analysis Histogram
    TH2D* fUHitWidthVsPeak;
    TH2D* fUPeakDivWidth;
    
    TH2D* fVHitWidthVsPeak;
    TH2D* fVHitPeakDistVsPeak;
    TH2D* fVHitPeakDistVsWidth;
    TH2D* fVPeakDivWidth;
    TH2D* fVPeakDivPeakDist;
    
    TH2D* fYHitWidthVsPeak;
    TH2D* fYPeakDivWidth;
    
    // Hit Graphs for all planes
    TGraphAsymmErrors* fUPlaneHits;
    TGraphAsymmErrors* fVPlaneHits;
    TGraphAsymmErrors* fYPlaneHits;
    
    // Canvases for drawing hit graphs
    TCanvas* fUCanvas;
    TCanvas* fVCanvas;
    TCanvas* fYCanvas;
    
  }; // class HitAna
  
  DEFINE_ART_MODULE(HitAna)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  HitAna::HitAna(fhicl::ParameterSet const& pset) : EDAnalyzer(pset)
  {
    // get a pointer to the geometry service provider
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
  }

  
  //-----------------------------------------------------------------------
  void HitAna::beginJob()
  {
    // If the event draw flag is enabled
    if(fDrawHits)
    {
      fUCanvas = new TCanvas("U-Plane Hits","U-Plane Hits",1000,700);
      fVCanvas = new TCanvas("V-Plane Hits","V-Plane Hits",1000,700);
      fYCanvas = new TCanvas("Y-Plane Hits","Y-Plane Hits",1000,700);
    }
    else
    {
      // Initialize the Art TFile service
      art::ServiceHandle<art::TFileService> TFileServiceHandle;
    
      // Make histograms
      fUHitWidthVsPeak	   = TFileServiceHandle->make<TH2D>("U-Plane Width vs. Peak","Width vs. Peak",1000,0,1000,2100,0,2100);
      fUPeakDivWidth	   = TFileServiceHandle->make<TH2D>("U-Plane Width vs. Peak/Width","Peak height devided by Width",1000,0,1000,2100,0,2100);
      fVHitWidthVsPeak	   = TFileServiceHandle->make<TH2D>("V-Plane Width vs. Peak","Widtht vs. Peak",1000,0,1000,4100,0,4100);
      fVHitPeakDistVsPeak  = TFileServiceHandle->make<TH2D>("V-Plane Peak Dist vs. Peak","PeakDist vs. Peak",1000,0,1000,4100,0,4100);
      fVHitPeakDistVsWidth = TFileServiceHandle->make<TH2D>("V-Plane Peak Dist vs. Hit Width","PeakDist vs. HitWidth",1000,0,1000,1000,0,1000);
      fVPeakDivWidth	   = TFileServiceHandle->make<TH2D>("V-Plane Width vs. Peak/Width","Peak height devided by Width",1000,0,1000,4100,0,4100);
      fVPeakDivPeakDist	   = TFileServiceHandle->make<TH2D>("V-Plane PeakDist vs. Peak/PeakDist","Peak height devided by Peak distance",1000,0,1000,4100,0,4100);
      fYHitWidthVsPeak	   = TFileServiceHandle->make<TH2D>("Y-Plane Width vs. Peak","Width vs. Peak",1000,0,1000,3500,0,3500);
      fYPeakDivWidth	   = TFileServiceHandle->make<TH2D>("Y-Plane Width vs. Peak/Width","Peak height devided by Width",1000,0,1000,3500,0,3500);
      gROOT->Reset();
    }
  }
  
  
  
  void  HitAna::endJob()
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
  void HitAna::beginRun(const art::Run& /*run*/)
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
  void HitAna::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameters from the .fcl file. The names in the arguments
    // to p.get<TYPE> must match names in the .fcl file.
    fHitModuleLabel         = parameterSet.get< std::string >("HitModuleLabel");
    fUPlaneInstanceLabel    = parameterSet.get< std::string >("UPlaneInstanceLabel");
    fVPlaneInstanceLabel    = parameterSet.get< std::string >("VPlaneInstanceLabel");
    fYPlaneInstanceLabel    = parameterSet.get< std::string >("YPlaneInstanceLabel");
    fDrawHits		    = parameterSet.get< bool 	    >("DrawHits");

    // Create input tags
    fUPlaneTag = art::InputTag(fHitModuleLabel,fUPlaneInstanceLabel);
    fVPlaneTag = art::InputTag(fHitModuleLabel,fVPlaneInstanceLabel);
    fYPlaneTag = art::InputTag(fHitModuleLabel,fYPlaneInstanceLabel);
  }

  //-----------------------------------------------------------------------
  void HitAna::analyze(const art::Event& event) 
  {
    // This is the handle to the hit data of this event (simply a pointer to std::vector<recob::hit>)   
    art::ValidHandle< std::vector<recob::Hit> > UPlaneHitVecHandle = event.getValidHandle<std::vector<recob::Hit>>(fUPlaneTag);
    art::ValidHandle< std::vector<recob::Hit> > VPlaneHitVecHandle = event.getValidHandle<std::vector<recob::Hit>>(fVPlaneTag);
    art::ValidHandle< std::vector<recob::Hit> > YPlaneHitVecHandle = event.getValidHandle<std::vector<recob::Hit>>(fYPlaneTag);
    
    std::vector<float> HitWireNumber;
    std::vector<float> HitTimeBin;
    std::vector<float> LowerHitTimeErr;
    std::vector<float> UpperHitTimeErr;
    
    HitWireNumber.reserve(UPlaneHitVecHandle->size());
    HitTimeBin.reserve(UPlaneHitVecHandle->size());
    LowerHitTimeErr.reserve(UPlaneHitVecHandle->size());
    UpperHitTimeErr.reserve(UPlaneHitVecHandle->size());
    
    
    // Loop over all u-plane hits
    for(auto const & UPlaneHit : *UPlaneHitVecHandle)
    {
      // Fill histograms
      if(!fDrawHits)
      {
	fUHitWidthVsPeak->Fill(UPlaneHit.EndTick()-UPlaneHit.StartTick(),-UPlaneHit.PeakAmplitude());
	fUPeakDivWidth->Fill(UPlaneHit.EndTick()-UPlaneHit.StartTick(),-UPlaneHit.PeakAmplitude()/(UPlaneHit.EndTick()-UPlaneHit.StartTick()));
      }
      // Fill vector for T-Graph with cuts
//       if( fabs(UPlaneHit.PeakAmplitude()) > 20 && fabs(UPlaneHit.RMS()) > 5)
      else if(   fabs(UPlaneHit.PeakAmplitude()) > 25 
	      &&(fabs(UPlaneHit.PeakAmplitude())/(UPlaneHit.EndTick()-UPlaneHit.StartTick()) > 1 || UPlaneHit.PeakAmplitude() > 1000) 
	      && UPlaneHit.EndTick()-UPlaneHit.StartTick() > 10)
      {
	HitWireNumber.push_back((float)UPlaneHit.WireID().Wire);
	HitTimeBin.push_back((float)UPlaneHit.PeakTime());
	LowerHitTimeErr.push_back((float)UPlaneHit.PeakTime()-UPlaneHit.StartTick());
	UpperHitTimeErr.push_back(UPlaneHit.EndTick()-(float)UPlaneHit.PeakTime());
      }
    }// end loop over u-plane hits entries
    
    if(HitWireNumber.size())
    {
      fUCanvas->cd();
      fUPlaneHits = FillGraph(HitWireNumber,HitTimeBin,LowerHitTimeErr,UpperHitTimeErr);
    }
    
    HitWireNumber.clear();
    HitTimeBin.clear();
    LowerHitTimeErr.clear();
    UpperHitTimeErr.clear();
    
    
    HitWireNumber.reserve(VPlaneHitVecHandle->size());
    HitTimeBin.reserve(VPlaneHitVecHandle->size());
    LowerHitTimeErr.reserve(VPlaneHitVecHandle->size());
    UpperHitTimeErr.reserve(VPlaneHitVecHandle->size());
    
    // Loop over all v-plane hits 
    for(auto const & VPlaneHit : *VPlaneHitVecHandle)
    {
      // Fill histograms
      if(!fDrawHits)
      {
	fVHitWidthVsPeak->Fill(VPlaneHit.EndTick()-VPlaneHit.StartTick(),VPlaneHit.PeakAmplitude());
	fVHitPeakDistVsPeak->Fill(VPlaneHit.RMS()*2.,VPlaneHit.PeakAmplitude());
	fVHitPeakDistVsWidth->Fill(VPlaneHit.RMS()*2.,VPlaneHit.EndTick()-VPlaneHit.StartTick());
	fVPeakDivWidth->Fill(VPlaneHit.EndTick()-VPlaneHit.StartTick(),VPlaneHit.PeakAmplitude()/(VPlaneHit.EndTick()-VPlaneHit.StartTick()));
	fVPeakDivPeakDist->Fill(VPlaneHit.RMS()*2.,VPlaneHit.PeakAmplitude()/VPlaneHit.RMS()*2.);
      }
      // Fill vector for T-Graph with cuts
//       if(VPlaneHit.PeakAmplitude() > 20 && fabs(VPlaneHit.RMS()) < 5 && fabs(VPlaneHit.EndTick()-VPlaneHit.StartTick()) > 5)
      else if(   VPlaneHit.PeakAmplitude()/(VPlaneHit.EndTick()-VPlaneHit.StartTick()) > 1 
	      && VPlaneHit.EndTick()-VPlaneHit.StartTick() > 12
	      &&(VPlaneHit.PeakAmplitude()/(VPlaneHit.RMS()*2.) > 5 || VPlaneHit.PeakAmplitude() > 1000)  
	      && VPlaneHit.RMS()*2. > 4
	      /*&& VPlaneHit.RMS()*2. < 20*/)
      {
	HitWireNumber.push_back((float)VPlaneHit.WireID().Wire);
	HitTimeBin.push_back((float)VPlaneHit.PeakTime());
	LowerHitTimeErr.push_back((float)VPlaneHit.PeakTime()-VPlaneHit.StartTick());
	UpperHitTimeErr.push_back(VPlaneHit.EndTick()-(float)VPlaneHit.PeakTime());
      }
    } // end loop over v-plane hits entries
    
    if(HitWireNumber.size())
    {
      fVCanvas->cd();
      fVPlaneHits = FillGraph(HitWireNumber,HitTimeBin,LowerHitTimeErr,UpperHitTimeErr);
    }
    
    HitWireNumber.clear();
    HitTimeBin.clear();
    LowerHitTimeErr.clear();
    UpperHitTimeErr.clear();
    
    HitWireNumber.reserve(YPlaneHitVecHandle->size());
    HitTimeBin.reserve(YPlaneHitVecHandle->size());
    LowerHitTimeErr.reserve(YPlaneHitVecHandle->size());
    UpperHitTimeErr.reserve(YPlaneHitVecHandle->size());
    
    // Loop over all y-plane hits
    for(auto const & YPlaneHit : *YPlaneHitVecHandle)
    {
      // Fill histograms
      if(!fDrawHits)
      {
	fYHitWidthVsPeak->Fill(YPlaneHit.EndTick()-YPlaneHit.StartTick(),YPlaneHit.PeakAmplitude());
	fYPeakDivWidth->Fill(YPlaneHit.EndTick()-YPlaneHit.StartTick(),YPlaneHit.PeakAmplitude()/(YPlaneHit.EndTick()-YPlaneHit.StartTick()));
      }
      // Fill vector for T-Graph with cuts
      else if(  (YPlaneHit.PeakAmplitude()/(YPlaneHit.EndTick()-YPlaneHit.StartTick()) > 1.5 || YPlaneHit.PeakAmplitude() > 1000)
	      && YPlaneHit.EndTick()-YPlaneHit.StartTick() > 6
	      /*&& fabs(YPlaneHit.RMS()) < 100*/)
      {
	HitWireNumber.push_back((float)YPlaneHit.WireID().Wire);
	HitTimeBin.push_back((float)YPlaneHit.PeakTime());
	LowerHitTimeErr.push_back((float)YPlaneHit.PeakTime()-YPlaneHit.StartTick());
	UpperHitTimeErr.push_back(YPlaneHit.EndTick()-(float)YPlaneHit.PeakTime());
      }
    }// end loop over y-plane hits entries
    
    if(HitWireNumber.size())
    {
      fYCanvas->cd();
      fYPlaneHits = FillGraph(HitWireNumber,HitTimeBin,LowerHitTimeErr,UpperHitTimeErr);
    }
    
    while(fDrawHits)
    {
      if(!gROOT->IsBatch())
      {
	fUCanvas->cd();
	if(HitWireNumber.size()) fUPlaneHits->Draw("ap");
	fUCanvas->Modified();
	fUCanvas->Update();
	
	fVCanvas->cd();
	if(HitWireNumber.size()) fVPlaneHits->Draw("ap");
	fVCanvas->Modified();
	fVCanvas->Update();
	
	fYCanvas->cd();
	if(HitWireNumber.size()) fYPlaneHits->Draw("ap");
	fYCanvas->Modified();
	fYCanvas->Update();
      }
      else
      {
	std::cout << "You are not in batch mode! Now drawings!" << std::endl; 
      }
      
//       std::string response;
//       std::cout << "Proceed? " << std::endl;
//       std::cin >> response;
      gSystem->Sleep(500);
      if(gSystem->ProcessEvents()) break;
    }// end Draw loop
    
  } // HitAna::analyze()
  
  TGraphAsymmErrors* HitAna::FillGraph(const std::vector<float>& WireNumber, 
				       const std::vector<float>& PeakTime,
				       const std::vector<float>& LowerHitTimeErr,
				       const std::vector<float>& UpperHitTimeErr)
  {
    std::vector<float> WireError(WireNumber.size(),0.5);
    
    TVectorT<float> TWireNumber(WireNumber.size(),WireNumber.data());
    TVectorT<float> TTimeBin(PeakTime.size(),PeakTime.data());
    TVectorT<float> TLowerHitError(LowerHitTimeErr.size(),LowerHitTimeErr.data());
    TVectorT<float> TUpperHitError(UpperHitTimeErr.size(),UpperHitTimeErr.data());
    TVectorT<float> TWireError(WireError.size(),WireError.data());
    
    
    return new TGraphAsymmErrors(TWireNumber,TTimeBin,TWireError,TWireError,TLowerHitError,TUpperHitError);
  }

} // namespace HitAna

#endif // HitAna_Module
