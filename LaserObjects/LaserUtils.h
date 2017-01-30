//
// Created by matthias on 21.07.16.
//

#ifndef LASER_LASERUTILS_H
#define LASER_LASERUTILS_H

#endif //LASER_LASERUTILS_H

#include "fhiclcpp/ParameterSet.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"

#include "art/Framework/Principal/Event.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Wire.h"

#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "LaserParameters.h"

#include <boost/tokenizer.hpp>
#include <fstream>

#include "canvas/Utilities/Exception.h"

struct  RawDigitDefinition{
    enum Value {
        Plane,
        Wire,
        CenterTick,
        Width,
        Amplitude,
        Offset,
        Multiplicity
    };
};

namespace lasercal
{
    std::vector<recob::Wire> GetWires(art::ValidHandle<std::vector<raw::RawDigit>> &DigitVecHandle,
                                      lasercal::LaserRecoParameters &fParameterSet,
                                      bool SubstractPedestal=true);

    std::unique_ptr<std::vector<std::vector<std::vector<float> > > > ReadHitDefs(std::string Filename, bool DEBUG = false);
}
