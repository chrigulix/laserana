// LaserReco_module.cc

#ifndef LaserReco_Module
#define LaserReco_Module

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
#include "LaserObjects/LaserBeam.h"
#include "LaserObjects/LaserParameters.h"

namespace {

} // local namespace


namespace LaserReco {

    class LaserReco : public art::EDProducer {
    public:


        /// Default constructor
        explicit LaserReco(fhicl::ParameterSet const &parameterSet);

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void reconfigure(fhicl::ParameterSet const &parameterSet) override;

        // The analysis routine, called once per event.
        virtual void produce(art::Event &event) override;

        // Wire crossing finder
        std::vector<std::pair<geo::WireID, geo::WireID> > CrossingWireRanges(geo::WireID WireID);

        void CutRegionOfInterest();

    private:

        // The parameters we'll read from the .fcl file.
        lasercal::LaserRecoParameters fParameterSet; ///< ficl parameter structure

        // Other variables that will be shared between different methods.
        geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider

        detinfo::DetectorProperties const *fDetProperties;  ///< pointer to detector properties provider

        std::string fFileName = "WireIndexMap.root";

        std::vector<std::map<unsigned int, unsigned int> > WireMaps;

        unsigned short fLCSNumber;

    }; // class LaserReco

    DEFINE_ART_MODULE(LaserReco)

    // Constructor
    LaserReco::LaserReco(fhicl::ParameterSet const &pset) {
        // get a pointer to the geometry service provider
        fGeometry = &*(art::ServiceHandle<geo::Geometry>());
        fDetProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();

        // Read in the parameters from the .fcl file.
        this->reconfigure(pset);

//     produces< std::vector<recob::Wire> >("blibla");
        produces<std::vector<recob::Hit> >("UPlaneLaserHits");
        produces<std::vector<recob::Hit> >("VPlaneLaserHits");
        produces<std::vector<recob::Hit> >("YPlaneLaserHits");
    }


    //-----------------------------------------------------------------------
    void LaserReco::beginJob() {
        if (!fParameterSet.WireMapGenerator) {
            TFile *InputFile = new TFile(fFileName.c_str(), "READ");

            std::map<unsigned int, unsigned int> *pUMap;
            std::map<unsigned int, unsigned int> *pVMap;
            std::map<unsigned int, unsigned int> *pYMap;

            InputFile->GetObject("UMap", pUMap);
            InputFile->GetObject("VMap", pVMap);
            InputFile->GetObject("YMap", pYMap);

            WireMaps.push_back(std::move(*pUMap));
            WireMaps.push_back(std::move(*pVMap));
            WireMaps.push_back(std::move(*pYMap));

            delete pUMap;
            delete pVMap;
            delete pYMap;
        }

        // TODO: Change later
        fLCSNumber = 2;

    }


    void LaserReco::endJob() {

    }

//-----------------------------------------------------------------------

    void LaserReco::reconfigure(fhicl::ParameterSet const &parameterSet) {
        //Read ficl parameterSet

        // Switches
        fParameterSet.WireMapGenerator = parameterSet.get<bool>("GenerateWireMap");
        fParameterSet.UseROI = parameterSet.get<bool>("GenerateWireMap");
        fParameterSet.HitBoxSize = parameterSet.get<float>("HitBoxSize");

        // Tag for reading raw digit data
        fParameterSet.RawDigitTag = parameterSet.get<art::InputTag>("LaserRecoModuleLabel");

        // Label for Laser beam data
        fParameterSet.LaserDataMergerModuleLabel = parameterSet.get<std::string>("LaserDataMergerModuleLabel");
        fParameterSet.LaserBeamInstanceLabel = parameterSet.get<std::string>("LaserBeamInstanceLabel");

        // Wire status tag
        fParameterSet.MinAllowedChanStatus = parameterSet.get<int>("MinAllowedChannelStatus");

        // Common hit finder threshold
        fParameterSet.HighAmplitudeThreshold = parameterSet.get<float>("HighAmplThreshold");

        // U-Plane hit finder thresholds
        fParameterSet.UHitThreshold = parameterSet.get<float>("UHitPeakThreshold");
        fParameterSet.UAmplitudeToWidthRatio = parameterSet.get<float>("UAmplitudeToWidthRatio");
        fParameterSet.UHitWidthThreshold = parameterSet.get<int>("UHitWidthThreshold");

        // V-Plane hit finder thresholds
        fParameterSet.VHitThreshold = parameterSet.get<float>("VHitPeakThreshold");
        fParameterSet.VAmplitudeToWidthRatio = parameterSet.get<float>("VAmplitudeToWidthRatio");
        fParameterSet.VAmplitudeToRMSRatio = parameterSet.get<float>("VAmplitudeToRMSRatio");
        fParameterSet.VHitWidthThreshold = parameterSet.get<int>("VHitWidthThreshold");
        fParameterSet.VRMSThreshold = parameterSet.get<int>("VRMSThreshold");

        // Y-Plane hit finder thresholds
        fParameterSet.YHitThreshold = parameterSet.get<float>("YHitPeakThreshold");
        fParameterSet.YAmplitudeToWidthRatio = parameterSet.get<float>("YAmplitudeToWidthRatio");
        fParameterSet.YHitWidthThreshold = parameterSet.get<int>("YHitWidthThreshold");
    }

