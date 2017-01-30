#ifndef lasercal_LaserROI_H
#define lasercal_LaserROI_H

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/Geometry/Geometry.h"

#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

#include "LaserObjects/LaserBeam.h"

#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <array>
#include <memory>

namespace lasercal
{
  class LaserROI
  {
    public:
      // Constructor with geometry and thresholds for the hit finder. 
      // It just initializes the object. There is no hit finding or filling of data.
      LaserROI();
      
      // Constructor wire data, geometry and thresholds for the hit finder.
      // It already runs the hit finder algorithms and fills the map data.
      LaserROI(const float& BoxSize, const lasercal::LaserBeam& LaserBeamInfo);

      /**
      * @brief Checks if wire is within rage defined in fRanges
      * @param Single wire to be checked
      * @return True if wire is within range, false if wire is not
      */
      bool IsWireInRange(const recob::Wire& WireToCheck) const;

      /**
      * @brief Checks if hit peaking time is within rage defined in fRanges
      * @param Single hit to be checked
      * @return True if wire is within range, false if wire is not
      */
      bool IsHitInRange(const recob::Hit& HitToCheck) const;

      /// Sets Range range to check directely.
      void setRanges(int BoxTickCenter, int BoxTickWidth, unsigned int Plane, std::pair<unsigned int, unsigned int> Wires);

      /// Sets Range range to check directely.
      std::vector< std::map< unsigned int, std::pair<float, float> > > GetRanges();

      unsigned int GetEntryWire(const unsigned int& PlaneNo) const;
      unsigned int GetExitWire(const unsigned int& PlaneNo) const;
      
      float GetEntryTimeTick(const unsigned int& PlaneNo) const;
      float GetExitTimeTick(const unsigned int& PlaneNo) const;
      
    private:
      
      // Detector geometry object
      const geo::GeometryCore* fGeometry;
      float fBoxSize;
      float fWireBoxSize;
      lasercal::LaserBeam fLaserBeam;
      float fXScaleFactor;
      
      std::vector<unsigned int> fEntryWire;
      std::vector<unsigned int> fExitWire;
      
      // Time tick ranges of ROI for all planes(vector) and every individual wire (map)
      std::vector< std::map< unsigned int, std::pair<float, float> > > fRanges;
      
    protected:
      
      // Calculates wire number range of the laser beam
      std::vector< std::pair<geo::WireID, geo::WireID> > WireRanges(const TVector3& StartPosition, const TVector3& EndPosition);
      
      // Calculate
      
    
  }; // class LaserHits
  
} // namespace lasercal

#endif // LaserObjects_LaserHits_H
