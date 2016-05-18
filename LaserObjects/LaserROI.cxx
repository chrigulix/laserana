#include "LaserObjects/LaserROI.h"


lasercal::LaserROI::LaserROI ( const geo::GeometryCore* Geometry, const float& BoxSize ) : fGeometry ( Geometry ), fBoxSize ( BoxSize )
{

} // Default constructor

//-------------------------------------------------------------------------------------------------------------------

lasercal::LaserROI::LaserROI ( const geo::GeometryCore* Geometry, const float& BoxSize, const lasercal::LaserBeam& LaserBeamInfo ) : fGeometry ( Geometry ), fBoxSize ( BoxSize ), fLaserBeam ( LaserBeamInfo )
{
    // Calculate box size in wire coordinates
    fWireBoxSize = BoxSize/fGeometry->WirePitch();
    
    TVector3 DistanceVector = fLaserBeam.GetEntryPoint() - fLaserBeam.GetExitPoint();
  
    // Calculate the scale factor of the ROI box x-coordinate
    fXScaleFactor = sqrt ( DistanceVector.Mag2() ) *BoxSize/fabs ( DistanceVector[2] );
    
    // Get wire ranges 
    auto RawRanges = WireRanges ( fLaserBeam.GetEntryPoint(),fLaserBeam.GetExitPoint() );
    
    // Initilize Extended ranges
    auto ExtendedRanges = RawRanges;
    
    for(unsigned int plane_no = 0; plane_no < fGeometry->Nplanes(); plane_no++)
    {
	// The following stuff is complicated because the authors wanted to make the code work
	// without knowing where the laser is positioned, sorry but this is the way it is!
	
	// If the wire number of the entry point is higher than the one of the exit point
	if(RawRanges.at(plane_no).first.Wire > RawRanges.at(plane_no).second.Wire)
	{
	    // If the entry point + box is in the wire plane range fill the new range point
	    if(RawRanges.at(plane_no).first.Wire + (int)fWireBoxSize < fGeometry->Nwires(plane_no))
	    {
		ExtendedRanges.at(plane_no).first.Wire = RawRanges.at(plane_no).first.Wire + (int)fWireBoxSize;
	    }
	    else // else fill the maximum wire number
	    {
		ExtendedRanges.at(plane_no).first.Wire = fGeometry->Nwires(plane_no) - 1;
	    }
	    // If the exit point - box is still in wire range fill new range point
	    if(RawRanges.at(plane_no).second.Wire - (int)fWireBoxSize > 0)
	    {
		ExtendedRanges.at(plane_no).second.Wire = RawRanges.at(plane_no).second.Wire - (int)fWireBoxSize;
	    }
	    else // else fill minimum wire number = 0
	    {
		ExtendedRanges.at(plane_no).second.Wire = 0;
	    }
	}
	else // If the exit point has the higher wire number than the entry point
	{
	    // If the entry point - box is in the wire plane range fill the new range point
	    if(RawRanges.at(plane_no).first.Wire - (int)fWireBoxSize > 0)
	    {
		ExtendedRanges.at(plane_no).first.Wire = RawRanges.at(plane_no).first.Wire - (int)fWireBoxSize;
	    }
	    else // else fill minimum wire number = 0
	    {
		ExtendedRanges.at(plane_no).first.Wire = 0;
	    }
	    // If the exit point + box is in the wire plane range fill the new range point
	    if(RawRanges.at(plane_no).second.Wire + (int)fWireBoxSize < fGeometry->Nwires(plane_no))
	    {
		ExtendedRanges.at(plane_no).second.Wire = RawRanges.at(plane_no).second.Wire + (int)fWireBoxSize;
	    }
	    else // else fill the maximum wire number
	    {
		ExtendedRanges.at(plane_no).second.Wire = fGeometry->Nwires(plane_no) - 1;
	    }
	}
    }


} // Constructor using all wire signals and geometry purposes

//----------------------------------------------------------------------------------------------------------------

std::vector< std::pair<geo::WireID, geo::WireID> > lasercal::LaserROI::WireRanges ( const TVector3& StartPosition, const TVector3& EndPosition )
{
  // Initialize start and end point of wire
  double Start[] = {StartPosition[0],StartPosition[1],StartPosition[2]};
  double End[] = {EndPosition[0],EndPosition[1],EndPosition[2]};

  // Initialize the return pair vector
  std::vector< std::pair<geo::WireID, geo::WireID> > CrossingWireRangeVec;

  for(unsigned int plane_no = 0; plane_no < fGeometry->Nplanes(); plane_no++)
  {
      // Generate PlaneID for microboone (cryostatID = 0, TPCID = 0, plane number)
      auto TargetPlaneID = geo::PlaneID ( 0,0,plane_no );

      // Get the closest wire on the target plane to the wire start position
      auto FirstWireID = fGeometry->NearestWireID ( Start, TargetPlaneID );

      // Get the closest wire on the target plane to the wire end postion
      auto LastWireID = fGeometry->NearestWireID ( End, TargetPlaneID );
      
      // First and last wire ID into vector
      CrossingWireRangeVec.push_back ( std::make_pair ( FirstWireID,LastWireID ) );
  }
  
  // Return the entr and exit wire IDs as a pair
  return CrossingWireRangeVec;
}
