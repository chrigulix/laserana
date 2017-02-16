// LaserReco_module.cc

#ifndef LaserReco_Module
#define LaserReco_Module

// LArSoft includes
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"

#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larcore/CoreUtils/ServiceUtil.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"

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
#include "LaserObjects/LaserUtils.h"
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

        bool fPedestalStubtract;
        bool fUseCalData;
        art::InputTag fCalDataTag;

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
        //produces<std::vector<recob::Hit> >("UPlaneLaserHits");
        //produces<std::vector<recob::Hit> >("VPlaneLaserHits");
        //produces<std::vector<recob::Hit> >("YPlaneLaserHits");
        produces<std::vector<recob::Hit> >("");
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

        fPedestalStubtract = parameterSet.get<bool> ("PedestalSubtract", true);
        fUseCalData = parameterSet.get<bool> ("UseCalData", false);
        fCalDataTag = parameterSet.get<art::InputTag>("CalDataTag", "");

        // Switches
        fParameterSet.WireMapGenerator = parameterSet.get<bool>("GenerateWireMap");
        fParameterSet.UseROI = parameterSet.get<bool>("UseROI");
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

        art::ValidHandle<std::vector<raw::RawDigit> > DigitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>(fParameterSet.RawDigitTag);

        art::Handle<std::vector<recob::Wire> > WireVecHandle;

        art::ValidHandle<lasercal::LaserBeam> LaserBeamHandle = event.getValidHandle<lasercal::LaserBeam>(
                fParameterSet.GetLaserBeamTag());

        if (fUseCalData) {

        }

        // Prepairing the hit vectors for all planes
        //std::unique_ptr<std::vector<recob::Hit> > UHitVec(new std::vector<recob::Hit>);
        //std::unique_ptr<std::vector<recob::Hit> > VHitVec(new std::vector<recob::Hit>);
        //std::unique_ptr<std::vector<recob::Hit> > YHitVec(new std::vector<recob::Hit>);

        std::unique_ptr<std::vector<recob::Hit> > HitVec(new std::vector<recob::Hit>);

        // Initialize Regions of interest
        recob::Wire::RegionsOfInterest_t RegionOfInterestFirst;
        recob::Wire::RegionsOfInterest_t RegionOfInterest;

        std::vector<recob::Wire> Wires;

        lasercal::LaserHits AllLaserHits;


        if (fUseCalData){
            event.getByLabel(fCalDataTag, WireVecHandle);
            AllLaserHits = lasercal::LaserHits(WireVecHandle, fParameterSet, *LaserBeamHandle);
        }
        else {
            auto Wires = lasercal::GetWires(DigitVecHandle, fParameterSet, fPedestalStubtract);
            AllLaserHits = lasercal::LaserHits(Wires, fParameterSet, *LaserBeamHandle);
        }
        // Create Laser Hits out of Wires

        // Filter for time matches of at least two planes
//     AllLaserHits.TimeMatchFilter();

        // Fill plane specific hit vectors
        //UHitVec = AllLaserHits.GetPlaneHits(0);
        //VHitVec = AllLaserHits.GetPlaneHits(1);
        //YHitVec = AllLaserHits.GetPlaneHits(2);

        HitVec = AllLaserHits.GetHits();

//     std::cout << fDetProperties->ConvertXToTicks(100,2,0,0) << std::endl;

//     std::cout << UHitVec->size() << std::endl;
//     std::cout << VHitVec->size() << std::endl;
//     std::cout << YHitVec->size() << std::endl;

        // Fill Hits of all planes into the new
        //event.put(std::move(UHitVec), "UPlaneLaserHits");
        //event.put(std::move(VHitVec), "VPlaneLaserHits");
        //event.put(std::move(YHitVec), "YPlaneLaserHits");

        event.put(std::move(HitVec));
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
