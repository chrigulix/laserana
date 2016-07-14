#ifndef WireInfo_Module
#define WireInfo_Module


// LArSoft includes
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "larcore/Geometry/Geometry.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"

// C++ Includes
#include <fstream>

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace {

} // local namespace


namespace WireInfo {

  class WireInfo : public art::EDAnalyzer
  {
  public:
 
    explicit WireInfo(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;
    
    virtual void endJob() override;

    virtual void beginRun(const art::Run& run) override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    virtual void analyze(const art::Event& event) override;
    
    void printArray(double[], int);
        

  private:
      geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
      
      std::vector< std::pair<unsigned int, unsigned int> > fWires;
    
  }; // class WireInfo
  
  DEFINE_ART_MODULE(WireInfo)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  WireInfo::WireInfo(fhicl::ParameterSet const& pset) : EDAnalyzer(pset)
  {    
      fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
  }

  
  //-----------------------------------------------------------------------
  void WireInfo::beginJob()
  {

  }
  
  void  WireInfo::endJob()
  {
  }
   
  //-----------------------------------------------------------------------
  void WireInfo::beginRun(const art::Run& /*run*/)
  {

  }

  //-----------------------------------------------------------------------
  void WireInfo::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
      fWires = parameterSet.get< std::vector< std::pair<unsigned int, unsigned int> > >("Wires");
  }

  //-----------------------------------------------------------------------
  void WireInfo::analyze(const art::Event& event)
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
  } // WireInfo::analyze()

  
  void WireInfo::printArray(double arr[], int size) {
    std::cout << "[";
    for ( int i = 0; i < size; i++ ) {
        
        std::cout << arr[i] << ' ';
    }
    std::cout << "]" << std::endl;
}


} // namespace WireInfo

#endif // WireInfor_Module