

#ifndef GetTracks_Module
#define GetTracks_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include "larsim/MCCheater/BackTracker.h"

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

#include "larevt/SpaceChargeServices/SpaceChargeService.h"

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

    // return the y value at x for a line defined by m and b supplied by line
    double pt(double x, std::pair<double, double> line);

    // calculate the line parameters based on two points
    std::pair<double, double> get_line(std::pair<double, double> pt1, std::pair<double, double> pt2);


private:

    bool DEBUG = false;

    // All this goes into the root tree
    TTree* fTrackTree;
    TTree* fLaserTree;
    TTree* fTrueTree;

    // reco track
    std::vector<float> trackx;
    std::vector<float> tracky;
    std::vector<float> trackz;
    unsigned int event_id;
    unsigned int track_id;

    // true track
    std::vector<float> true_trackx;
    std::vector<float> true_tracky;
    std::vector<float> true_trackz;
    unsigned int true_event_id;
    unsigned int true_track_id;

    Double_t laser_entry_x;
    Double_t laser_entry_y;
    Double_t laser_entry_z;

    Double_t laser_exit_x;
    Double_t laser_exit_y;
    Double_t laser_exit_z;

    TVector3 position;
    TVector3 direction;

    // fhicl input parameters
    art::InputTag fTrackLabel;

    bool fGetTracks;
    bool fGetLaser;
    bool fGetMC = false;
    bool fGetTrue = false;
    bool fPerfectTrack;

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

    fTrueTree = tfs->make<TTree>("True", "Tracks");
    fTrueTree->Branch("event", &event_id);
    fTrueTree->Branch("x", &true_trackx);
    fTrueTree->Branch("y", &true_tracky);
    fTrueTree->Branch("z", &true_trackz);
    fTrueTree->Branch("track_id", &track_id);

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
    fGetTracks = parameterSet.get<bool>("GetTracks", true);
    fGetLaser = parameterSet.get<bool>("GetLaser", true);
    fGetMC = parameterSet.get<bool>("GetMC", false);
    fPerfectTrack = parameterSet.get<bool>("GetPerfectTracks", true);
}

//-----------------------------------------------------------------------

