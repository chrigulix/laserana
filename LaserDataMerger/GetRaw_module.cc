

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
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"


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

    bool fGetAll;
    unsigned int fGetPlane;

    // raw digit holders
    std::vector<short> RawDigit;
    unsigned int Channel;
    unsigned int event_id;
    unsigned int wire;
    unsigned int plane;

    art::InputTag fRawLabel;
    std::vector<std::pair<unsigned int, unsigned int>> fUWires;
    std::vector<std::pair<unsigned int, unsigned int>> fVWires;
    std::vector<std::pair<unsigned int, unsigned int>> fYWires;


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
    fYWires = parameterSet.get<std::vector<std::pair<unsigned int, unsigned int>> >("YWires");
    fGetAll = parameterSet.get<bool>("GetAll", true);
    fGetPlane = parameterSet.get<unsigned int>("GetPlane", 2);
    //fUWires = parameterSet.get<std::vector<std::pair<unsigned int, unsigned int>> >("UWires");
    //fVWires = parameterSet.get<std::vector<std::pair<unsigned int, unsigned int>> >("VWires");
}

//-----------------------------------------------------------------------

void GetRaw::produce(art::Event& event)
{
    art::Handle<std::vector<raw::RawDigit> > Raw;
    const lariov::ChannelStatusProvider &ChannelFilter = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();

    event.getByLabel(fRawLabel, Raw);

    raw::RawDigit aaa;
    //auto track = tr.fXYZ;
    for (auto const &Digit : *Raw) {

        Channel = Digit.Channel();
        auto WireID = fGeometry->ChannelToWire(Channel);
        auto this_wire = WireID.front().Wire;
        auto this_plane = WireID.front().Plane;
        if (fGetAll) {
            if (this_plane != fGetPlane) continue; // only interested in collection plane

            if (ChannelFilter.Status(Channel) < 1 || !ChannelFilter.IsPresent(Channel)) {
                continue;// jump to next iterator in RawDigit loop
            }
            plane = this_plane;
            wire = this_wire;
            event_id = (unsigned int) event.id().event();

            // Cut out interesting region
            for (uint sp = 4000; sp < 6001; sp++){
                RawDigit.push_back(Digit.ADC(sp));
            }

            fRawTree->Fill();
            RawDigit.clear();
        }
        else {
            // for the moment we only access y-plane
            if (this_plane != 2) continue;

            // loop over all specified pairs of ranges
            for (uint range_idx = 0; range_idx < fYWires.size(); range_idx++) {

                auto range = fYWires[range_idx];
                if ((range.first <= this_wire) and (this_wire <= range.second)) {

                    plane = this_plane;
                    wire = this_wire;

                    std::cout << "Writing Channel: " << Channel
                              << " Plane/Wire: " << plane << '/' << wire
                              << std::endl;
                    event_id = (unsigned int) event.id().event();
                    RawDigit = Digit.ADCs();
                    fRawTree->Fill();
                }
                RawDigit.clear();
            }
        }
    }
 }
    DEFINE_ART_MODULE(GetRaw)
} // namespace GetRaw

#endif // GetRaw_Module
