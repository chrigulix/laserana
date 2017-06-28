#include "LaserObjects/LaserHits.h"


lasercal::LaserHits::LaserHits(const lasercal::LaserRecoParameters &ParameterSet) {
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    fParameters = ParameterSet;
} // Default constructor

//-------------------------------------------------------------------------------------------------------------------

lasercal::LaserHits::LaserHits(const std::vector<recob::Wire> &Wires, const lasercal::LaserRecoParameters &ParameterSet,
                               const lasercal::LaserBeam &LaserBeam) {
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    fParameters = ParameterSet;

    fLaserROI = lasercal::LaserROI(fParameters.HitBoxSize, LaserBeam);

    // Reserve space for hit container
    for (auto &MapVector : fHitMapsByPlane) {
        MapVector.reserve(Wires.size());
    }

    // Loop over all wires
    for (const auto &SingleWire : Wires) {
        // Get channel information
        raw::ChannelID_t Channel = SingleWire.Channel();
        unsigned Plane = fGeometry->ChannelToWire(Channel).front().Plane;

        // Get Single wire hits
        std::map<float, recob::Hit> HitMap = FindSingleWireHits(SingleWire, Plane);

        // Fill map data by pushing back the wire vector
        fHitMapsByPlane.at(Plane).push_back(HitMap);
    }// end loop over wires

} // Constructor using all wire signals and geometry purposes

lasercal::LaserHits::LaserHits(const art::Handle< std::vector<recob::Wire>> WireHandle, const lasercal::LaserRecoParameters &ParameterSet,
                               const lasercal::LaserBeam &LaserBeam) {

    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    fParameters = ParameterSet;

    fLaserROI = lasercal::LaserROI(fParameters.HitBoxSize, LaserBeam);

    // Reserve space for hit container
    for (auto &MapVector : fHitMapsByPlane) {
        MapVector.reserve(WireHandle->size());
    }

    // Loop over all wires
    for (const auto &SingleWire : *WireHandle) {
        // Get channel information
        raw::ChannelID_t Channel = SingleWire.Channel();
        unsigned Plane = fGeometry->ChannelToWire(Channel).front().Plane;

        // Get Single wire hits
        std::map<float, recob::Hit> HitMap = FindSingleWireHits(SingleWire, Plane);

        // Fill map data by pushing back the wire vector
        fHitMapsByPlane.at(Plane).push_back(HitMap);
    }// end loop over wires

} // Constructor using all wire signals and geometry purposes

//-------------------------------------------------------------------------------------------------------------------

lasercal::LaserHits::LaserHits(const std::vector<recob::Wire> &Wires, const lasercal::LaserRecoParameters &ParameterSet,
                               lasercal::LaserROI &LaserROI) {
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    fParameters = ParameterSet;

    fLaserROI = LaserROI;

    // Reserve space for hit container
    for (auto &MapVector : fHitMapsByPlane) {
        MapVector.reserve(Wires.size());
    }

    // Loop over all wires
    for (const auto &SingleWire : Wires) {
        // Get channel information
        raw::ChannelID_t Channel = SingleWire.Channel();
        unsigned Plane = fGeometry->ChannelToWire(Channel).front().Plane;

        // Get Single wire hits
        std::map<float, recob::Hit> HitMap = FindSingleWireHits(SingleWire, Plane);
        // Fill map data by pushing back the wire vector
        fHitMapsByPlane.at(Plane).push_back(HitMap);
    }// end loop over wires

} // Constructor using all wire signals and geometry purposes

//-------------------------------------------------------------------------------------------------------------------

void lasercal::LaserHits::AddHitsFromWire(const recob::Wire &Wire) {
    // Get channel information
    raw::ChannelID_t Channel = Wire.Channel();
    unsigned Plane = fGeometry->ChannelToWire(Channel).front().Plane;

    // Get Single wire hits
    std::map<float, recob::Hit> HitMap = FindSingleWireHits(Wire, Plane);

    // Fill map data by pushing back the wire vector
    fHitMapsByPlane.at(Plane).push_back(HitMap);
}

