
/*
 * Little module to extract the space charge distortion and store it in the same format that we use
 * for laser field calibration output
 *
 */

#ifndef GetTracks_Module
#define GetTracks_Module

// LArSoft includes
#include "lardataobj/RecoBase/Track.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"

#include "larcore/CoreUtils/ServiceUtil.h"

#include "larevt/SpaceChargeServices/SpaceChargeService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

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

class GetDistortion : public art::EDProducer
{
public:

    explicit GetDistortion(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;

    virtual void beginRun(art::Run& run);

    virtual void endJob() override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void produce(art::Event& event) override;
   

private:

    bool DEBUG = true;

    spacecharge::SpaceCharge const* sSpaceCharge;
    geo::GeometryCore const *sGeometry;

    std::vector<unsigned int> DetectorResolution = {26,26,101};

}; // class GetDistortion



GetDistortion::GetDistortion(fhicl::ParameterSet const& pset)
{

    sSpaceCharge = lar::providerFrom<spacecharge::SpaceChargeService>();
    sGeometry = lar::providerFrom<geo::Geometry>();
    this->reconfigure(pset);
}


//-----------------------------------------------------------------------

void GetDistortion::beginJob()
{
}

void GetDistortion::beginRun(art::Run& run)
{
    auto y_max = sGeometry->DetHalfHeight();
    auto y_min = - y_max;
    double y_step = (y_max - y_min) / DetectorResolution[1];

    auto x_max = 2.0 * sGeometry->DetHalfWidth();
    auto x_min = 0;
    double x_step = (x_max - x_min) / DetectorResolution[0];

    double z_min = 0.;
    auto z_max = sGeometry->DetLength();
    double z_step = (z_max - z_min) / DetectorResolution[2];

    // Initialize all TH3F
    std::vector<float> MinimumCoord = {-5.1208, -120.9, -5.184};    // These seem to be wrong, I copied them from our field calibration
    std::vector<float> MaximumCoord = {10.2416, -106.95, 10.368};   // These seem to be wrong, I copied them from our field calibration

    std::vector<TH3F> RecoDisplacement;
    RecoDisplacement.push_back(TH3F("Reco_Displacement_X","Reco Displacement X",DetectorResolution[0],MinimumCoord[0],MaximumCoord[0],DetectorResolution[1],MinimumCoord[1],MaximumCoord[1],DetectorResolution[2],MinimumCoord[2],MaximumCoord[2]));
    RecoDisplacement.push_back(TH3F("Reco_Displacement_Y","Reco Displacement Y",DetectorResolution[0],MinimumCoord[0],MaximumCoord[0],DetectorResolution[1],MinimumCoord[1],MaximumCoord[1],DetectorResolution[2],MinimumCoord[2],MaximumCoord[2]));
    RecoDisplacement.push_back(TH3F("Reco_Displacement_Z","Reco Displacement Z",DetectorResolution[0],MinimumCoord[0],MaximumCoord[0],DetectorResolution[1],MinimumCoord[1],MaximumCoord[1],DetectorResolution[2],MinimumCoord[2],MaximumCoord[2]));

    // Loop over all xbins
    for(unsigned xbin = 0; xbin < DetectorResolution[0]; xbin++)
    {
        // Loop over all ybins
        for(unsigned ybin = 0; ybin < DetectorResolution[1]; ybin++)
        {
            // Loop over all zbins
            for(unsigned zbin = 0; zbin < DetectorResolution[2]; zbin++)
            {
                auto Dispacement = sSpaceCharge->GetPosOffsets(xbin*x_step, y_min + (ybin*y_step), zbin * z_step);

                // Loop over all coordinates
                for(unsigned coord = 0; coord < 3; coord++)
                {
                    // you have to multiply the x component by -1 because of uboone coordinate system conventions
                    // discussed in some meeting...
                    if (coord == 0) {
                        Dispacement[0] *= -1.;
                    }
                    std::cout << "pos: " << xbin*x_step << "/" << y_min + (ybin * y_step) << "/" << zbin * z_step  << " dis: " << Dispacement[0] << "/" << Dispacement[1] << "/" << Dispacement[2] << std::endl;
                    // Fill interpolated grid points into histograms
                    RecoDisplacement[coord].SetBinContent(xbin+1,ybin+1,zbin+1, Dispacement[coord]);
                } // end coordinate loop
            } // end zbin loop
        } // end ybin loop
    } // end zbin loop

    TFile OutputFile("SpaceCharge.root", "recreate");

    // Loop over space coordinates
    for(unsigned coord = 0; coord < RecoDisplacement.size(); coord++)
    {
        // Write every TH3 map into file
        RecoDisplacement[coord].Write();
    }

    // Close output file and clean up
    OutputFile.Close();
    gDirectory->GetList()->Delete();

    return;
}

void GetDistortion::endJob()
{
}

void GetDistortion::reconfigure(fhicl::ParameterSet const& parameterSet)
{

}

//-----------------------------------------------------------------------

void GetDistortion::produce(art::Event& event) {
}
    DEFINE_ART_MODULE(GetDistortion)
} // namespace GetDistortion

#endif
