// RawFilter_module.cc

#ifndef HitFilter_Module
#define HitFilter_Module

// LArSoft includes
#include "lardataobj/RecoBase/Hit.h"
#include "larcore/Geometry/Geometry.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"




namespace HitFilter {

    class HitFilter : public art::EDProducer {
    public:

        /// Default constructor
        explicit HitFilter(fhicl::ParameterSet const &parameterSet);

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void reconfigure(fhicl::ParameterSet const &parameterSet) override;

        // The analysis routine, called once per event.
        virtual void produce(art::Event &event) override;

    private:
        art::InputTag fHitLabel;

        geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider
    }; // class RawFilter

    DEFINE_ART_MODULE(HitFilter)

    // Constructor
    HitFilter::HitFilter(fhicl::ParameterSet const &pset) {
        // get a pointer to the geometry service provider
        fGeometry = &*(art::ServiceHandle<geo::Geometry>());

        // Read in the parameters from the .fcl file.
        this->reconfigure(pset);
        produces<std::vector<recob::Hit> >("");
    }


    //-----------------------------------------------------------------------
    void HitFilter::beginJob() {

    }


    void HitFilter::endJob() {
    }

//-----------------------------------------------------------------------

    void HitFilter::reconfigure(fhicl::ParameterSet const &parameterSet) {
        fHitLabel = parameterSet.get<art::InputTag>("HitLabel", "laserhit");

    }

    //-----------------------------------------------------------------------
    void HitFilter::produce(art::Event &event) {
        art::Handle<std::vector<recob::Hit> > Hits;
        std::unique_ptr<std::vector<recob::Hit>> HitsFiltered(new std::vector<recob::Hit>);

        // Load Hits in event
        event.getByLabel(fHitLabel, Hits);

        for (auto const &Hit : *Hits) {
            // Loop over all hits in the event:
            bool keep = true;
            auto plane = fGeometry->ChannelToWire(Hit.Channel()).at(0).Plane;

            switch (plane) {
                case 0: // u plane
                    break;
                case 1: // v plane
                    break;
                case 2: // y plane
                    break;
            }
            if (keep) HitsFiltered->push_back(Hit);
        }

        event.put(std::move(HitsFiltered), "");

    } // RawFilter::analyze()

}

#endif // HitFilter_Module
