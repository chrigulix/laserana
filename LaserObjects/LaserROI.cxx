#include "LaserObjects/LaserROI.h"


lasercal::LaserROI::LaserROI()
{
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
} // Default constructor

//-------------------------------------------------------------------------------------------------------------------

lasercal::LaserROI::LaserROI(const float& BoxSize, const lasercal::LaserBeam& LaserBeamInfo ) : fBoxSize ( BoxSize ), fLaserBeam ( LaserBeamInfo )
{
    std::cout << "Preparing " << std::endl;
    
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Get Detector properties
    detinfo::DetectorProperties const* DetProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();
    
    TVector3 EntryPoint = fLaserBeam.GetEntryPoint();
    TVector3 ExitPoint = fLaserBeam.GetExitPoint();
    
//     EntryPoint.Print();
//     ExitPoint.Print();
    
    TVector3 BeamVector = EntryPoint - ExitPoint;
    
    // Calculate box size in wire coordinates
    fWireBoxSize = BoxSize/fGeometry->WirePitch();
  
    // Calculate the scale factor of the ROI box x-coordinate
    fXScaleFactor = sqrt ( BeamVector.Mag2() ) *BoxSize/ fabs ( BeamVector[2] );
    
    // Get wire ranges for all planes
    auto RawRanges = WireRanges ( fLaserBeam.GetEntryPoint(),fLaserBeam.GetExitPoint() );
    
    // Initilize Extended ranges
    auto BoxRanges = RawRanges;
    
    // Prepare time range vector
    fRanges.resize(fGeometry->Nplanes());
    
    // Loop over all planes
    for(unsigned int plane_no = 0; plane_no < fGeometry->Nplanes(); plane_no++)
    {
	// The following stuff is complicated because the authors wanted to make the code work
	// without knowing where the laser is positioned, sorry but this is the way it is!
	
	// Initialize x-coordinate of exit and entry points
	float XEntry = EntryPoint[0];
	float XExit = ExitPoint[0];
	
	// If the wire number of the entry point is higher than the one of the exit point
	if(RawRanges.at(plane_no).first.Wire > RawRanges.at(plane_no).second.Wire)
	{
	    // Swap them if order is decending
	    std::swap(RawRanges.at(plane_no).first, RawRanges.at(plane_no).second);
	    // Also swap entry and exit points for right sign
	    std::swap(XEntry,XExit);
	}
	
	// If the entry point - box is in the wire plane range fill the new range point
	if((int)RawRanges.at(plane_no).first.Wire - (int)fWireBoxSize > 0)
	{
	    BoxRanges.at(plane_no).first.Wire = RawRanges.at(plane_no).first.Wire - (int)fWireBoxSize;
	}
	else // else fill minimum wire number = 0
	{
	    BoxRanges.at(plane_no).first.Wire = 0;
	}
	
	// If the exit point + box is in the wire plane range fill the new range point
	if((int)RawRanges.at(plane_no).second.Wire + (int)fWireBoxSize < fGeometry->Nwires(plane_no))
	{
	    BoxRanges.at(plane_no).second.Wire = RawRanges.at(plane_no).second.Wire + (int)fWireBoxSize;
	}
	else // else fill the maximum wire number
	
	{
	    BoxRanges.at(plane_no).second.Wire = fGeometry->Nwires(plane_no) - 1;
	}
	
	// Slope of beam in X (drift coordinate)
	float XSlope = (XExit - XEntry) / (RawRanges.at(plane_no).second.Wire - RawRanges.at(plane_no).first.Wire);
	
	// Loop over wire range
	for(unsigned int wire_no = BoxRanges.at(plane_no).first.Wire; wire_no <= BoxRanges.at(plane_no).second.Wire; wire_no++)
	{
	    // Linear function to calculate central x value in ROIBox 
	    float CentralX = XEntry + ((float)wire_no - (float)RawRanges.at(plane_no).first.Wire)*XSlope;
	    
	    // Fill x limits of this specific wire
	    std::pair<float,float> TickLimitsOfWire = std::make_pair(CentralX-fXScaleFactor,CentralX+fXScaleFactor);
	    
	    // Convert x coordinates to time ticks
	    TickLimitsOfWire.first = DetProperties->ConvertXToTicks(TickLimitsOfWire.first,plane_no,0,0);
	    TickLimitsOfWire.second = DetProperties->ConvertXToTicks(TickLimitsOfWire.second,plane_no,0,0);
	    
	    // Fill hit limit object
	    fRanges.at(plane_no).insert( std::make_pair(wire_no, std::move(TickLimitsOfWire)) );
	} // loop over wires
	
	// Fill entry point and exit point in wire coordinate
	fEntryWire.push_back(fRanges.at(plane_no).begin()->first);
	fExitWire.push_back(fRanges.at(plane_no).rbegin()->first);
    } // Loop over planes
} // Constructor using all wire signals and geometry purposes

