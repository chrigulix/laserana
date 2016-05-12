#ifndef lasercal_LaserROI_H
#define lasercal_LaserROI_H

#include "larcore/SimpleTypesAndConstants/RawTypes.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Wire.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "larcore/Geometry/GeometryCore.h"

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
      LaserROI(const geo::GeometryCore* Geometry, const std::array<float,3>& UVYThresholds);
      
      // Constructor wire data, geometry and thresholds for the hit finder.
      // It already runs the hit finder algorithms and fills the map data.
      LaserROI(const std::vector<recob::Wire>& Wires, const geo::GeometryCore* Geometry, const std::array<float,3>& UVYThresholds);
      
    private:
      
      // Detector geometry object
      const geo::GeometryCore* fGeometry;
      
      // Calculates wire number range of the laser beam
      std::vector< std::pair<geo::WireID, geo::WireID> > WireRanges(geo::WireID WireID);
      
    
  }; // class LaserHits
  
} // namespace lasercal

#endif // LaserObjects_LaserHits_H