    //-----------------------------------------------------------------------
    void LaserReco::produce(art::Event &event) {
        // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)
        art::ValidHandle<std::vector<raw::RawDigit> > DigitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>(
                fParameterSet.RawDigitTag);

        art::ValidHandle<lasercal::LaserBeam> LaserBeamHandle = event.getValidHandle<lasercal::LaserBeam>(
                fParameterSet.GetLaserBeamTag());

//     LaserBeamHandle->GetEntryPoint().Print();
//     LaserBeamHandle->GetExitPoint().Print();

//     LaserBeamHandle->GetLaserPosition().Print();
//     LaserBeamHandle->GetLaserDirection().Print();

        // Prepairing the wire signal vector. It will be just the raw signal with subtracted pedestial
        std::vector<recob::Wire> WireVec;

        // Prepairing the hit vectors for all planes
        std::unique_ptr<std::vector<recob::Hit> > UHitVec(new std::vector<recob::Hit>);
        std::unique_ptr<std::vector<recob::Hit> > VHitVec(new std::vector<recob::Hit>);
        std::unique_ptr<std::vector<recob::Hit> > YHitVec(new std::vector<recob::Hit>);

        // Preparing WireID vector
        std::vector<geo::WireID> WireIDs;

        // Reserve space for wire vector
        WireVec.reserve(DigitVecHandle->size());

        // Get Service providers
        const lariov::DetPedestalProvider &PedestalRetrievalAlg = art::ServiceHandle<lariov::DetPedestalService>()->GetPedestalProvider();
        const lariov::ChannelStatusProvider &ChannelFilter = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();

        // Initialize raw time tick vectors
        std::vector<short> RawADC;
        std::vector<float> RawROI;
        RawADC.resize(DigitVecHandle->at(0).Samples());
        RawROI.resize(DigitVecHandle->at(0).Samples());

        // Prepare laser hits object
//     lasercal::LaserHits YROIHits(fUVYThresholds);

        // Set region of interest limits for first hit scan
        unsigned int StartROI = 4500;
        unsigned int EndROI = 5500;

        // Initialize Regions of interest
        recob::Wire::RegionsOfInterest_t RegionOfInterestFirst;
        recob::Wire::RegionsOfInterest_t RegionOfInterest;

        // Loop over downstream laser system
//     if(fLCSNumber == 2)
//     { 
//       // Loop over N collection wires at the edge of the TPC based on the wire map
//       for(unsigned int WireIndex = WireMaps.back().size()-1; WireIndex >= WireMaps.back().size() - 13; WireIndex--)
//       {
//         // Get the raw data for this particular wire
//         auto RawDigit = DigitVecHandle->at(WireMaps.back().at(WireIndex));
// 	
// 	// Get Channel number
// 	raw::ChannelID_t channel = RawDigit.Channel();
// 	
// 	// Skip channel if dead or noisy
// 	if( ChannelFilter.Status(channel) < fParameterSet.MinAllowedChanStatus || !ChannelFilter.IsPresent(channel) )
// 	{
// 	  continue;
// 	}
// 	
// 	// Uncompress raw digits into RawADC vector
// 	raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());
//         
//         // copy only the region of interest into the the RawROI vector TODO Watch commented stuff
// 	std::copy(RawADC.begin()+StartROI, RawADC.begin()+EndROI, RawROI.begin());
// 
//         // Subtract pedestal from samples in ROI
// 	for(auto & RawSample : RawROI)
// 	{
// 	  RawSample -= PedestalRetrievalAlg.PedMean(channel);
// 	}
// 	
//         // Create a wire from the selected ROI (1. copy raw into ROI container, 2. copy the ROI vector into a wire container)
//         RegionOfInterestFirst.add_range(StartROI,RawROI.begin(),RawROI.end());
// 	
// 	// Search for hits with the created wire
// 	YROIHits.AddHitsFromWire(recob::WireCreator( std::move(RegionOfInterestFirst), RawDigit ).move());
// 	
//       }// end for over endge collection wires
//       
//       // Check if there are more than x hits in the selected area and jump event if there are less
//       if( YROIHits.NumberOfWiresWithHits().at(2) < 7 ) 
//       {
// 	event.put(std::move(UHitVec), "UPlaneLaserHits");
// 	event.put(std::move(VHitVec), "VPlaneLaserHits");
// 	event.put(std::move(YHitVec), "YPlaneLaserHits");
// 	return;
//       }
//     }// end if laser system number two


        // Loop over all raw digit entries
        for (auto const &RawDigit : *DigitVecHandle) {
            // Get channel ID
            raw::ChannelID_t channel = RawDigit.Channel();

            // Skip channel if dead or noisy
            if (ChannelFilter.Status(channel) < fParameterSet.MinAllowedChanStatus ||
                !ChannelFilter.IsPresent(channel)) {
                continue;// jump to next iterator in RawDigit loop
            }

            // Extract data into RawADC vector and uncompress it
            raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());

            // Copy the Raw ADC digit (short) into the signal vector (float)
            std::copy(RawADC.begin(), RawADC.end(), RawROI.begin());

            // subtract pedestial
            for (auto &RawSample : RawROI) {
                RawSample -= PedestalRetrievalAlg.PedMean(channel);
            }

            // Create the region of interest (in this case the whole wire)
            RegionOfInterest.add_range(0, RawROI.begin(), RawROI.end());

            // Create a Wire object with the raw signal
            WireVec.emplace_back(recob::WireCreator(std::move(RegionOfInterest), RawDigit).move());

        } // end loop over raw digit entries

