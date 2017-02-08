

#ifndef GetTracks_Module
#define GetTracks_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "lardata/RecoBase/Track.h"

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


namespace GetTracks
{

class GetTracks : public art::EDProducer
{
public:

    explicit GetTracks(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;

    virtual void beginRun(art::Run& run);

    virtual void endJob() override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void produce(art::Event& event) override;
   

private:

    bool DEBUG = true;

    // All this goes into the root tree
    TTree* fTrackTree;
    std::vector<Double_t> trackx;
    std::vector<Double_t> tracky;
    std::vector<Double_t> trackz;
    unsigned int event_id;
    unsigned int track_id;


    art::InputTag fTrackLabel;

}; // class GetTracks



GetTracks::GetTracks(fhicl::ParameterSet const& pset)
{

    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
}


//-----------------------------------------------------------------------

void GetTracks::beginJob()
{

}

void GetTracks::beginRun(art::Run& run)
{
    
    art::ServiceHandle<art::TFileService> tfs;

    // Initialize time info root file
    fTrackTree = tfs->make<TTree>("Tracks", "Tracks");
    fTrackTree->Branch("event", &event_id);
    fTrackTree->Branch("x", &trackx);
    fTrackTree->Branch("y", &tracky);
    fTrackTree->Branch("z", &trackz);
    fTrackTree->Branch("track_id", &track_id);
    return;
}

void GetTracks::endJob()
{
}

void GetTracks::reconfigure(fhicl::ParameterSet const& parameterSet)
{
    fTrackLabel = parameterSet.get<art::InputTag>("TrackLabel");
}

//-----------------------------------------------------------------------

void GetTracks::produce(art::Event& event)
{
    art::Handle<std::vector<recob::Track> > Tracks;

    event.getByLabel(fTrackLabel, Tracks);

    recob::Track track;
    //auto track = tr.fXYZ;
    for (auto const &Track : *Tracks) {

        event_id = (unsigned int) event.id().event();
        track_id = Track.ID();

        size_t track_size = Track.NumberTrajectoryPoints();

        trackx.resize(track_size); tracky.resize(track_size); trackz.resize(track_size);

        for (size_t i = 0; i < track_size; ++i)  {
            trackx.at(i) = Track.LocationAtPoint(i).x();
            tracky.at(i) = Track.LocationAtPoint(i).y();
            trackz.at(i) = Track.LocationAtPoint(i).z();
        }
        fTrackTree->Fill();
        trackx.clear(); tracky.clear(); trackz.clear();
    }
 }
    DEFINE_ART_MODULE(GetTracks)
} // namespace GetTracks

#endif // GetTracks_Module
