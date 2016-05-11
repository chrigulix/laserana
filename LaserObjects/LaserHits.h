#ifndef LaserObjects_LaserHits_H
#define LaserObjects_LaserHits_H

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

namespace LaserObjects
{
  class LaserHits
  {
    public:
      // Constructor with geometry and thresholds for the hit finder. 
      // It just initializes the object. There is no hit finding or filling of data.
      LaserHits(const geo::GeometryCore* Geometry, const std::array<float,3>& UVYThresholds);
      
      // Constructor wire data, geometry and thresholds for the hit finder.
      // It already runs the hit finder algorithms and fills the map data.
      LaserHits(const std::vector<recob::Wire>& Wires, const geo::GeometryCore* Geometry, const std::array<float,3>& UVYThresholds);
      
      void AddHitsFromWire(const recob::Wire& Wire);
      
      const std::array<size_t,3> NumberOfWiresWithHits();
      
      // Clear all data out
      void clear();
      
      // Get all hits of a certain plane
      std::unique_ptr< std::vector<recob::Hit> > GetPlaneHits(size_t PlaneIndex);
      
      // Remove hits without time match
      void TimeMatchFilter();
      
    private:
      
      // Hit data member, it is an array for all planes cantainig vectors with all wire entries
      std::array< std::vector<std::map<float, recob::Hit>>, 3 > fHitMapsByPlane;
      const geo::GeometryCore* fGeometry;
      std::array<float,3> fUVYThresholds;
      
      // Single wire hit finder which fills hits into a map with hit time as a key
      std::map<float, recob::Hit> FindSingleWireHits(const recob::Wire& Wire, unsigned int Plane);
      
      // Hit finder algorithms for different wire planes
      std::map<float, recob::Hit> UPlaneHitFinder(const recob::Wire& SingleWire);
      std::map<float, recob::Hit> VPlaneHitFinder(const recob::Wire& SingleWire);
      std::map<float, recob::Hit> YPlaneHitFinder(const recob::Wire& SingleWire);
    
  }; // class LaserHits
  
} // namespace LaserOjects

#endif // LaserObjects_LaserHits_H