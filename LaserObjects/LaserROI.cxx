#include "LaserObjects/LaserROI.h"


lasercal::LaserROI::LaserROI(const geo::GeometryCore* Geometry, const std::array<float,3>& UVYThresholds)
{
  fGeometry = Geometry;
} // Default constructor

//-------------------------------------------------------------------------------------------------------------------

lasercal::LaserROI::LaserROI(const std::vector<recob::Wire>& Wires, const geo::GeometryCore* Geometry, const std::array<float,3>& UVYThresholds)
{
  fGeometry = Geometry;
} // Constructor using all wire signals and geometry purposes

//----------------------------------------------------------------------------------------------------------------

std::vector< std::pair<geo::WireID, geo::WireID> > lasercal::LaserROI::WireRanges(geo::WireID WireID)
{
  // Initialize the return pair vector
  std::vector< std::pair<geo::WireID, geo::WireID> > CrossingWireRangeVec;

  // Initialize start and end point of wire
  double Start[3], End[3];
  
  // Fill the wire End points
  fGeometry->WireEndPoints(WireID, Start, End);
  
  // Loop over target plane number
  for(unsigned int plane_no = 0; plane_no < 3; plane_no++)
  {
    // Search for crossing wires only if the aren't on the same plane
    if(plane_no != WireID.Plane)
    {
      // Generate PlaneID for microboone (cryostatID = 0, TPCID = 0, plane number)
      auto TargetPlaneID = geo::PlaneID(0,0,plane_no);
	
      // Get the closest wire on the target plane to the wire start position
      auto FirstWireID = fGeometry->NearestWireID(Start, TargetPlaneID);
      
      // Get the closest wire on the target plane to the wire end postion
      auto LastWireID = fGeometry->NearestWireID(End, TargetPlaneID);
	
      // Check if wires are in right order
      if(FirstWireID.Wire > LastWireID.Wire)
      {
	// Swap them if order is decending
	std::swap(FirstWireID, LastWireID);
      }
      
      // Generate a dummy intersection coordinate, because shit only works that way (eye roll)
      geo::WireIDIntersection DummyMcDumbFace;
	
      // Check if first and last wires are crossing, if not take one next to them
      if( !fGeometry->WireIDsIntersect(WireID, FirstWireID, DummyMcDumbFace) )
      {
	++(FirstWireID.Wire);
      }
      
      if( !fGeometry->WireIDsIntersect(WireID, LastWireID, DummyMcDumbFace) )
      {
	--(LastWireID.Wire);
      }
      CrossingWireRangeVec.push_back( std::make_pair(FirstWireID,LastWireID) );
    }
  }
  return CrossingWireRangeVec;
}
