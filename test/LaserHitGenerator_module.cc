#ifndef LaserObjectsTest_Module
#define LaserObjectsTest_Module

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"

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

        bool DEBUG = true;

    protected:
    };

    LaserHitGenerator::LaserHitGenerator(fhicl::ParameterSet const& pset) {
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
        std::string HitFile = fHitFile;
        fstream stream(HitFile, std::ios::in);
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
            std::cerr << "Error: Unable to open file " << HitFile << std::endl;
        }
    }

    void LaserHitGenerator::endJob() {
    }

    void LaserHitGenerator::produce(art::Event& event)  {

        // Its going to wrap around the hit config if you specify too many events
        auto id = event.id().event();

        if (id > HitValues.size() - 1 ) return;

        for (auto& HitValue : HitValues.at(id)) {

            std::cout
                      << HitValue.at(HitDataStructure::Wire) << " "
                      << HitValue.at(HitDataStructure::Plane) << " "
                      << HitValue.at(HitDataStructure::TickStart) << " "
                      << HitValue.at(HitDataStructure::TickEnd) << " "
                      << HitValue.at(HitDataStructure::PeakAmplitude) << " "
                      << HitValue.at(HitDataStructure::Multiplicity) << std::endl;

            //auto Wire = recob::Wire(i,i,geo::_plane_proj::kW);

/*
 *
 * auto channel = i;


            auto Wire = recob::Wire(1,channel,geo::_plane_proj::kW);



            auto RecoHit = recob::HitCreator(Wire,
                                             i,
                                             HitStart,
                                             HitEnd,
                                             fabs(HitStart - HitEnd) / 2,
                                             (float) PeakTime,
                                             fabs(HitStart - HitEnd) / 2,
                                             Peak,
                                             sqrt(Peak),
                                             0.,
                                             0.,
                                             1,
                                             HitIdx,
                                             1.,
                                             0).move();*/
        }

        //for (auto wire: wires){
        //    std::cout << "bibi: " << wire.Signal().at(0) << std::endl;
        //}

    }

    DEFINE_ART_MODULE(LaserHitGenerator)
}

#endif //LaserUtils_test