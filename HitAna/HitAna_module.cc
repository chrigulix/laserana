#ifndef HitAna_Module
#define HitAna_Module

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
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"

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
#include "TLine.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
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
#include "LaserObjects/LaserROI.h"
#include "LaserObjects/LaserBeam.h"


namespace 
{
  
} // local namespace


namespace HitAna {

  class HitAna : public art::EDAnalyzer
  {
  public:
 
    // Standard constructor for an ART module; we don't need a special
    // destructor here.
    /// Default constructor
    explicit HitAna(fhicl::ParameterSet const& parameterSet);


    virtual void beginJob() override;
    
    virtual void endJob() override;

    virtual void beginRun(const art::Run& run) override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    virtual void analyze(const art::Event& event) override;
    
    virtual TGraphAsymmErrors* FillGraph(const std::vector<float>& WireNumber, 
					 const std::vector<float>& PeakTime,
					 const std::vector<float>& LowerHitTimeErr,
					 const std::vector<float>& UpperHitTimeErr);
    

  private:

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
    
    // Hit Graphs for selected ROI of all planes
    TGraphAsymmErrors* fUPlaneHitsSelected;
    TGraphAsymmErrors* fVPlaneHitsSelected;
    TGraphAsymmErrors* fYPlaneHitsSelected;
    
    TLine* fULaserBeam;
    TLine* fVLaserBeam;
    TLine* fYLaserBeam;
    
    // Hit Graphs for selected ROI of all planes
    TMultiGraph* fUPlaneTot;
    TMultiGraph* fVPlaneTot;
    TMultiGraph* fYPlaneTot;
    
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
      
      fUPlaneTot = new TMultiGraph();
      fVPlaneTot = new TMultiGraph();
      fYPlaneTot = new TMultiGraph();
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
  
  //-----------------------------------------------------------------------
  void  HitAna::endJob()
  {
      
  }
   
  //-----------------------------------------------------------------------
  void HitAna::beginRun(const art::Run& /*run*/)
  {
      
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
    
    // Read Laser Beam data
    auto LaserTag = art::InputTag("LaserDataMerger", "LaserBeam");
    art::ValidHandle< lasercal::LaserBeam > LaserBeamHandle = event.getValidHandle< lasercal::LaserBeam >(LaserTag);
    
    lasercal::LaserROI ROI(10.0, *LaserBeamHandle);
    
    std::vector<float> HitWireNumber;
    std::vector<float> HitTimeBin;
    std::vector<float> LowerHitTimeErr;
    std::vector<float> UpperHitTimeErr;
    
    std::vector<float> HitWireNumberSel;
    std::vector<float> HitTimeBinSel;
    std::vector<float> LowerHitTimeErrSel;
    std::vector<float> UpperHitTimeErrSel;
    
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
	      &&(fabs(UPlaneHit.PeakAmplitude())/(UPlaneHit.EndTick()-UPlaneHit.StartTick()) > 1 || fabs(UPlaneHit.PeakAmplitude()) > 1000) 
	      && UPlaneHit.EndTick()-UPlaneHit.StartTick() > 10)
      {
	  if(!ROI.IsHitInRange(UPlaneHit))
	  {
	      HitWireNumber.push_back((float)UPlaneHit.WireID().Wire);
	      HitTimeBin.push_back((float)UPlaneHit.PeakTime());
	      LowerHitTimeErr.push_back((float)UPlaneHit.PeakTime()-UPlaneHit.StartTick());
	      UpperHitTimeErr.push_back(UPlaneHit.EndTick()-(float)UPlaneHit.PeakTime());
	  }
	  else
	  {
	      HitWireNumberSel.push_back((float)UPlaneHit.WireID().Wire);
	      HitTimeBinSel.push_back((float)UPlaneHit.PeakTime());
	      LowerHitTimeErrSel.push_back((float)UPlaneHit.PeakTime()-UPlaneHit.StartTick());
	      UpperHitTimeErrSel.push_back(UPlaneHit.EndTick()-(float)UPlaneHit.PeakTime());
	  }
      }
    }// end loop over u-plane hits entries
    
    if(HitWireNumber.size())
    {
	fUCanvas->cd();
	fUPlaneHits = FillGraph(HitWireNumber,HitTimeBin,LowerHitTimeErr,UpperHitTimeErr);
	fUPlaneTot->Add(fUPlaneHits);
    }
    if(HitWireNumberSel.size())
    {
	fUPlaneHitsSelected = FillGraph(HitWireNumberSel,HitTimeBinSel,LowerHitTimeErrSel,UpperHitTimeErrSel);
	fUPlaneHitsSelected->SetLineColor(2);
	fUPlaneTot->Add(fUPlaneHitsSelected);
	
	fULaserBeam = new TLine(ROI.GetEntryWire(0),ROI.GetEntryTimeTick(0),ROI.GetExitWire(0),ROI.GetExitTimeTick(0));
	fULaserBeam->SetLineColor(3);
    }
    
    HitWireNumber.clear();
    HitTimeBin.clear();
    LowerHitTimeErr.clear();
    UpperHitTimeErr.clear();
    
