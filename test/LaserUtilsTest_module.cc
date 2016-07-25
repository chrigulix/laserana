#ifndef LaserUtilsTest_Module
#define LaserUtilsTest_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"

#include "LaserObjects/LaserUtils.h"

#include <assert.h>

namespace LaserUtilsTest {

    class LaserUtilsTest : public art::EDAnalyzer {

    public:
        explicit LaserUtilsTest(fhicl::ParameterSet const& pset);
        virtual ~LaserUtilsTest();

        virtual void analyze(const art::Event& event) override;

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void reconfigure(fhicl::ParameterSet const &p) override;

    private:


    protected:
    };

    LaserUtilsTest::LaserUtilsTest(fhicl::ParameterSet const& pset) : EDAnalyzer(pset) {
            this->reconfigure(pset);
    }

    LaserUtilsTest::~LaserUtilsTest() {
    }

    void LaserUtilsTest::reconfigure(fhicl::ParameterSet const &pset) {
        // fInputParameter = pset.get< std::string >("InputParameterName");
    }

    void LaserUtilsTest::beginJob() {
    }

    void LaserUtilsTest::endJob() {
    }

    void LaserUtilsTest::analyze(const art::Event& evt) {

        art::InputTag rawtag("daq");

        lasercal::LaserRecoParameters params;
        params.RawDigitTag = rawtag;
        params.MinAllowedChanStatus = 4;

        art::ValidHandle<std::vector<raw::RawDigit>> DigitVecHandle = evt.getValidHandle<std::vector<raw::RawDigit>>(params.RawDigitTag);

        auto wires = lasercal::GetWires(DigitVecHandle, params);

        assert(wires.size() == 7426);

    }

    DEFINE_ART_MODULE(LaserUtilsTest)
}

#endif //LaserUtils_test