//-------------------------------------------------------------------------------------------------------------------

const std::array<size_t, 3> lasercal::LaserHits::NumberOfWiresWithHits() {
    // Initialize output array
    std::array<size_t, 3> NumberOfWiresHit;

    // Loop over plane number
    for (size_t plane_no = 0; plane_no < NumberOfWiresHit.size(); plane_no++) {
        // Set wire with hit count to zero
        size_t WireWithSignalCount = 0;
        // Loop over all wires and their maps
        for (const auto &HitWireMap : fHitMapsByPlane.at(plane_no)) {
            // If the map has entries increase wires with hit count
            if (HitWireMap.size()) {
                WireWithSignalCount++;
            }
        }// end loop over wires

        // Fill number of wires with signal per plane
        NumberOfWiresHit.at(plane_no) = WireWithSignalCount;
    }
    // return the whole shit
    return NumberOfWiresHit;
}

//-------------------------------------------------------------------------------------------------------------------

void lasercal::LaserHits::clear() {
    for (auto &HitMaps : fHitMapsByPlane) {
        HitMaps.clear();
    }
}

//-------------------------------------------------------------------------------------------------------------------

std::unique_ptr<std::vector<recob::Hit> > lasercal::LaserHits::GetPlaneHits(size_t PlaneIndex) {
    std::unique_ptr<std::vector<recob::Hit> > HitVector(new std::vector<recob::Hit>);

    // Loop over all hit maps in a vector of a certain plane
    for (const auto &HitMaps : fHitMapsByPlane.at(PlaneIndex)) {
        // Loop through map
        for (const auto &HitMap : HitMaps) {
            HitVector->push_back(HitMap.second);
        }// loop through map
    }// loop over hit map vector

    return std::move(HitVector);
}

//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------

std::unique_ptr<std::vector<recob::Hit> > lasercal::LaserHits::GetHits() {
    std::unique_ptr<std::vector<recob::Hit> > HitVector(new std::vector<recob::Hit>);
    for (size_t plane = 0; plane < fNumberOfPlanes; plane++ ) {
        // Loop over all hit maps in a vector of a certain plane
        for (const auto &HitMaps : fHitMapsByPlane.at(plane)) {
            // Loop through map
            for (const auto &HitMap : HitMaps) {
                HitVector->push_back(HitMap.second);
            }// loop through map
        }// loop over hit map vector

    }
    return std::move(HitVector);
}

//-------------------------------------------------------------------------------------------------------------------

std::map<float, recob::Hit> lasercal::LaserHits::FindSingleWireHits(const recob::Wire &Wire, unsigned Plane) {
    // Initialize hit map
    std::map<float, recob::Hit> HitMap;

    // Check wich plane it is and use the corresponding hit finder algorithm
    if (Plane == 0) {
        HitMap = UPlaneHitFinder(Wire);
    } else if (Plane == 1) {
        HitMap = VPlaneHitFinder(Wire);
    } else if (Plane == 2) {
        HitMap = YPlaneHitFinder(Wire);
    }

    return HitMap;
}

//-------------------------------------------------------------------------------------------------------------------

