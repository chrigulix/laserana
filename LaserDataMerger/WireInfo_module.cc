#ifndef WireInfor_Module
#define WireInfor_Module


// LArSoft includes
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Cluster.h"
#include "lardata/RecoBase/Wire.h"
#include "lardata/RecoBase/Track.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
//#include "larcore/SimpleTypesAndConstants/geo_types.h"

// Framework includes
#include "art/Utilities/Exception.h"
// #include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Persistency/Common/Ptr.h"

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
#include <fstream>
//#include <SimpleTypesAndConstants/geo_types.h>

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace {

} // local namespace


namespace WireInfor {

  class WireInfor : public art::EDAnalyzer
  {
  public:
 
    explicit WireInfor(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;
    
    virtual void endJob() override;

    virtual void beginRun(const art::Run& run) override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    virtual void analyze(const art::Event& event) override;
    
    void printArray(double[], int);
        

  private:
      geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
      
      std::vector< std::pair<int, int> > fWires;
    
  }; // class WireInfor
  
  DEFINE_ART_MODULE(WireInfor)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  WireInfor::WireInfor(fhicl::ParameterSet const& pset) : EDAnalyzer(pset)
  {    
      fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
  }

  
  //-----------------------------------------------------------------------
  void WireInfor::beginJob()
  {

  }
  
  void  WireInfor::endJob()
  {
  }
   
  //-----------------------------------------------------------------------
  void WireInfor::beginRun(const art::Run& /*run*/)
  {

  }

  //-----------------------------------------------------------------------
  void WireInfor::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
      fWires = parameterSet.get< std::vector< std::pair<int, int> > >("Wires");
  }

  //-----------------------------------------------------------------------
  void WireInfor::analyze(const art::Event& event) 
  {
      geo::TPCID::TPCID_t tpc = 0;
      geo::CryostatID::CryostatID_t cryostat = 0;
      
      for (auto wire : fWires){
        std::cout << "Plane: " << wire.first << " Wire: " << wire.second << std::endl;
         
        auto planeid = geo::PlaneID(cryostat, tpc, wire.first);
        auto wireid = geo::WireID(planeid, wire.second);
        
        int size = 3;
        double Start[size], End[size];
        
        fGeometry->WireEndPoints(wireid, Start, End);
        std::cout << " Start: ";
        printArray(Start, size);
        std::cout << " End:   ";
        printArray(End, size);
      }
      std::cout << "\n\n";
  } // WireInfor::analyze()

  
  void WireInfor::printArray(double arr[], int size) {
    std::cout << "[";
    for ( int i = 0; i < size; i++ ) {
        
        std::cout << arr[i] << ' ';
    }
    std::cout << "]" << std::endl;
}


} // namespace WireInfor

#endif // WireInfor_Module