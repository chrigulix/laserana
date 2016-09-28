//
// Created by matthias on 21.07.16.
//
#include "LaserUtils.h"
#include "LaserParameters.h"

std::vector<recob::Wire> lasercal::GetWires(art::ValidHandle<std::vector<raw::RawDigit>> &DigitVecHandle, lasercal::LaserRecoParameters &fParameterSet) {

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
        for (auto &RawSample : RawROI) {
            RawSample -= PedestalRetrievalAlg.PedMean(channel);
        }

        // Create the region of interest (in this case the whole wire)
        RegionOfInterest.add_range(0, RawROI.begin(), RawROI.end());

        // Create a Wire object with the raw signal
        WireVec.emplace_back(recob::WireCreator(std::move(RegionOfInterest), RawDigit).move());

    } // end loop over raw digit entries

    return WireVec;
}