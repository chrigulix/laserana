#ifndef LaserSpotter_Module
#define LaserSpotter_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Wire.h"

#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"

// Laser Module Classes
#include "LaserObjects/LaserBeam.h"
#include "LaserObjects/LaserHits.h"
#include "LaserObjects/LaserUtils.h"
#include "LaserObjects/LaserParameters.h"
#include "LaserObjects/LaserROI.h"


#include <vector>

/*
 *  This filter module acts on merged laser data (laser data merged into swizzled raw files). It looks for tracks in
 *  a predefined area on the collection plane (presumably just in front of the entry point of the laser) using the laser
 *  track finder.
 *
 *  Input parameters from fhicl file (two entries expected for the two laser systems):
 *   EntryTicks  - Center Time Tick of the window where we are going to look for the laser
 *   TickWidths  - Total Width of the tick window
 *   WireWidths  - Number of wires to extend the search into the TPC from the border
 *
 *  Internal functionallity (just for code planning):
 *   1. Get all wires
 *   2. Loop over wires
 *   3. Is wire in region?
 *   4. Is wire in good channels?
 *   5. Look for hits in window
 *   6. If hits present -> pass event or further analysis
 *      If no hits present -> ignore event in future analysis
 *
 */

auto laser_sign = [] (int x) { return -2 * x + 3; }; // no fuking ifs!

namespace LaserSpotter {

    class LaserSpotter : public art::EDFilter {

    public:
        explicit LaserSpotter(fhicl::ParameterSet const &);

        virtual ~LaserSpotter();

        bool filter(art::Event &evt);

        void beginJob();

        void endJob();

        void reconfigure(fhicl::ParameterSet const &p);

    private:
        bool fManualBox;

        // for automatic mode
        uint fTickWidth;
        uint fWireWidth;

        // for manual mode
        lasercal::LaserRecoParameters fParameterSet; ///< ficl parameter structure
        std::vector<int> fCenterTicks, fTickWidths;
        std::vector< std::pair < unsigned int, unsigned int> >fWireBoxes;

        // filter parameters
        unsigned int fMinHits;



        bool fPedestalStubtract;
        bool fDebug;
    protected:
    };

    LaserSpotter::LaserSpotter(fhicl::ParameterSet const &pset) {
        this->reconfigure(pset);
    }

    LaserSpotter::~LaserSpotter() {
    }

    void LaserSpotter::reconfigure(fhicl::ParameterSet const &pset) {
        // --------------------------------------------- Filter Parameters ---------------------------------------------- //

        // get nested psets:
        fhicl::ParameterSet pset_io = pset.get<fhicl::ParameterSet>("io");
        fhicl::ParameterSet pset_box = pset.get<fhicl::ParameterSet>("spotter");
        fhicl::ParameterSet pset_hitfinder = pset.get<fhicl::ParameterSet>("hitfinder");


        fPedestalStubtract = pset.get<bool> ("PedestalSubtract", true);

        // --------------------------------------------- Spotter Parameters ---------------------------------------------- //
        fDebug =        pset_box.get<uint>("Debug", false);

        fTickWidth =    pset_box.get<uint>("TickWidth", 100);
        fWireWidth =    pset_box.get<uint>("WireWidth", 100);
        fMinHits =      pset_box.get<uint>("MinHits", 10);
        fManualBox =    pset_box.get<bool>("ManualBox", false);

        // in manual mode
        fCenterTicks =  pset_box.get<std::vector<int> >("CenterTicks", std::vector<int> ());
        fTickWidths =   pset_box.get<std::vector<int> >("TickWidths", std::vector<int> ());
        fWireBoxes  =   pset_box.get<std::vector<std::pair<unsigned int, unsigned int>>>("WireBoxes", std::vector<std::pair<uint, uint>> ());

        // --------------------------------------------- File Handling Parameters --------------------------------------- //
        // Tag for reading raw digit data
        fParameterSet.RawDigitTag =                 pset_io.get<art::InputTag>("LaserRecoModuleLabel");
        // Label for Laser beam data
        fParameterSet.LaserDataMergerModuleLabel =  pset_io.get<std::string>("LaserDataMergerModuleLabel");
        fParameterSet.LaserBeamInstanceLabel =      pset_io.get<std::string>("LaserBeamInstanceLabel");

        // --------------------------------------------- Hit Finder Parameters ------------------------------------------ //
        fParameterSet.WireMapGenerator =    pset_hitfinder.get<bool>("GenerateWireMap");
        fParameterSet.UseROI =              pset_hitfinder.get<bool>("GenerateWireMap");
        fParameterSet.HitBoxSize =          pset_hitfinder.get<float>("HitBoxSize");

        // Wire status tag
        fParameterSet.MinAllowedChanStatus = pset_hitfinder.get<int>("MinAllowedChannelStatus");

        // Common hit finder threshold
        fParameterSet.HighAmplitudeThreshold = pset_hitfinder.get<float>("HighAmplThreshold");

        // U-Plane hit finder thresholds
        fParameterSet.UHitThreshold = pset_hitfinder.get<float>("UHitPeakThreshold");
        fParameterSet.UAmplitudeToWidthRatio = pset_hitfinder.get<float>("UAmplitudeToWidthRatio");
        fParameterSet.UHitWidthThreshold = pset_hitfinder.get<int>("UHitWidthThreshold");

        // V-Plane hit finder thresholds
        fParameterSet.VHitThreshold = pset_hitfinder.get<float>("VHitPeakThreshold");
        fParameterSet.VAmplitudeToWidthRatio = pset_hitfinder.get<float>("VAmplitudeToWidthRatio");
        fParameterSet.VAmplitudeToRMSRatio = pset_hitfinder.get<float>("VAmplitudeToRMSRatio");
        fParameterSet.VHitWidthThreshold = pset_hitfinder.get<int>("VHitWidthThreshold");
        fParameterSet.VRMSThreshold = pset_hitfinder.get<int>("VRMSThreshold");

        // Y-Plane hit finder thresholds
        fParameterSet.YHitThreshold = pset_hitfinder.get<float>("YHitPeakThreshold");
        fParameterSet.YAmplitudeToWidthRatio = pset_hitfinder.get<float>("YAmplitudeToWidthRatio");
        fParameterSet.YHitWidthThreshold = pset_hitfinder.get<int>("YHitWidthThreshold");

    }

