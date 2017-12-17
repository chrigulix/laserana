////////////////////////////////////////////////////////////////////////
// Class:       LaserSim
// Module Type: producer
// File:        LaserSim_module.cc
//
// Generated at Thu Sep 22 16:12:52 2016 by matthias,,, using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/Exception.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"

#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

#include "lardataobj/Simulation/SimChannel.h"
#include "larsim/LArG4/LArVoxelReadout.h"

#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/math/distributions/normal.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "LaserObjects/LaserUtils.h"

class LaserSim;

/* This module produces Gaussian hits which can be specified in the RawDigit file. It fills the rest of
 * the wire with random values.
*/

class LaserSim : public art::EDProducer {
public:
    explicit LaserSim(fhicl::ParameterSet const &p);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    LaserSim(LaserSim const &) = delete;

    LaserSim(LaserSim &&) = delete;

    LaserSim &operator=(LaserSim const &) = delete;

    LaserSim &operator=(LaserSim &&) = delete;

    // Required functions.
    void produce(art::Event &e) override;

    // Selected optional functions.
    void beginJob() override;

    void beginRun(art::Run &r) override;

    void endJob() override;

    void endRun(art::Run &r) override;

    void reconfigure(fhicl::ParameterSet const &p) override;

private:

    // Declare member data here.

    detinfo::DetectorProperties const *fDetProperties;  ///< pointer to detector properties provider
    geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider

    std::string fRawDigitFile;
    std::string fRawDigitLabel;

    bool DEBUG = false;

};

DEFINE_ART_MODULE(LaserSim)

LaserSim::LaserSim(fhicl::ParameterSet const &pset)
// :
// Initialize member data here.
{
    //fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    //fDetProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();
    // Call appropriate produces<>() functions here.
    this->reconfigure(pset);

    produces<std::vector<sim::SimChannel> >("");

}

// ==================================== PRODUCE ======================================
void LaserSim::produce(art::Event &event) {

    std::unique_ptr< std::vector<sim::SimChannel>  > scCol(new std::vector<sim::SimChannel>);

    auto id = event.id().event();
    sim::SimChannel test(8255);
    double xyz[3] = {1, 2, 3};
    test.AddIonizationElectrons(8255, 1000, 10000.,xyz,10000.);

    //larg4::LArVoxelReadout("bla", 0, 0);

    scCol->push_back(test);

    event.put(std::move(scCol));


}
// ==================================== PRODUCE ======================================

void LaserSim::beginJob() {
    // Reading of RawDigit config file
}

void LaserSim::beginRun(art::Run &r) {
    // Implementation of optional member function here.
}

void LaserSim::endJob() {
    // Implementation of optional member function here.
}

void LaserSim::endRun(art::Run &r) {
    // Implementation of optional member function here.
}

void LaserSim::reconfigure(fhicl::ParameterSet const &pset) {
    // Implementation of optional member function here.
    //fRawDigitFile = pset.get<std::string>("RawDigitFile");
}