void GetTracks::produce(art::Event& event)
{

    art::Handle<std::vector<simb::MCTruth>> MCTruth;
    art::Handle<std::vector<simb::MCParticle>> MCParticles;
    art::Handle<std::vector<sim::MCTrack>> MCTrack;

    art::Handle<std::vector<recob::Track> > Tracks;
    art::Handle<lasercal::LaserBeam>  Laser;

/*

    art::Handle< std::vector<recob::Hit> > hitcol;
    event.getByLabel("laserhit", hitcol);

    // make a vector of them - we aren't writing anything out to a file
    // so no need for a art::PtrVector here
    std::vector< art::Ptr<recob::Hit> > hits;
    art::fill_ptr_vector(hits, hitcol);

    // loop over the hits and figure out which particle contributed to each one
    std::vector< art::Ptr<recob::Hit> >::iterator itr = hits.begin();

    art::ServiceHandle<cheat::BackTracker> bt;

    std::cout << "SIZE: " << hits.size() << std::endl;
    std::cout << "SIZE: " << hitcol->size() << std::endl;

    int i = 0;
    while( itr != hits.end() ) {
        std::vector<double> xyz = bt->HitToXYZ(*itr);
        true_trackx.push_back((float) xyz[0]);
        true_tracky.push_back((float) xyz[1]);
        true_trackz.push_back((float) xyz[2]);
        //std::cout << "offsets: [" << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << "]" << std::endl;
        itr++;
        i++;
    }
    std::cout << "SIZE: " << i << std::endl;
    fTrueTree->Fill();
    true_trackx.clear();
    true_tracky.clear();
    true_trackz.clear();
*/


    event_id = (unsigned int) event.id().event();

    if (fGetMC) {
        try {
            event.getByLabel("mcreco", "", MCTrack);


            for (auto const &mctrack : *MCTrack) {
                if (mctrack.PdgCode() == 13) { // Only look at muons
                    size_t track_size = mctrack.size();
                    true_trackx.resize(track_size), true_tracky.resize(track_size), true_trackz.resize(track_size);

                    for (uint idx = 0; idx < track_size; idx++) {
                        auto pt = mctrack.at(idx);
                        true_trackx.at(idx) = pt.X();
                        true_tracky.at(idx) = pt.Y();
                        true_trackz.at(idx) = pt.Z();

                        std::cout << "[" << pt.X() << ", " << pt.Y() << ", " << pt.Z() << "]" <<std::endl;
                    }
                    fTrueTree->Fill();
                    true_trackx.clear();
                    true_tracky.clear();
                    true_trackz.clear();
                }
            }
        }
        catch (...) {
            std::cout << "Could not find mctruth information" << std::endl;
        }
    }

    if (fGetLaser) {
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

            if (fPerfectTrack) {
                // Here we produce perfect reconstruction. Meaning we use the laser inforamtion
                // to introduce track points which we then distort. This is for simulation and
                // verification.
                Double_t stepsize = 0.3; ///< Stepsize of track production in mm

                double start_z;
                double end_z;

                if (laser_entry_z > laser_exit_z) {
                    start_z = laser_exit_z;
                    end_z = laser_entry_z;
                }
                else {
                    start_z = laser_entry_z;
                    end_z = laser_exit_z;
                }
                // construct the points based on entry and exit
                auto zx_entry = std::make_pair(laser_entry_z, laser_entry_x);
                auto zx_exit =  std::make_pair(laser_exit_z, laser_exit_x);

                auto zy_entry = std::make_pair(laser_entry_z, laser_entry_y);
                auto zy_exit = std::make_pair(laser_exit_z, laser_exit_y);

                // calculate the line parameters for the two planes
                std::pair<double, double> zx_line = get_line(zx_entry, zx_exit);
                std::pair<double, double> zy_line = get_line(zy_entry, zy_exit);

                if (DEBUG) {
                    std::cout << "zx line: " << zx_line.first << " " << zx_line.second << std::endl;
                    std::cout << "zy line: " << zy_line.first << " " << zy_line.second << std::endl;
                }


                // now we loop over the range in z to obtain all tracks
                for (Double_t z = start_z; z < end_z; z += stepsize) {

                    // get the points on the planes for the corresponding z
                    double x = pt(z, zx_line);
                    double y = pt(z, zy_line);

                    // Now get the space chage distortions
                    auto const* SCE = lar::providerFrom<spacecharge::SpaceChargeService>();
                    auto offset = SCE->GetPosOffsets(x,y,z);

                    if (DEBUG) {
                        std::cout << " at: " << z << ", " << x << ", " << y << std::endl;
                        std::cout << " of: [" << offset[0] << ", " << offset[1] << ", " << offset[2] << "]"
                                  << std::endl;
                    }

                    // Apply the correction and store it to the vector
                    true_trackx.push_back(x + (-offset[0])); // This is convention beacause of the uboone coordinate system
                    true_tracky.push_back(y  + offset[1]);
                    true_trackz.push_back(z + offset[2]);
                }

                // Fill it in.
                fTrueTree->Fill();
                true_trackx.clear();
                true_tracky.clear();
                true_trackz.clear();
            }
        }
        catch (...) {
            std::cout << "Could not find laser tracks" << std::endl;
        }; // pretty dangerous, but we just ignore writing the laser tree if no laser data is present.
    }

    if (fGetTracks) {
        event.getByLabel(fTrackLabel, Tracks);
        //auto track = tr.fXYZ;
        for (auto const &Track : *Tracks) {

            track_id, true_track_id = Track.ID();

            size_t track_size = Track.NumberTrajectoryPoints();

            trackx.resize(track_size);
            tracky.resize(track_size);
            trackz.resize(track_size);
            if (fGetTrue) {
                true_trackx.resize(track_size);
                true_tracky.resize(track_size);
                true_trackz.resize(track_size);
            }

            for (size_t i = 0; i < track_size; ++i) {
                double x = Track.LocationAtPoint(i).x();
                double y = Track.LocationAtPoint(i).y();
                double z = Track.LocationAtPoint(i).z();

                trackx.at(i) = (float) x;
                tracky.at(i) = (float) y;
                trackz.at(i) = (float) z;

                if (fGetTrue)
                {
                    auto const* SCE = lar::providerFrom<spacecharge::SpaceChargeService>();
                    auto offset = SCE->GetPosOffsets(x,y,z);
                    std::cout << "offsets: [" << offset[0] << ", " << offset[1] << ", " << offset[2] << "]" << std::endl;

                    true_trackx.at(i) = (float) (x - offset[0]);
                    true_tracky.at(i) = (float) (y - offset[1]);
                    true_trackz.at(i) = (float) (z - offset[2]);
                }
            }
            fTrackTree->Fill();
            trackx.clear();
            tracky.clear();
            trackz.clear();

            if (fGetTrue) {
                fTrueTree->Fill();
                true_trackx.clear();
                true_tracky.clear();
                true_trackz.clear();
            }
        }
    }
 }

    double GetTracks::pt(double x, std::pair<double, double> line) {
        double y;
        y = line.first * x + line.second;
        return y;
    }

    std::pair<double, double> GetTracks::get_line(std::pair<double, double> pt1, std::pair<double, double> pt2) {
        double m, b;
        m = (pt2.second - pt1.second) / (pt2.first - pt1.first);
        b = pt1.second - m * pt1.first;
        return std::make_pair(m, b);
    }
    DEFINE_ART_MODULE(GetTracks)
} // namespace GetTracks

#endif // GetTracks_Module