void lasercal::LaserHits::TimeMatchFilter() {
    std::array<std::vector<std::map<float, recob::Hit>>, 3> NewHitMapsByPlane;

    for (size_t plane_no = 0; plane_no < fHitMapsByPlane.size(); plane_no++) {
        // Reserve memory space
        NewHitMapsByPlane.at(plane_no).resize(fHitMapsByPlane.at(plane_no).size());
    }

    float TimeMatchDifference = 3.0;

    // Loop over first two planes
    for (size_t plane_no = 0; plane_no < fHitMapsByPlane.size() - 1; plane_no++) {
        // Loops over planes to compare
        for (size_t second_plane_no = plane_no + 1; second_plane_no < fHitMapsByPlane.size(); second_plane_no++) {
            // Loop over all Maps (channels) of the first plane
            for (size_t map_index = 0; map_index < fHitMapsByPlane.at(plane_no).size(); map_index++) {
                // Loop over all Maps (channels) of the second comparison plane
                for (size_t second_map_index = 0;
                     second_map_index < fHitMapsByPlane.at(second_plane_no).size(); second_map_index++) {
                    // Loop over all hits in the first Map
                    for (const auto &Hits : fHitMapsByPlane.at(plane_no).at(map_index)) {
                        // Loop over all Hit iterators of the second compairison set
                        // that are found within the a time match difference of the first hit
                        for (auto Hit_Iter = fHitMapsByPlane.at(second_plane_no).at(second_map_index).lower_bound(
                                Hits.first - TimeMatchDifference);
                             Hit_Iter != fHitMapsByPlane.at(second_plane_no).at(second_map_index).end(); Hit_Iter++) {
                            // If the hit difference is bigger than the time match difference go out of the loop
                            if (std::abs(Hit_Iter->first - Hits.first) > TimeMatchDifference) {
                                break;
                            }
                            // If there are hits within this difference write them into a new selection of hits
                            NewHitMapsByPlane.at(plane_no).at(map_index).insert(Hits);
                            NewHitMapsByPlane.at(second_plane_no).at(second_map_index).insert(*Hit_Iter);
                        }// loop over hits in comparison map
                    }// loop over hits in the first map
                }// loop over other maps
            }// loop over all maps
        }// loop over other planes
    }// Loop over first two planes

    // Overwrite old data with filtered hits
    fHitMapsByPlane = NewHitMapsByPlane;
}


//-------------------------------------------------------------------------------------------------------------------

std::map<float, recob::Hit> lasercal::LaserHits::UPlaneHitFinder(const recob::Wire &SingleWire) {
    std::map<float, recob::Hit> LaserHits;

    if (fParameters.UseROI) {
        if (!fLaserROI.IsWireInRange(SingleWire)) {
            return LaserHits;
        }
    }

    // Set Numbers (-9999 for debugging purposes)
    int HitEnd = -9999;
    int HitStart = -9999;
    float Peak = -9999;
    int PeakTime = -9999;
    int HitIdx = 0;
    float PseudoArea = 0;

    // Extract Channel ID and raw signal from Wire object
    raw::ChannelID_t Channel = SingleWire.Channel();
    std::vector<float> Signal = SingleWire.Signal();


    // Set below threshold flag to false
    bool BelowThreshold = false;


    // loop over wire
    for (unsigned int sample = 0; sample < Signal.size(); sample++) {
        if (Signal.at(sample) <= fParameters.UHitThreshold) { // we expect a negatvie signal
            // If we go over the threshold the first time, save the time tick
            PseudoArea += abs(Signal.at(sample));
            if (!BelowThreshold) {
                HitStart = sample;
                BelowThreshold = true;
                Peak = Signal.at(sample);
                PeakTime = sample;
            }
            if (Signal.at(sample) < Peak) {
                Peak = Signal.at(sample);
                PeakTime = sample;
            }
        } else if (BelowThreshold && (Signal.at(sample) > fParameters.UHitThreshold || Signal.size() - 1 == sample)) {
            HitEnd = sample;
            BelowThreshold = false;

            if ((fabs(Peak) / (float) (HitEnd - HitStart) > fParameters.UAmplitudeToWidthRatio ||
                 fabs(Peak) > fParameters.HighAmplitudeThreshold)
                && HitEnd - HitStart > fParameters.UHitWidthThreshold) {
                // Create hit
                auto RecoHit = recob::HitCreator(SingleWire,
                                                 fGeometry->ChannelToWire(Channel).front(),
                                                 HitStart,
                                                 HitEnd,
                                                 fabs(HitStart - HitEnd) / 2,
                                                 (float) PeakTime,
                                                 fabs(HitStart - HitEnd) / 2,
                                                 Peak,
                                                 sqrt(Peak),
                                                 PseudoArea, // HitIntegral
                                                 1.,         // Sigma HitIntegral
                                                 1,
                                                 HitIdx,
                                                 1.,
                                                 0).move();

                if (fParameters.UseROI) {
                    if (fLaserROI.IsHitInRange(RecoHit)) {
                        LaserHits.emplace(std::make_pair((float) PeakTime, RecoHit));
                    }
                } else {
                    LaserHits.emplace(std::make_pair((float) PeakTime, RecoHit));
                }
                HitIdx++;
            }
            PseudoArea = 0; // reset area
        }
    }
    return LaserHits;
}

