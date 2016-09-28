#ifndef LaserObjectsTest_Module
#define LaserObjectsTest_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Utilities/Exception.h"

#include "LaserObjects/LaserUtils.h"
#include "LaserObjects/LaserHits.h"

#include <assert.h>

#include <fstream>
#include <boost/tokenizer.hpp>

#include <string>

namespace LaserHitGenerator {

    class LaserHitGenerator : public art::EDProducer {

    public:
        explicit LaserHitGenerator(fhicl::ParameterSet const& pset);
        virtual ~LaserHitGenerator();

        virtual void produce (art::Event& event) override;

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void reconfigure(fhicl::ParameterSet const &p) override;

    private:

        // External Hit generator definitions
        std::string fHitFile;
        // inner most vector is configuration for hit
        // middle vector contains all hits for the specific event
        // outer vector contians hits over all events
        std::vector< std::vector< std::vector<float> > > HitValues; ///< line by line csv container

        enum HitDataStructure {
            Wire,
            Plane,
            TickStart,
            TickEnd,
            PeakAmplitude,
            Multiplicity
        };

        bool DEBUG = false;

    protected:
    };

    LaserHitGenerator::LaserHitGenerator(fhicl::ParameterSet const& pset) {

        produces<std::vector<recob::Hit> >("TestHits");
        this->reconfigure(pset);
    }

    LaserHitGenerator::~LaserHitGenerator() {
    }

    void LaserHitGenerator::reconfigure(fhicl::ParameterSet const &pset) {
        fHitFile = pset.get< std::string >("HitFile");
    }

    void LaserHitGenerator::beginJob() {
        int EventIdx = -2;

        // read the hit data file into a vector
        std::string FileName = fHitFile;
        fstream stream(FileName, std::ios::in);
        if (stream)
        {
            typedef boost::tokenizer< boost::char_separator<char> > Tokenizer;
            boost::char_separator<char> sep(", ");
            std::string line;

            std::vector<std::vector<float>> Hits;

            while (getline(stream, line))
            {
                // skip comments
                if (line[0] =='#' ){
                    if (EventIdx > -1) HitValues.push_back(Hits);
                    Hits.clear();
                    EventIdx++;
                    continue;
                }

                Tokenizer info(line, sep); // tokenize the line of data
                std::vector<float> values;
                for (Tokenizer::iterator it = info.begin(); it != info.end(); ++it)
                {
                    // convert data into double value, and store
                    values.push_back(std::strtof(it->c_str(), 0));
                }
                // store array of values
                Hits.push_back(values);


                if (DEBUG)
                {
                    std::cout << "DEBUG " << EventIdx << " ";
                    for (auto entry : values){
                        std::cout << entry << "";

                    }
                    std::cout << std::endl;
                }
            }
            stream.close();
        }
        else
        {
            throw art::Exception(art::errors::FileOpenError) << " File does not exist: " << FileName << std::endl;
        }
    }

    void LaserHitGenerator::endJob() {
    }

    void LaserHitGenerator::produce(art::Event& event)  {

        std::unique_ptr<std::vector<recob::Hit> > TestHits(new std::vector<recob::Hit>);
        auto id = event.id().event();
        unsigned int HitIdx = 0;

        if (id > HitValues.size() - 1 ) {
            event.put(std::move(TestHits), "TestHits");
            return;
        };

        auto HitsInThisEvent = HitValues.at(id);

        // create all hits in this event
        for (auto SingleHit : HitsInThisEvent) {

            unsigned int Wire_id = SingleHit.at(HitDataStructure::Wire);
            unsigned int Plane = SingleHit.at(HitDataStructure::Plane);
            float TickStart1 = SingleHit.at(HitDataStructure::TickStart);
            float TickEnd = SingleHit.at(HitDataStructure::TickEnd);
            float PeakAmplitude = SingleHit.at(HitDataStructure::PeakAmplitude);
            float Multiplicity = SingleHit.at(HitDataStructure::Multiplicity);

            std::vector<float> DummyROI = {0,1,2};
            const geo::WireID WireID = geo::WireID(1,1,Plane, Wire_id);

            recob::Wire::RegionsOfInterest_t RegionOfInterest;
            RegionOfInterest.add_range(0,DummyROI.begin(), DummyROI.end());

            const auto Wire = recob::Wire(RegionOfInterest, HitIdx, geo::_plane_proj::kW);


            auto RecoHit = recob::HitCreator(Wire,
                                             WireID,
                                             TickStart1,
                                             TickEnd,
                                             fabs(TickStart - TickEnd) / 2,
                                             (float) PeakAmplitude,
                                             fabs(TickStart - TickEnd) / 2,
                                             PeakAmplitude,
                                             sqrt(PeakAmplitude),
                                             0.,
                                             0,
                                             1,
                                             Multiplicity,
                                             1,
                                             (size_t) 0).move();

            TestHits->push_back(RecoHit);
            HitIdx++;
        }
        event.put(std::move(TestHits), "TestHits");

    }
    DEFINE_ART_MODULE(LaserHitGenerator)
}

#endif //LaserUtils_test