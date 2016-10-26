#ifndef LaserHitFinderTest_Module
#define LaserHitFinderTest_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"

#include "LaserObjects/LaserUtils.h"
#include "LaserObjects/LaserHits.h"
#include "LaserObjects/LaserParameters.h"

#include <assert.h>

namespace LaserHitFinderTest {

    class LaserHitFinderTest : public art::EDAnalyzer {

    public:
        explicit LaserHitFinderTest(fhicl::ParameterSet const& pset);
        virtual ~LaserHitFinderTest();

        virtual void analyze(const art::Event& event) override;

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void reconfigure(fhicl::ParameterSet const &p) override;

    private:
        // The parameters we'll read from the .fcl file.
        lasercal::LaserRecoParameters fParameterSet; ///< ficl parameter structure


    protected:
    };

    LaserHitFinderTest::LaserHitFinderTest(fhicl::ParameterSet const& pset) : EDAnalyzer(pset) {
            this->reconfigure(pset);
    }

    LaserHitFinderTest::~LaserHitFinderTest() {
    }

    void LaserHitFinderTest::reconfigure(fhicl::ParameterSet const &parameterSet) {
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

    void LaserHitFinderTest::beginJob() {
    }

    void LaserHitFinderTest::endJob() {
    }

    void LaserHitFinderTest::analyze(const art::Event& evt) {

        art::InputTag rawtag("daq");
        art::InputTag lasertag("LaserDataMerger", "LaserBeam");

        lasercal::LaserRecoParameters params = fParameterSet;
        params.RawDigitTag = rawtag;
        params.UseROI = true;
        params.MinAllowedChanStatus = 4;

        art::ValidHandle<std::vector<raw::RawDigit>> DigitVecHandle = evt.getValidHandle<std::vector<raw::RawDigit>>(params.RawDigitTag);
        //art::ValidHandle <lasercal::LaserBeam> LaserBeamHandle = evt.getValidHandle<lasercal::LaserBeam>(lasertag) ;

        auto wires = lasercal::GetWires(DigitVecHandle, params);

        auto laser_roi = lasercal::LaserROI();;

        int BoxTickCenter = 5063;
        int BoxTickWidth = 200;
        unsigned int Plane = 2;
        std::pair<unsigned int, unsigned int> Wires(3400, 3500);

        laser_roi.setRanges(BoxTickCenter, BoxTickWidth, Plane, Wires);

        auto hits = lasercal::LaserHits(wires, params, laser_roi);

        auto YHits = hits.GetPlaneHits(2);

        for(auto const & YPlaneHit : *YHits){
            std::cout << "W: " << YPlaneHit.WireID().Wire <<  " PT: " << YPlaneHit.PeakTime() << " PH: " << YPlaneHit.PeakAmplitude() << std::endl;
        }
        assert(wires.size() == 7426);

    }

    DEFINE_ART_MODULE(LaserHitFinderTest)
}

#endif //LaserUtils_test