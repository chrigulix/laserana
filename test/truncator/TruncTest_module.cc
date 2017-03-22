////////////////////////////////////////////////////////////////////////
// Class:       TruncTest
// Plugin Type: analyzer (art v2_05_00)
// File:        TruncTest_module.cc
//
// Generated at Wed Mar 22 09:07:42 2017 by devlaser using cetskelgen
// from cetlib version v1_21_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RawData/RawDigit.h"

#include "larcore/Geometry/Geometry.h"

class TruncTest;


class TruncTest : public art::EDAnalyzer {
public:
    explicit TruncTest(fhicl::ParameterSet const &p);
    // The compiler-generated destructor is fine for non-base
    // classes without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    TruncTest(TruncTest const &) = delete;

    TruncTest(TruncTest &&) = delete;

    TruncTest &operator=(TruncTest const &) = delete;

    TruncTest &operator=(TruncTest &&) = delete;

    // Required functions.
    void analyze(art::Event const &e) override;

    void reconfigure(fhicl::ParameterSet const &pset) override;

private:
    std::string fRawDigitLabel;
    std::string fRawDigitModul;
    std::string fTestConfigFile;

    geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider

};


TruncTest::TruncTest(fhicl::ParameterSet const &p)
        :
        EDAnalyzer(p) {
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    this->reconfigure(p);
}

void TruncTest::analyze(art::Event const &event) {
    auto id = event.id().event();
    auto DigitTag = art::InputTag(fRawDigitModul, fRawDigitLabel);

    art::ValidHandle<std::vector<raw::RawDigit> > RawDigits = event.getValidHandle<std::vector<raw::RawDigit>>(DigitTag);
    if (fTestConfigFile.compare("NoTrunc.txt") == 0) {
        std::cout << "==> Testing no trucation" << std::endl;
        for (auto const &RawDigit : *RawDigits) {
            assert(id == 0);
            assert(RawDigit.NADC() == 9600);
            auto max = std::max_element(RawDigit.ADCs().begin(), RawDigit.ADCs().end());
            auto max_idx = std::distance(RawDigit.ADCs().begin(), max);
            std::cout << "max at: " <<max_idx << std::endl;
            assert(max_idx == 601);

        }
    }
    if (fTestConfigFile.compare("OnlyFront.txt") == 0) {
        std::cout << "==> Testing front truncation" << std::endl;
        for (auto const &RawDigit : *RawDigits) {
            assert(id == 0);
            assert(RawDigit.NADC() == 8000);
            auto max = std::max_element(RawDigit.ADCs().begin(), RawDigit.ADCs().end());
            auto max_idx = std::distance(RawDigit.ADCs().begin(), max);
            assert(max_idx == 1000);
        }
    }
    if (fTestConfigFile.compare("FullTrunc.txt") == 0) {
        std::cout << "==> Testing front truncation with fixed window" << std::endl;
        for (auto const &RawDigit : *RawDigits) {
            assert(id == 0);
            assert(RawDigit.NADC() == 1000);
            auto max = std::max_element(RawDigit.ADCs().begin(), RawDigit.ADCs().end());
            auto max_idx = std::distance(RawDigit.ADCs().begin(), max);
            assert(max_idx == 400);

        }
    }
    if (fTestConfigFile.compare("MultiWires.txt") == 0) {
        std::cout << "==> Testing trucation on multiple wires" << std::endl;
        unsigned int idx = 0;
        for (auto const &RawDigit : *RawDigits) {
            auto wire_id = fGeometry->ChannelToWire(RawDigit.Channel()).front();
            int wire = wire_id.Wire;

            auto max = std::max_element(RawDigit.ADCs().begin(), RawDigit.ADCs().end());
            int max_idx = std::distance(RawDigit.ADCs().begin(), max);

            assert(id == 0);
            assert(wire_id.Plane == 2);
            assert(RawDigit.NADC() == 1000);

            assert(500 + (wire - 1) * 10 == max_idx);
            idx++;
        }
        assert(idx == 10);
    }

}

void TruncTest::reconfigure(fhicl::ParameterSet const &pset) {
    fRawDigitModul = pset.get<std::string>("RawDigitModul");
    fRawDigitLabel = pset.get<std::string>("RawDigitLabel");

    fTestConfigFile = pset.get<std::string>("TestConfigFile");
    // Implementation of optional member function here.
}

DEFINE_ART_MODULE(TruncTest)