        // Create Laser Hits out of Wires
        lasercal::LaserHits AllLaserHits(WireVec, fParameterSet, *LaserBeamHandle);

        // Filter for time matches of at least two planes
//     AllLaserHits.TimeMatchFilter();

        // Fill plane specific hit vectors
        UHitVec = AllLaserHits.GetPlaneHits(0);
        VHitVec = AllLaserHits.GetPlaneHits(1);
        YHitVec = AllLaserHits.GetPlaneHits(2);

//     std::cout << fDetProperties->ConvertXToTicks(100,2,0,0) << std::endl;

//     std::cout << UHitVec->size() << std::endl;
//     std::cout << VHitVec->size() << std::endl;
//     std::cout << YHitVec->size() << std::endl;

        // Fill Hits of all planes into the new
        event.put(std::move(UHitVec), "UPlaneLaserHits");
        event.put(std::move(VHitVec), "VPlaneLaserHits");
        event.put(std::move(YHitVec), "YPlaneLaserHits");
    } // LaserReco::analyze()

    // Gives out a vector of WireIDs which cross a certain input wire
    std::vector<std::pair<geo::WireID, geo::WireID> > LaserReco::CrossingWireRanges(geo::WireID WireID) {
        // Initialize the return pair vector
        std::vector<std::pair<geo::WireID, geo::WireID> > CrossingWireRangeVec;

        // Initialize start and end point of wire
        double Start[3], End[3];

        // Fill the wire End points
        fGeometry->WireEndPoints(WireID, Start, End);

        // Loop over target plane number
        for (unsigned int plane_no = 0; plane_no < fGeometry->Nplanes(); plane_no++) {
            // Search for crossing wires only if the aren't on the same plane
            if (plane_no != WireID.Plane) {
                // Generate PlaneID for microboone (cryostatID = 0, TPCID = 0, plane number)
                auto TargetPlaneID = geo::PlaneID(0, 0, plane_no);

                // Get the closest wire on the target plane to the wire start position
                auto FirstWireID = fGeometry->NearestWireID(Start, TargetPlaneID);

                // Get the closest wire on the target plane to the wire end postion
                auto LastWireID = fGeometry->NearestWireID(End, TargetPlaneID);

                // Check if wires are in right order
                if (FirstWireID.Wire > LastWireID.Wire) {
                    // Swap them if order is decending
                    std::swap(FirstWireID, LastWireID);
                }

                // Generate a dummy intersection coordinate, because shit only works that way (eye roll)
                geo::WireIDIntersection DummyMcDumbFace;

                // Check if first and last wires are crossing, if not take one next to them
                if (!fGeometry->WireIDsIntersect(WireID, FirstWireID, DummyMcDumbFace)) {
                    ++(FirstWireID.Wire);
                }
                if (!fGeometry->WireIDsIntersect(WireID, LastWireID, DummyMcDumbFace)) {
                    --(LastWireID.Wire);
                }
                CrossingWireRangeVec.push_back(std::make_pair(FirstWireID, LastWireID));
            }
        }
        return CrossingWireRangeVec;
    }

} // namespace LaserReco

#endif // LaserReco_Module