//-------------------------------------------------------------------------------------------------------------------

std::map<float, recob::Hit> lasercal::LaserHits::VPlaneHitFinder(const recob::Wire &SingleWire) {
    std::map<float, recob::Hit> LaserHits;


    if (fParameters.UseROI) {
        if (!fLaserROI.IsWireInRange(SingleWire)) {
            return LaserHits;
        }
    }

    // Set Numbers (-9999 for debugging purposes)
    int HitEnd = -9999;
    int HitStart = -9999;
    float Peak = -9999;
    float Dip = -9999;
    int PeakTime = -9999;
    int DipTime = -9999;
    float HitTime = -9999;
    int HitIdx = 0;
    float PseudoArea = 0;

    // Set all flags to false
    bool AboveThreshold = false;
    bool BelowThreshold = false;
    bool Handover_flag = false;

    // Extract Channel ID and raw signal from Wire object
    raw::ChannelID_t Channel = SingleWire.Channel();
    std::vector<float> Signal = SingleWire.Signal();


    // loop over wire
    for (unsigned int sample = 0; sample < Signal.size(); sample++) {
        if (!BelowThreshold && Signal.at(sample) >= fParameters.VHitThreshold) {
            PseudoArea += abs(Signal.at(sample));
            // If we go over the threshold the first time, save the time tick
            if (!AboveThreshold) {
                AboveThreshold = true;
                Handover_flag = false;
                HitStart = sample;
                Peak = Signal.at(sample);
                PeakTime = sample;
            }
            if (Signal.at(sample) > Peak) {
                Peak = Signal.at(sample);
                PeakTime = sample;
            }
        } else if (AboveThreshold && Signal.at(sample) < fParameters.VHitThreshold) {
            AboveThreshold = false;
            Handover_flag = true;
        }
        if (Handover_flag && !AboveThreshold && Signal.at(sample) <= -fParameters.VHitThreshold) {
            if (!BelowThreshold) {
                BelowThreshold = true;
                Dip = Signal.at(sample);
                DipTime = sample;
            }
            if (Signal.at(sample) < Dip) {
                Dip = Signal.at(sample);
                DipTime = sample;
            }
        } else if (Handover_flag && BelowThreshold &&
                   (Signal.at(sample) > -fParameters.VHitThreshold || Signal.size() - 1 == sample)) {
            HitEnd = sample;
            HitTime = (float) PeakTime + ((float) DipTime - (float) PeakTime) / 2;
            BelowThreshold = false;
            Handover_flag = false;

            if (((Peak - Dip) / (float) (HitEnd - HitStart) > fParameters.VAmplitudeToWidthRatio ||
                 Peak - Dip > fParameters.HighAmplitudeThreshold)
                && HitEnd - HitStart > fParameters.VHitWidthThreshold
                && (Peak / (float) (DipTime - PeakTime) > fParameters.VAmplitudeToRMSRatio ||
                    Peak - Dip > fParameters.HighAmplitudeThreshold)
                && DipTime - PeakTime > fParameters.VRMSThreshold) {
                // Create hit
                auto RecoHit = recob::HitCreator(SingleWire,
                                                 fGeometry->ChannelToWire(Channel).front(),
                                                 HitStart,
                                                 HitEnd,
                                                 fabs(DipTime - PeakTime) / 2,
                                                 HitTime,
                                                 fabs(DipTime - PeakTime) / 2,
                                                 Peak - Dip,
                                                 sqrt(Peak - Dip),
                                                 100.,
                                                 PseudoArea, // HitIntegral
                                                 1.,         // Sigma HitIntegral
                                                 1,
                                                 HitIdx,
                                                 1.,
                                                 0).move();

                if (fParameters.UseROI) {
                    if (fLaserROI.IsHitInRange(RecoHit)) {
                        LaserHits.emplace(std::make_pair((float) PeakTime, RecoHit));
                    }
                } else {
                    LaserHits.emplace(std::make_pair((float) PeakTime, RecoHit));
                }

                HitIdx++;
            }
            PseudoArea = 0;
        }
    }
    return LaserHits;
}

