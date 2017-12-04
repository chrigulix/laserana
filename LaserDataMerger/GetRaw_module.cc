

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


    return;
}

void GetRaw::endJob()
{
}

void GetRaw::reconfigure(fhicl::ParameterSet const& parameterSet)
{
    fRawLabel = parameterSet.get<art::InputTag>("RawLabel", "daq");
    fChannels = parameterSet.get<std::vector<unsigned int> >("Channels");
}

//-----------------------------------------------------------------------

void GetRaw::produce(art::Event& event)
{
    art::Handle<std::vector<raw::RawDigit> > Raw;

    event.getByLabel(fRawLabel, Raw);


    //auto track = tr.fXYZ;
    for (auto const &Digit : *Raw) {
        auto test = raw::RawDigit();

        Channel = Digit.Channel();

        if(std::find(fChannels.begin(), fChannels.end(), Channel) != fChannels.end()) {

            auto WireID = fGeometry->ChannelToWire(Channel);

            plane = WireID.front().Plane;
            wire = WireID.front().Wire;

            std::cout << "Writing Channel: " << Channel
                      << " Plane/Wire: " <<  plane  << '/' << wire
                      << std::endl;
            event_id = (unsigned int) event.id().event();
            RawDigit = Digit.ADCs();
            fRawTree->Fill();
        }
        RawDigit.clear();

    }
 }
    DEFINE_ART_MODULE(GetRaw)
} // namespace GetRaw

#endif // GetRaw_Module
