////////////////////////////////////////////////////////////////////////
// Class:       Trunc
// Plugin Type: producer (art v2_05_00)
// File:        Trunc_module.cc
//
// Generated at Tue Mar 21 18:57:58 2017 by devlaser using cetskelgen
// from cetlib version v1_21_00.
////////////////////////////////////////////////////////////////////////

#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"

#include "larcore/Geometry/Geometry.h"

class Trunc;


class Trunc : public art::EDProducer {
public:
    explicit Trunc(fhicl::ParameterSet const &p);
    // The compiler-generated destructor is fine for non-base
    // classes without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    Trunc(Trunc const &) = delete;

    Trunc(Trunc &&) = delete;

    Trunc &operator=(Trunc const &) = delete;

    Trunc &operator=(Trunc &&) = delete;

    // Required functions.
    virtual void produce(art::Event &e) override;

    virtual void reconfigure(fhicl::ParameterSet const &p) override;

private:
    std::string fDigitModuleLabel;      ///<
    std::string fRawDigitLabel;         ///<
    unsigned int fNumTicksToDropFront;   ///< # ticks to drop from front of waveform
    unsigned int fWindowSize;           ///< number of samples to keep after the dropped samples. If 0 all samples will be kept

    geo::GeometryCore const *fGeometry;             ///< pointer to Geometry service
    detinfo::DetectorProperties const *fDetectorProperties;   ///< Detector properties service

};

Trunc::Trunc(fhicl::ParameterSet const &pset) {
    fGeometry = lar::providerFrom<geo::Geometry>();

    fDetectorProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();
    this->reconfigure(pset);
    produces<std::vector<raw::RawDigit> >();
}

void Trunc::reconfigure(fhicl::ParameterSet const &pset) {
    fDigitModuleLabel = pset.get<std::string>("DigitModuleLabel", "daq");
    fRawDigitLabel = pset.get<std::string>("RawDigitLabel", "");
    fNumTicksToDropFront = pset.get<size_t>("NumTicksToDropFront", 0);
    fWindowSize = pset.get<size_t>("WindowSize", 0);
}

void Trunc::produce(art::Event &event) {

    auto DigitTag = art::InputTag(fDigitModuleLabel, fRawDigitLabel);

    std::unique_ptr<std::vector<raw::RawDigit> > filteredRawDigit(new std::vector<raw::RawDigit>);
    art::Handle<std::vector<raw::RawDigit> > digitVecHandle;
    event.getByLabel(DigitTag, digitVecHandle);

    unsigned int maxTimeSamples = fDetectorProperties->NumberTimeSamples();

    // If the window size is specified to be 0 (default value) we keep all values
    if (fWindowSize == 0) {
        fWindowSize = maxTimeSamples - fNumTicksToDropFront;
    }
    raw::RawDigit::ADCvector_t tempVec;
    raw::RawDigit::ADCvector_t trunc_adc;
    trunc_adc.resize(fWindowSize);

    for (const auto &rawDigit : *digitVecHandle) {

        raw::ChannelID_t channel = rawDigit.Channel();
        unsigned int samples = rawDigit.Samples();
        tempVec.resize(samples);
        raw::Uncompress(rawDigit.ADCs(), tempVec, rawDigit.Compression());

        // this is doing the actual truncation by selecting samples out of the full adc vector
        std::copy(tempVec.begin() + fNumTicksToDropFront, tempVec.begin() + fNumTicksToDropFront + fWindowSize ,
                  trunc_adc.begin());

        filteredRawDigit->emplace_back(std::move(raw::RawDigit(channel, trunc_adc.size(), trunc_adc, raw::kNone)));
    }
    event.put(std::move(filteredRawDigit));
}

DEFINE_ART_MODULE(Trunc)