//-------------------------------------------------------------------------------------------------------------------

std::map<float, recob::Hit> lasercal::LaserHits::YPlaneHitFinder(const recob::Wire &SingleWire) {
    std::map<float, recob::Hit> LaserHits;

    // Abort the hit search if wire is not in the defined range

    if (fParameters.UseROI) {
        if (!fLaserROI.IsWireInRange(SingleWire)) {
            return LaserHits;
        }
    }


    // Set Numbers (-9999 for debugging purposes)
    int HitEnd = -9999;
    int HitStart = -9999;
    float Peak = -9999;
    int PeakTime = -9999;
    int HitIdx = 0;
    float PseudoArea = 0;

    // Extract Channel ID and raw signal from Wire object
    raw::ChannelID_t Channel = SingleWire.Channel();
    std::vector<float> Signal = SingleWire.Signal();


    // Set Above Threshold flag to false
    bool AboveThreshold = false;

    // loop over wire
    for (unsigned int sample = 0; sample < Signal.size(); sample++) {
        if (Signal.at(sample) >= fParameters.YHitThreshold) {
            PseudoArea += abs(Signal.at(sample));
            //std::cout << "----------- HERE ------------ c:" << sample << " " << Signal.at(sample) << std::endl;
            // If we go over the threshold the first time, save the time tick
            if (!AboveThreshold) {
                HitStart = sample;
                AboveThreshold = true;
                Peak = Signal.at(sample);
                PeakTime = sample;
            }
            if (Signal.at(sample) > Peak) {
                Peak = Signal.at(sample);
                PeakTime = sample;
            }
        } else if (AboveThreshold && (Signal.at(sample) < fParameters.YHitThreshold || Signal.size() - 1 == sample)) {
            HitEnd = sample;
            AboveThreshold = false;
            if ((Peak / (float) (HitEnd - HitStart) > fParameters.YAmplitudeToWidthRatio ||
                 Peak > fParameters.HighAmplitudeThreshold)
                && HitEnd - HitStart > fParameters.YHitWidthThreshold) {
                // Create hit
                auto RecoHit = recob::HitCreator(SingleWire,
                                                 fGeometry->ChannelToWire(Channel).front(),
                                                 HitStart,
                                                 HitEnd,
                                                 fabs(HitStart - HitEnd) / 2,
                                                 (float) PeakTime,
                                                 fabs(HitStart - HitEnd) / 2,
                                                 Peak,
                                                 sqrt(Peak),
                                                 PseudoArea,
                                                 1.,
                                                 1,
                                                 HitIdx,
                                                 1.,
                                                 0).move();

                if (fParameters.UseROI) {
                    if (fLaserROI.IsHitInRange(RecoHit)) {
                        LaserHits.emplace(std::make_pair((float) PeakTime, RecoHit));
                    }
                } else {
                    LaserHits.emplace(std::make_pair((float) PeakTime, RecoHit));
                }

                HitIdx++;
            }
            PseudoArea = 0;
        }
    }
    return LaserHits;
}

//-------------------------------------------------------------------------------------------------------------------
