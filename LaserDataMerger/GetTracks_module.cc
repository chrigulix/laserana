

#ifndef GetTracks_Module
#define GetTracks_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "lardataobj/RecoBase/Track.h"

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
    TTree* fLaserTree;

    std::vector<float> trackx;
    std::vector<float> tracky;
    std::vector<float> trackz;
    unsigned int event_id;
    unsigned int track_id;

    Double_t laser_entry_x;
    Double_t laser_entry_y;
    Double_t laser_entry_z;

    Double_t laser_exit_x;
    Double_t laser_exit_y;
    Double_t laser_exit_z;

    TVector3 position;
    TVector3 direction;

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

    fLaserTree = tfs->make<TTree>("Laser", "Laser");
    fLaserTree->Branch("event", &event_id);
    fLaserTree->Branch("entry_x", &laser_entry_x);
    fLaserTree->Branch("entry_y", &laser_entry_y);
    fLaserTree->Branch("entry_z", &laser_entry_z);
    fLaserTree->Branch("exit_x", &laser_exit_x);
    fLaserTree->Branch("exit_y", &laser_exit_y);
    fLaserTree->Branch("exit_z", &laser_exit_z);
    fLaserTree->Branch("dir", &direction);
    fLaserTree->Branch("pos", &position);

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
    art::Handle<lasercal::LaserBeam>  Laser;

    event.getByLabel(fTrackLabel, Tracks);

    try {
        event.getByLabel("LaserDataMerger", "LaserBeam", Laser);

    	event_id = event.id().event();
        laser_entry_x = Laser->GetEntryPoint().x();
        laser_entry_y = Laser->GetEntryPoint().y();
        laser_entry_z = Laser->GetEntryPoint().z();

        laser_exit_x = Laser->GetExitPoint().x();
        laser_exit_y = Laser->GetExitPoint().y();
        laser_exit_z = Laser->GetExitPoint().z();

        direction = Laser->GetLaserDirection();
        position = Laser->GetLaserPosition();

        fLaserTree->Fill();
    }
    catch (...){
        std::cout << "Could not find laser tracks" << std::endl;
    }; // pretty dangerous, but we just ignore writing the laser tree if no laser data is present.


    //auto track = tr.fXYZ;
    for (auto const &Track : *Tracks) {

        event_id = (unsigned int) event.id().event();
        track_id = Track.ID();

        size_t track_size = Track.NumberTrajectoryPoints();

        trackx.resize(track_size); tracky.resize(track_size); trackz.resize(track_size);

        for (size_t i = 0; i < track_size; ++i)  {
            trackx.at(i) = (float) Track.LocationAtPoint(i).x();
            tracky.at(i) = (float) Track.LocationAtPoint(i).y();
            trackz.at(i) = (float) Track.LocationAtPoint(i).z();
        }
        fTrackTree->Fill();
        trackx.clear(); tracky.clear(); trackz.clear();
    }
 }
    DEFINE_ART_MODULE(GetTracks)
} // namespace GetTracks

#endif // GetTracks_Module
