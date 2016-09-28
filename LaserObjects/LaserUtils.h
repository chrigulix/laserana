//
// Created by matthias on 21.07.16.
//

#ifndef LASER_LASERUTILS_H
#define LASER_LASERUTILS_H

#endif //LASER_LASERUTILS_H

#include "fhiclcpp/ParameterSet.h"

#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"

#include "art/Framework/Principal/Event.h"

#include "lardata/RawData/RawDigit.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Wire.h"

#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "LaserParameters.h"

namespace lasercal
{
    std::vector<recob::Wire> GetWires(art::ValidHandle<std::vector<raw::RawDigit>> &DigitVecHandle,
                                      lasercal::LaserRecoParameters &fParameterSet);
}