    HitWireNumberSel.clear();
    HitTimeBinSel.clear();
    LowerHitTimeErrSel.clear();
    UpperHitTimeErrSel.clear();
    
    
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
      else if(  (VPlaneHit.PeakAmplitude()/(VPlaneHit.EndTick()-VPlaneHit.StartTick()) > 1 || VPlaneHit.PeakAmplitude() > 1000)
	      && VPlaneHit.EndTick()-VPlaneHit.StartTick() > 12
	      &&(VPlaneHit.PeakAmplitude()/(VPlaneHit.RMS()*2.) > 2 || VPlaneHit.PeakAmplitude() > 1000)  
	      && VPlaneHit.RMS()*2. > 4
	      /*&& VPlaneHit.RMS()*2. < 20*/)
      {
	  if(!ROI.IsHitInRange(VPlaneHit))
	  {
	      HitWireNumber.push_back((float)VPlaneHit.WireID().Wire);
	      HitTimeBin.push_back((float)VPlaneHit.PeakTime());
	      LowerHitTimeErr.push_back((float)VPlaneHit.PeakTime()-VPlaneHit.StartTick());
	      UpperHitTimeErr.push_back(VPlaneHit.EndTick()-(float)VPlaneHit.PeakTime());
	  }
	  else
	  {
	      HitWireNumberSel.push_back((float)VPlaneHit.WireID().Wire);
	      HitTimeBinSel.push_back((float)VPlaneHit.PeakTime());
	      LowerHitTimeErrSel.push_back((float)VPlaneHit.PeakTime()-VPlaneHit.StartTick());
	      UpperHitTimeErrSel.push_back(VPlaneHit.EndTick()-(float)VPlaneHit.PeakTime());
	  }
      }
    } // end loop over v-plane hits entries
    
    if(HitWireNumber.size())
    {
	fVCanvas->cd();
	fVPlaneHits = FillGraph(HitWireNumber,HitTimeBin,LowerHitTimeErr,UpperHitTimeErr);
	fVPlaneTot->Add(fVPlaneHits);
    }
    if(HitWireNumberSel.size())
    {
	fVPlaneHitsSelected = FillGraph(HitWireNumberSel,HitTimeBinSel,LowerHitTimeErrSel,UpperHitTimeErrSel);
	fVPlaneHitsSelected->SetLineColor(2);
	fVPlaneTot->Add(fVPlaneHitsSelected);
	
	fVLaserBeam = new TLine(ROI.GetEntryWire(1),ROI.GetEntryTimeTick(1),ROI.GetExitWire(1),ROI.GetExitTimeTick(1));
	fVLaserBeam->SetLineColor(3);
    }
    
    
    HitWireNumber.clear();
    HitTimeBin.clear();
    LowerHitTimeErr.clear();
    UpperHitTimeErr.clear();
    
    HitWireNumberSel.clear();
    HitTimeBinSel.clear();
    LowerHitTimeErrSel.clear();
    UpperHitTimeErrSel.clear();
    
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
	  if(!ROI.IsHitInRange(YPlaneHit))
	  {
	      HitWireNumber.push_back((float)YPlaneHit.WireID().Wire);
	      HitTimeBin.push_back((float)YPlaneHit.PeakTime());
	      LowerHitTimeErr.push_back((float)YPlaneHit.PeakTime()-YPlaneHit.StartTick());
	      UpperHitTimeErr.push_back(YPlaneHit.EndTick()-(float)YPlaneHit.PeakTime());
	  }
	  else
	  {
	      HitWireNumberSel.push_back((float)YPlaneHit.WireID().Wire);
	      HitTimeBinSel.push_back((float)YPlaneHit.PeakTime());
	      LowerHitTimeErrSel.push_back((float)YPlaneHit.PeakTime()-YPlaneHit.StartTick());
	      UpperHitTimeErrSel.push_back(YPlaneHit.EndTick()-(float)YPlaneHit.PeakTime());
	  }
      }
    }// end loop over y-plane hits entries
    
    if(HitWireNumber.size())
    {
	fYCanvas->cd();
	fYPlaneHits = FillGraph(HitWireNumber,HitTimeBin,LowerHitTimeErr,UpperHitTimeErr);
	fYPlaneTot->Add(fYPlaneHits);
	
    }
    if(HitWireNumberSel.size())
    {
	fYPlaneHitsSelected = FillGraph(HitWireNumberSel,HitTimeBinSel,LowerHitTimeErrSel,UpperHitTimeErrSel);
	fYPlaneHitsSelected->SetLineColor(2);
	fYPlaneTot->Add(fYPlaneHitsSelected);
	
	fYLaserBeam = new TLine(ROI.GetEntryWire(2),ROI.GetEntryTimeTick(2),ROI.GetExitWire(2),ROI.GetExitTimeTick(2));
	fYLaserBeam->SetLineColor(3);
    }
    
    while(fDrawHits)
    {
      if(!gROOT->IsBatch())
      {
	fUCanvas->cd();
	if(HitWireNumber.size())
	{
	    fUPlaneTot->Draw("AP");
	    fULaserBeam->Draw("same");
	}
	fUCanvas->Modified();
	fUCanvas->Update();
	
	fVCanvas->cd();
	if(HitWireNumber.size()) 
	{
	    fVPlaneTot->Draw("AP");
	    fVLaserBeam->Draw("same");
	}
	fVCanvas->Modified();
	fVCanvas->Update();
	
	fYCanvas->cd();
	if(HitWireNumber.size()) 
	{
	    fYPlaneTot->Draw("AP");
	    fYLaserBeam->Draw("same");
	}
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
