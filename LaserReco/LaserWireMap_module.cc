// LaserWireMap_module.cc

#ifndef LaserWireMap_Module
#define LaserWireMap_Module

// LArSoft includes
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"

#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "larcore/CoreUtils/ServiceUtil.h"

// Framework includes
#include "art/Utilities/Exception.h"
#include "art/Utilities/Exception.h"
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Persistency/Common/Ptr.h"

// uBooNE includes
#include "lardata/Utilities/AssociationUtil.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

// #include "uboone/Utilities/SignalShapingServiceMicroBooNE.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"

// ROOT includes. Note: To look up the properties of the ROOT classes,
// use the ROOT web site; e.g.,
// <http://root.cern.ch/root/html532/ClassIndex.html>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TFile.h"

// C++ Includes
#include <map>
#include <vector>
#include <array>
#include <string>
#include <cmath>
#include <typeinfo>
#include <utility>
#include <memory>
#include <iterator>

// Laser Module Classes
#include "LaserObjects/LaserHits.h"
#include "LaserObjects/LaserUtils.h"
#include "LaserObjects/LaserBeam.h"
#include "LaserObjects/LaserParameters.h"

namespace {

} // local namespace


namespace LaserWireMap {

    class LaserWireMap : public art::EDProducer {
    public:
        explicit LaserWireMap(fhicl::ParameterSet const &parameterSet);

        virtual void beginJob() /*override*/;

        virtual void endJob() /*override*/;

        virtual void reconfigure(fhicl::ParameterSet const &parameterSet) /*override*/;

        // The analysis routine, called once per event.
        virtual void produce(art::Event &event) /*override*/;

    private:
        geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider

        std::string fFileName = "WireIndexMap.root";

        std::map<unsigned int, unsigned int> UMap;
        std::map<unsigned int, unsigned int> VMap;
        std::map<unsigned int, unsigned int> YMap;

    }; // class LaserWireMap



    // Constructor
    LaserWireMap::LaserWireMap(fhicl::ParameterSet const &pset) {
        fGeometry = &*(art::ServiceHandle<geo::Geometry>());
        this->reconfigure(pset);

    }


    //-----------------------------------------------------------------------
    void LaserWireMap::beginJob() {
            }


    void LaserWireMap::endJob() {
        TFile* OutputFile = new TFile(fFileName.c_str(), "RECREATE");

        OutputFile->WriteObject(&UMap,"UMap");
        OutputFile->WriteObject(&VMap,"VMap");
        OutputFile->WriteObject(&YMap,"YMap");
    }

//-----------------------------------------------------------------------

    void LaserWireMap::reconfigure(fhicl::ParameterSet const &parameterSet) {

    }

    //-----------------------------------------------------------------------
    void LaserWireMap::produce(art::Event &event) {
        art::ValidHandle<std::vector<raw::RawDigit> > DigitVecHandle = event.getValidHandle<std::vector<raw::RawDigit>>("daq");

        unsigned int Index = 0;
        for (auto const &RawDigit : *DigitVecHandle) {
            raw::ChannelID_t channel = RawDigit.Channel();
            if (fGeometry->ChannelToWire(channel).front().Plane == 2) // If wire plane is Y-plane
            {
                YMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
            } else if (fGeometry->ChannelToWire(channel).front().Plane == 1) // If wire plane is V-plane
            {
                VMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
            } else if (fGeometry->ChannelToWire(channel).front().Plane == 0) // If wire plane is U-plane
            {
                UMap[fGeometry->ChannelToWire(channel).front().Wire] = Index;
            }
        }
    } // LaserWireMap::analyze()

    DEFINE_ART_MODULE(LaserWireMap)
} // namespace LaserWireMap

#endif // LaserWireMap_Module
