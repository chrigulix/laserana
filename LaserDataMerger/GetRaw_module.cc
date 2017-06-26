

#ifndef GetRaw_Module
#define GetRaw_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"

#include "lardataobj/RecoBase/Wire.h"

#include "lardataobj/RecoBase/Hit.h"
#include "larcore/Geometry/Geometry.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"

// uBooNE includes
#include "lardata/Utilities/AssociationUtil.h"

// ROOT includes.
#include "TTree.h"
#include "TFile.h"

#include <fstream>

// Laser Module Classes
#include "LaserObjects/LaserBeam.h"


namespace GetDistortion
{

class GetRaw : public art::EDProducer
{
public:

    explicit GetRaw(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;

    virtual void beginRun(art::Run& run);

    virtual void endJob() override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void produce(art::Event& event) override;
   

private:
    geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider
    bool DEBUG = true;

    TTree* fRawTree;
    TTree* fHitTree;
    TTree* fCalTree;


    // raw digit holders
    std::vector<short> RawDigit;
    unsigned int Channel;
    unsigned int event_id;
    unsigned int wire;
    unsigned int plane;

    // wire holders
    std::vector<float> calwire;
    std::vector<float> roi;

    // holders for the hit information per wire of interest
    std::vector<unsigned int> start_ticks;
    std::vector<unsigned int> end_ticks;
    std::vector<unsigned int> peak_times;
    std::vector<float> peak_amps;
    unsigned int hit_channel;


    art::InputTag fRawLabel;
    art::InputTag fHitLabel;
    art::InputTag fCalLabel;
    std::vector<unsigned int> fChannels;


}; // class GetRaw



GetRaw::GetRaw(fhicl::ParameterSet const& pset)
{
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
}


//-----------------------------------------------------------------------

void GetRaw::beginJob()
{

}

void GetRaw::beginRun(art::Run& run)
{
    art::ServiceHandle<art::TFileService> tfs;

    // Initialize time info root file
    fRawTree = tfs->make<TTree>("RawDigit", "Tracks");
    fRawTree->Branch("event", &event_id);
    fRawTree->Branch("channel", &Channel);
    fRawTree->Branch("wire", &wire);
    fRawTree->Branch("plane", &plane);
    fRawTree->Branch("raw", &RawDigit);

    fHitTree = tfs->make<TTree>("HitTree", "Tracks");
    fHitTree->Branch("start_ticks", &start_ticks);
    fHitTree->Branch("end_ticks", &end_ticks);
    fHitTree->Branch("peak_times", &peak_times);
    fHitTree->Branch("peak_amps", &peak_amps);
    fHitTree->Branch("channel", &hit_channel);

    fCalTree = tfs->make<TTree>("CalTree", "Tracks");
    fCalTree->Branch("clawire", &calwire);

    return;
}

void GetRaw::endJob()
{
}

void GetRaw::reconfigure(fhicl::ParameterSet const& parameterSet)
{
    fRawLabel = parameterSet.get<art::InputTag>("RawLabel", "digitfilter");
    fHitLabel = parameterSet.get<art::InputTag>("HitLabel", "daq");
    fCalLabel = parameterSet.get<art::InputTag>("CalLabel", "caldata");
    fChannels = parameterSet.get<std::vector<unsigned int> >("Channels");
}

//-----------------------------------------------------------------------

void GetRaw::produce(art::Event& event)
{
    art::Handle<std::vector<raw::RawDigit> > Raw;
    art::Handle<std::vector<recob::Hit> > Hits;
    art::Handle<std::vector<recob::Wire> > CalWires;

    event.getByLabel(fRawLabel, Raw);
    event.getByLabel(fHitLabel, Hits);
    event.getByLabel(fCalLabel, CalWires);


    //auto track = tr.fXYZ;
    for (auto const &Digit : *Raw) {
        auto test = raw::RawDigit();
        auto hitt = recob::Hit();
        auto calw = recob::Wire();

        std::vector<recob::Hit> hits;

        hitt.Channel();
        calw.SignalROI();


        Channel = Digit.Channel();

        if(std::find(fChannels.begin(), fChannels.end(), Channel) != fChannels.end()) {

            auto WireID = fGeometry->ChannelToWire(Channel);

            // find all hits in the channel
            for (auto const &Hit : *Hits) {
                if (Hit.Channel() == Channel) {
                    start_ticks.push_back(Hit.StartTick());
                    end_ticks.push_back(Hit.EndTick());
                    peak_times.push_back(Hit.PeakTime());
                    peak_amps.push_back(Hit.PeakAmplitude());
                    hit_channel = Hit.Channel();
                }
            }
            for (auto const &CalWire : *CalWires) {
                if (CalWire.Channel() == Channel) {
                    calwire = CalWire.Signal();
                }
            }

            std::cout << "start: " << start_ticks.front() << std::endl;

            plane = WireID.front().Plane;
            wire = WireID.front().Wire;

            std::cout << "Writing Channel: " << Channel
                      << " Plane/Wire: " <<  plane  << '/' << wire
                      << std::endl;
            event_id = (unsigned int) event.id().event();
            RawDigit = Digit.ADCs();
            fRawTree->Fill();
            fHitTree->Fill();
            fCalTree->Fill();
        }
        RawDigit.clear();

        hits.clear();

        start_ticks.clear();
        end_ticks.clear();
        peak_times.clear();
        peak_amps.clear();

        calwire.clear();
    }
 }
    DEFINE_ART_MODULE(GetRaw)
} // namespace GetRaw

#endif // GetRaw_Module
