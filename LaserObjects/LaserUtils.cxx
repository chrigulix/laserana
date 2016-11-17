//
// Created by matthias on 21.07.16.
//
#include "LaserUtils.h"
#include "LaserParameters.h"

std::vector<recob::Wire> lasercal::GetWires(art::ValidHandle<std::vector<raw::RawDigit>> &DigitVecHandle,
                                            lasercal::LaserRecoParameters &fParameterSet,
                                            bool SubstractPedestal) {

    std::vector<recob::Wire> WireVec;

    std::vector<geo::WireID> WireIDs;

    recob::Wire::RegionsOfInterest_t RegionOfInterest;

    // Reserve space for wire vector
    WireVec.reserve(DigitVecHandle->size());

    // Get Service providers
    const lariov::DetPedestalProvider &PedestalRetrievalAlg = art::ServiceHandle<lariov::DetPedestalService>()->GetPedestalProvider();
    const lariov::ChannelStatusProvider &ChannelFilter = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();

    // Initialize raw time tick vectors
    std::vector<short> RawADC;
    std::vector<float> RawROI;
    RawADC.resize(DigitVecHandle->at(0).Samples());
    RawROI.resize(DigitVecHandle->at(0).Samples());
    // Get channel ID



    // Loop over all raw digit entries
    for (auto const &RawDigit : *DigitVecHandle) {

        raw::ChannelID_t channel = RawDigit.Channel();

        // Skip channel if dead or noisy
        if (ChannelFilter.Status(channel) < fParameterSet.MinAllowedChanStatus || !ChannelFilter.IsPresent(channel)) {
            continue;// jump to next iterator in RawDigit loop
        }

        // Extract data into RawADC vector and uncompress it
        raw::Uncompress(RawDigit.ADCs(), RawADC, RawDigit.Compression());

        // Copy the Raw ADC digit (short) into the signal vector (float)
        std::copy(RawADC.begin(), RawADC.end(), RawROI.begin());

        // subtract pedestial
        if (SubstractPedestal) {
            for (auto &RawSample : RawROI) {
                RawSample -= PedestalRetrievalAlg.PedMean(channel);
            }
        }
        // Create the region of interest (in this case the whole wire)
        RegionOfInterest.add_range(0, RawROI.begin(), RawROI.end());

        // Create a Wire object with the raw signal
        WireVec.emplace_back(recob::WireCreator(std::move(RegionOfInterest), RawDigit).move());

    } // end loop over raw digit entries

    return WireVec;
}

std::unique_ptr<std::vector<std::vector<std::vector<float>>>> lasercal::ReadHitDefs(std::string Filename, bool DEBUG)
/*
 * Reads hit definitions from csv file
 *
 *  Filename:   path to csv file
 *  DEBUG:      outputs line by line input read
 *
 *  returns:    - inner most vector is configuration of hit in the form:
 *              - middle vector contains all hits for a single event
 *              - outer vector contains hits over all events
 *
 */

{
    std::unique_ptr<std::vector<std::vector<std::vector<float> > > > RawDigitValues( new std::vector<std::vector<std::vector<float> > >); ///< line by line csv container
    std::fstream stream(Filename, std::ios::in);
        // Reading of RawDigit config file
        int EventIdx = -2;

        // read the hit data file into a vector
        if (stream) {
            typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
            boost::char_separator<char> sep(", ");
            std::string line;

            std::vector<std::vector<float>> Hits;

            while (getline(stream, line)) {
                // skip comments
                if (line[0] == '#') {
                    if (EventIdx > -1) RawDigitValues->push_back(Hits);
                    Hits.clear();
                    EventIdx++;
                    continue;
                }

                Tokenizer info(line, sep); // tokenize the line of data
                std::vector<float> values;
                for (Tokenizer::iterator it = info.begin(); it != info.end(); ++it) {
                    // convert data into double value, and store
                    values.push_back(std::strtof(it->c_str(), 0));
                }
                // store array of values
                Hits.push_back(values);


                if (DEBUG) {
                    std::cout << "DEBUG " << EventIdx << " ";
                    for (auto entry : values) {
                        std::cout << entry << " ";

                    }
                    std::cout << std::endl;
                }
            }
            stream.close();
        } else {
            stream.close();
            throw art::Exception(art::errors::FileOpenError) << " File does not exist: " << Filename << std::endl;
        }

    return std::move(RawDigitValues);
}