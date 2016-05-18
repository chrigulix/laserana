#include "LaserObjects/LaserROI.h"


lasercal::LaserROI::LaserROI(const geo::GeometryCore* Geometry, const float& BoxSize) : fGeometry(Geometry), fBoxSize(BoxSize)
{
  
} // Default constructor

//-------------------------------------------------------------------------------------------------------------------

lasercal::LaserROI::LaserROI(const geo::GeometryCore* Geometry, const float& BoxSize, const lasercal::LaserBeam& LaserBeamInfo) : fGeometry(Geometry), fBoxSize(BoxSize), fLaserBeam(LaserBeamInfo)
{
  TVector3 DistanceVector = fLaserBeam.GetEntryPoint() - fLaserBeam.GetExitPoint();
  
  // Calculate the scale factor of the ROI box x-coordinate
  fXScaleFactor = sqrt(DistanceVector.Mag2())*BoxSize/fabs(DistanceVector[2]);
  
} // Constructor using all wire signals and geometry purposes

//----------------------------------------------------------------------------------------------------------------

std::pair<geo::WireID, geo::WireID> lasercal::LaserROI::WireRanges(TVector3 StartPosition, TVector3 EndPosition, geo::PlaneID::PlaneID_t PlaneID)
{
  // Initialize start and end point of wire
  double Start[] = {StartPosition[0],StartPosition[1],StartPosition[2]}; 
  double End[] = {EndPosition[0],EndPosition[1],EndPosition[2]};
  
  // Generate PlaneID for microboone (cryostatID = 0, TPCID = 0, plane number)
  auto TargetPlaneID = geo::PlaneID(0,0,PlaneID);
  
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
  
  // Return the entr and exit wire IDs as a pair
  return std::make_pair(FirstWireID,LastWireID);
}
