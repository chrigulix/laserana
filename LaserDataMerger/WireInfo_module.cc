#ifndef WireInfo_Module
#define WireInfo_Module


// LArSoft includes
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"
#include "lardata/ArtDataHelper/WireCreator.h"
#include "lardata/ArtDataHelper/HitCreator.h"
#include "larcore/Geometry/Geometry.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"

// C++ Includes
#include <fstream>

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace {

} // local namespace


namespace WireInfo {

    class WireInfo : public art::EDAnalyzer {
    public:

        explicit WireInfo(fhicl::ParameterSet const &parameterSet);

        virtual void beginJob() override;

        virtual void endJob() override;

        virtual void beginRun(const art::Run &run) override;

        virtual void reconfigure(fhicl::ParameterSet const &parameterSet) override;

        virtual void analyze(const art::Event &event) override;

        void printArray(double[], int);


    private:
        geo::GeometryCore const *fGeometry;       ///< pointer to Geometry provider

        std::vector<std::pair<unsigned int, unsigned int> > fWires;

    }; // class WireInfo



    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // class implementation

    //-----------------------------------------------------------------------
    // Constructor
    WireInfo::WireInfo(fhicl::ParameterSet const &pset) : EDAnalyzer(pset) {
        fGeometry = &*(art::ServiceHandle<geo::Geometry>());
        // Read in the parameters from the .fcl file.
        this->reconfigure(pset);
    }


    //-----------------------------------------------------------------------
    void WireInfo::beginJob() {

    }

    void  WireInfo::endJob() {
    }

    //-----------------------------------------------------------------------
    void WireInfo::beginRun(const art::Run & /*run*/) {

    }

    //-----------------------------------------------------------------------
    void WireInfo::reconfigure(fhicl::ParameterSet const &parameterSet) {
        fWires = parameterSet.get<std::vector<std::pair<unsigned int, unsigned int> > >("Wires");
    }

    //-----------------------------------------------------------------------
    void WireInfo::analyze(const art::Event &event) {
        geo::TPCID::TPCID_t tpc = 0;
        geo::CryostatID::CryostatID_t cryostat = 0;

        TVector3 pt(0, 60.8, 125.7);
        std::cout << "Nearest point to x: " << pt.x() << " y: " << pt.y() << " z: " << pt.z() << std::endl;
        for (uint plane = 0; plane < 3; plane++) {
            auto wire = fGeometry->NearestWire(pt, plane);
            std::cout << " plane: " << plane << " wire: " << wire << std::endl;
        }
        double a[6];
        fGeometry->CryostatBoundaries(a);
        auto len = fGeometry->DetLength();
        auto wid = fGeometry->DetHalfWidth();
        auto hih = fGeometry->DetHalfHeight();

        std::cout << "dims: " << " " << a[0]<< " "  << a[1]<< " "  << a[2]<< " "  << a[3]<< " "  << a[4]<< " "  << a[5]<< " "  << std::endl;

        for (auto wire : fWires) {

            auto planeid = geo::PlaneID(cryostat, tpc, wire.first);
            auto wireid = geo::WireID(planeid, wire.second);

            int size = 3;
            double Start[size], End[size];
            auto channelid = fGeometry->PlaneWireToChannel(wireid);
            fGeometry->WireEndPoints(wireid, Start, End);

            std::cout << "Plane: " << wire.first << " Wire: " << wire.second << " channel: " << channelid << std::endl;
            std::cout << " Start: ";
            printArray(Start, size);
            std::cout << " End:   ";
            printArray(End, size);
        }
        std::cout << "\n\n";
    } // WireInfo::analyze()


    void WireInfo::printArray(double arr[], int size) {
        std::cout << "[";
        for (int i = 0; i < size; i++) {

            std::cout << arr[i] << ' ';
        }
        std::cout << "]" << std::endl;
    }

    DEFINE_ART_MODULE(WireInfo)
} // namespace WireInfo

#endif // WireInfor_Module