//----------------------------------------------------------------------------------------------------------------

bool lasercal::LaserROI::IsWireInRange( const recob::Wire& WireToCheck ) const
{
    // Fetch WireID
    std::vector<geo::WireID> WireIDs = fGeometry->ChannelToWire(WireToCheck.Channel());
    
    unsigned int WireNo = WireIDs.front().Wire;
    unsigned int PlaneNo = WireIDs.front().Plane;
    
    // Check if wire number is in range for the given plane
    if( fRanges.at(PlaneNo).begin()->first <= WireNo && fRanges.at(PlaneNo).end()->first >= WireNo )
    {
	return true;
    }
    else // if not in inverval
    {
	return false;
    }
}

bool lasercal::LaserROI::IsHitInRange( const recob::Hit& HitToCheck ) const
{
    // Get wire information first
    unsigned int WireNo = HitToCheck.WireID().Wire;
    unsigned int PlaneNo = HitToCheck.WireID().Plane;
    
    // Check if wire number is in range for the given plane and if hit is inside of the interval of the given wire
    if( fRanges.at(PlaneNo).begin()->first <= WireNo && fRanges.at(PlaneNo).rbegin()->first >= WireNo &&
	fRanges.at(PlaneNo).find(WireNo)->second.first <= HitToCheck.PeakTime() && fRanges.at(PlaneNo).find(WireNo)->second.second >= HitToCheck.PeakTime() )
    {
	return true;
    }
    else // if wire or time tick of hit is not in inverval
    {
	return false;
    }
    
}

//-----------------------------------------------------------------------------------------------------------
unsigned int lasercal::LaserROI::GetEntryWire(const unsigned int& PlaneNo) const
{
    return fEntryWire.at(PlaneNo);
}

//----------------------------------------------------------------------------------------------------------------

unsigned int lasercal::LaserROI::GetExitWire(const unsigned int& PlaneNo) const
{
    return fExitWire.at(PlaneNo);
}

//----------------------------------------------------------------------------------------------------------------

float lasercal::LaserROI::GetEntryTimeTick(const unsigned int& PlaneNo) const
{
    return 0.5*(fRanges.at(PlaneNo).find(fEntryWire.at(PlaneNo))->second.first + fRanges.at(PlaneNo).find(fExitWire.at(PlaneNo))->second.second);
}

//----------------------------------------------------------------------------------------------------------------

float lasercal::LaserROI::GetExitTimeTick(const unsigned int& PlaneNo) const
{
    return 0.5*(fRanges.at(PlaneNo).find(fExitWire.at(PlaneNo))->second.first + fRanges.at(PlaneNo).find(fExitWire.at(PlaneNo))->second.second);
}

//----------------------------------------------------------------------------------------------------------------

std::vector< std::pair<geo::WireID, geo::WireID> > lasercal::LaserROI::WireRanges( const TVector3& StartPosition, const TVector3& EndPosition )
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
