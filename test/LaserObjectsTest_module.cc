#ifndef LaserObjectsTest_Module
#define LaserObjectsTest_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"

#include "LaserObjects/LaserUtils.h"
#include "LaserObjects/LaserHits.h"

#include <assert.h>

namespace LaserObjectsTest {

    class LaserObjectsTest : public art::EDAnalyzer {

    public:
        explicit LaserObjectsTest(fhicl::ParameterSet const& pset);
        virtual ~LaserObjectsTest();

        virtual void analyze(const art::Event& event) override;

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void reconfigure(fhicl::ParameterSet const &p) override;

    private:


    protected:
    };

    LaserObjectsTest::LaserObjectsTest(fhicl::ParameterSet const& pset) : EDAnalyzer(pset) {
            this->reconfigure(pset);
    }

    LaserObjectsTest::~LaserObjectsTest() {
    }

    void LaserObjectsTest::reconfigure(fhicl::ParameterSet const &pset) {
        // fInputParameter = pset.get< std::string >("InputParameterName");
    }

    void LaserObjectsTest::beginJob() {
    }

    void LaserObjectsTest::endJob() {
    }

    void LaserObjectsTest::analyze(const art::Event& evt) {

        art::InputTag rawtag("daq");
        art::InputTag lasertag("LaserDataMerger");

        lasercal::LaserRecoParameters params;
        params.RawDigitTag = rawtag;
        params.MinAllowedChanStatus = 4;

        art::ValidHandle<std::vector<raw::RawDigit>> DigitVecHandle = evt.getValidHandle<std::vector<raw::RawDigit>>(params.RawDigitTag);
        //art::ValidHandle <lasercal::LaserBeam> LaserBeamHandle = evt.getValidHandle<lasercal::LaserBeam>() ;
        auto wires = lasercal::GetWires(DigitVecHandle, params);

        //for (auto wire: wires){
        //    std::cout << "bibi: " << wire.Signal().at(0) << std::endl;
        //}
        assert(wires.size() == 7426);

    }

    DEFINE_ART_MODULE(LaserObjectsTest)
}

#endif //LaserUtils_test