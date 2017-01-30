#ifndef LaserSpotter_Module
#define LaserSpotter_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"

#include "lardataobj/RawData/RawDigit.h""
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
        lasercal::LaserRecoParameters fParameterSet; ///< ficl parameter structure
        std::vector<int> fCenterTicks, fTickWidths;
        std::vector< std::pair < unsigned int, unsigned int> >fWireBoxes;

        unsigned int fMinHits;

    protected:
    };

    LaserSpotter::LaserSpotter(fhicl::ParameterSet const &pset) {
        this->reconfigure(pset);
    }

    LaserSpotter::~LaserSpotter() {
    }

    void LaserSpotter::reconfigure(fhicl::ParameterSet const &pset) {
        // --------------------------------------------- Filter Parameters ---------------------------------------------- //

        fCenterTicks = pset.get<std::vector<int> >("CenterTicks");
        fTickWidths = pset.get<std::vector<int> >("TickWidths");
        fWireBoxes  = pset.get<std::vector<std::pair<unsigned int, unsigned int>>>("WireBoxes");
        fTickWidths = pset.get<std::vector<int> >("TickWidths");
        fMinHits = pset.get<int> ("MinHits");

        // --------------------------------------------- Hit Finder Parameters ---------------------------------------------- //
        fParameterSet.WireMapGenerator = pset.get<bool>("GenerateWireMap");
        fParameterSet.UseROI = pset.get<bool>("GenerateWireMap");
        fParameterSet.HitBoxSize = pset.get<float>("HitBoxSize");
        // Tag for reading raw digit data
        fParameterSet.RawDigitTag = pset.get<art::InputTag>("LaserRecoModuleLabel");

        // Label for Laser beam data
        fParameterSet.LaserDataMergerModuleLabel = pset.get<std::string>("LaserDataMergerModuleLabel");
        fParameterSet.LaserBeamInstanceLabel = pset.get<std::string>("LaserBeamInstanceLabel");

        // Wire status tag
        fParameterSet.MinAllowedChanStatus = pset.get<int>("MinAllowedChannelStatus");

        // Common hit finder threshold
        fParameterSet.HighAmplitudeThreshold = pset.get<float>("HighAmplThreshold");

        // U-Plane hit finder thresholds
        fParameterSet.UHitThreshold = pset.get<float>("UHitPeakThreshold");
        fParameterSet.UAmplitudeToWidthRatio = pset.get<float>("UAmplitudeToWidthRatio");
        fParameterSet.UHitWidthThreshold = pset.get<int>("UHitWidthThreshold");

        // V-Plane hit finder thresholds
        fParameterSet.VHitThreshold = pset.get<float>("VHitPeakThreshold");
        fParameterSet.VAmplitudeToWidthRatio = pset.get<float>("VAmplitudeToWidthRatio");
        fParameterSet.VAmplitudeToRMSRatio = pset.get<float>("VAmplitudeToRMSRatio");
        fParameterSet.VHitWidthThreshold = pset.get<int>("VHitWidthThreshold");
        fParameterSet.VRMSThreshold = pset.get<int>("VRMSThreshold");

        // Y-Plane hit finder thresholds
        fParameterSet.YHitThreshold = pset.get<float>("YHitPeakThreshold");
        fParameterSet.YAmplitudeToWidthRatio = pset.get<float>("YAmplitudeToWidthRatio");
        fParameterSet.YHitWidthThreshold = pset.get<int>("YHitWidthThreshold");

        // Configurations

    }

    void LaserSpotter::beginJob() {
    }

    void LaserSpotter::endJob() {
    }

    bool LaserSpotter::filter(art::Event &evt) {


        // Get the necessary products
        art::ValidHandle <std::vector<raw::RawDigit>> DigitVecHandle = evt.getValidHandle<std::vector<raw::RawDigit>>(fParameterSet.RawDigitTag);
        art::ValidHandle <lasercal::LaserBeam> LaserBeamHandle = evt.getValidHandle<lasercal::LaserBeam>(fParameterSet.GetLaserBeamTag());

        int id = int(evt.id().event());
        int run = int(evt.id().run());

        // load appropriate windows according to laser
        int CenterTick = -9999;
        int TickWidth = -9999;

        fParameterSet.UseROI = true;
        fParameterSet.MinAllowedChanStatus = 4;
        std::pair<unsigned int, unsigned int> WireRange;

        auto laserid = LaserBeamHandle->GetLaserID();
        int Plane = 2;
        if (0 < laserid < 2) {
            CenterTick = fCenterTicks.at(laserid - 1);
            TickWidth = fTickWidths.at(laserid - 1);
            WireRange.first = fWireBoxes.at(laserid-1).first;
            WireRange.second = fWireBoxes.at(laserid-1).second;

            //std::cout << "Box Definitions for LCS" << laserid << std::endl;;
            //std::cout << "Wires range: [" << WireRange.first << ", " << WireRange.second << "]" << std::endl;
            //std::cout << "Tick range:  [" << CenterTick - TickWidth/2 << ", " << CenterTick + TickWidth/2 << "]" << std::endl;

        } else {
            // TODO: Emmit proper error message within the art framework
            std::cout << "laser ID not recoginized" << std::endl;
            exit(-1);
        }

        auto wires = lasercal::GetWires(DigitVecHandle, fParameterSet);

        auto laser_roi = lasercal::LaserROI();
        laser_roi.setRanges(CenterTick, TickWidth, Plane, WireRange);

        auto hits = lasercal::LaserHits(wires, fParameterSet, laser_roi);

        auto YHits = hits.GetPlaneHits(Plane);

        bool above_threshold = hits.NumberOfWiresWithHits().at(Plane) >= fMinHits;
        std::cout << "- id: " << id << std::endl;
        std::cout << "- NHits; " << hits.NumberOfWiresWithHits().at(Plane) << std::endl;
        if (run == 3165) {
            switch (id) {
                case 50:
                    assert(above_threshold);
                    break;
                case 51:
                    assert(above_threshold);
                    break;
                case 251:
                    assert(above_threshold);
                    break;
                case 257:
                    assert(above_threshold);
                    break;
                case 516:
                    assert(above_threshold);
                    break;
                case 517:
                    assert(above_threshold);
                    break;
                case 518:
                    assert(!above_threshold);
                    break;
                case 519:
                    assert(above_threshold);
                    break;
                case 521:
                    assert(above_threshold);
                    break;
            }
        }
        else if (run == 3007){
            switch(id) {
                case 104:
                    assert(above_threshold);
                    break;
                case 111:
                    assert(!above_threshold);
                    break;
                case 132:
                    assert(above_threshold);
                    break;
                case 133:
                    assert(above_threshold);
                    break;
                case 151:
                    assert(above_threshold);
                    break;
                case 154:
                    assert(above_threshold);
                    break;
            }
        }
        else{
            assert(false);
        }
        return true;
    }

    DEFINE_ART_MODULE(LaserSpotter)
}

#endif //LaserSpotter_Module