    void LaserSpotter::beginJob() {
    }

    void LaserSpotter::endJob() {
    }

    bool LaserSpotter::filter(art::Event &evt) {


        // Get the necessary products
        art::ValidHandle <std::vector<raw::RawDigit>> DigitVecHandle = evt.getValidHandle<std::vector<raw::RawDigit>>(fParameterSet.RawDigitTag);
        art::ValidHandle <lasercal::LaserBeam> LaserBeam = evt.getValidHandle<lasercal::LaserBeam>(fParameterSet.GetLaserBeamTag());

        //TODO: Implement adjustements of box due to drift field

        // load appropriate windows according to laser
        int CenterTick = -9999;
        int TickWidth = -9999;

        fParameterSet.UseROI = true;
        std::pair<unsigned int, unsigned int> WireRange;

        auto laserid = LaserBeam->GetLaserID();
        int Plane = 2;


        if (fManualBox) {
            if (0 < laserid && laserid < 2) {
                CenterTick = fCenterTicks.at(laserid - 1);
                TickWidth = fTickWidths.at(laserid - 1);
                WireRange.first = fWireBoxes.at(laserid - 1).first;
                WireRange.second = fWireBoxes.at(laserid - 1).second;
            } else {
                // TODO: Emmit proper error message within the art framework
                std::cout << "laser ID not recoginized" << std::endl;
                exit(-1);
            }
        }
        else {
            lasercal::LaserBeam DummyLaserBeam(LaserBeam->GetLaserPosition(), TVector3(0,0,1));
            CenterTick = DummyLaserBeam.getEntryTick();
            TickWidth = fTickWidth;
            WireRange.first = DummyLaserBeam.getEntryWire().at(Plane).Wire;
            WireRange.second = DummyLaserBeam.getEntryWire().at(Plane).Wire + laser_sign(laserid) * fWireWidth;
        }

        if (fDebug) {
            std::cout << "Box Definitions for LCS" << laserid << std::endl;;
            std::cout << "Wires range: [" << WireRange.first << ", " << WireRange.second << "]" << std::endl;
            std::cout << "Tick range:  [" << CenterTick - TickWidth/2 << ", " << CenterTick + TickWidth/2 << "]" << std::endl;
        }


        auto wires = lasercal::GetWires(DigitVecHandle, fParameterSet, fPedestalStubtract);

        auto laser_roi = lasercal::LaserROI();
        laser_roi.setRanges(CenterTick, TickWidth, Plane, WireRange);

        auto hits = lasercal::LaserHits(wires, fParameterSet, laser_roi);

        auto YHits = hits.GetPlaneHits(Plane);
        std::cout << "Number of wires: "<< wires.size() << std::endl;
        std::cout << "Number of hits:  "<< hits.NumberOfWiresWithHits().at(Plane) << std::endl;

        if (hits.NumberOfWiresWithHits().at(Plane) >= fMinHits) {
            std::cout << "True" << std::endl;
            return true;
        }
        else {
            std::cout << "False" << std::endl;
            return false;
        }
    }

    DEFINE_ART_MODULE(LaserSpotter)
}

#endif //LaserSpotter_Module
