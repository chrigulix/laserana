#include "LaserBeam.h"

lasercal::LaserBeam::LaserBeam()
{
}

lasercal::LaserBeam::LaserBeam(const TVector3& LaserPosition, const TVector3& LaserDirection) : fLaserPosition(LaserPosition), fDirection(LaserDirection)
{
     // Calculate the intersection points with the TPC. This has to be done after SetPosition and SetDirection!
    SetIntersectionPoints();
}

lasercal::LaserBeam::LaserBeam(const TVector3& LaserPosition, const float& Phi, const float& Theta)
{
    
    // Set position and direction
    SetPosition(LaserPosition, false);
    SetDirection(Phi,Theta);
    
    // Calculate the intersection points with the TPC. This has to be done after SetPosition and SetDirection!
//     SetIntersectionPoints();
    
}

void lasercal::LaserBeam::SetPosition(const TVector3& LaserPosition, const bool& ReCalcFlag)
{
    fLaserPosition = LaserPosition;
    
    if(ReCalcFlag)
    {
	// Re-calculate the intersection points with the TPC.
	SetIntersectionPoints();
    }
}

void lasercal::LaserBeam::SetDirection(const TVector3& LaserDirection, const bool& ReCalcFlag)
{
    fDirection = LaserDirection;
    
    if(ReCalcFlag)
    {
	// Re-calculate the intersection points with the TPC.
	SetIntersectionPoints();
    }
}

void lasercal::LaserBeam::SetDirection(const float& Phi, const float& Theta, const bool& ReCalcFlag)
{
    TVector3 LaserDir;          // 3-Vector in laser coordinate system
    TVector3 LaserDirection;    // 3-Vector in uboone coordinate system 
    
    // Transform from measured angles to uboone coordinate system.
    LaserDir.SetMagThetaPhi(1., Theta, Phi);
    LaserDirection.SetX(LaserDir.Y());
    LaserDirection.SetY(LaserDir.Z());
    LaserDirection.SetZ(LaserDir.X());
    
    // Set directions
    SetDirection(LaserDirection,ReCalcFlag);
}


void lasercal::LaserBeam::SetPower(const float& AttenuatorPercentage) 
{
    fPower = AttenuatorPercentage;
}

void lasercal::LaserBeam::SetTime(const float& sec, const float& usec) 
{
    fTime.sec = (unsigned long) sec;
    fTime.usec = (unsigned long) usec;
}

// TODO: Fix class compilation if including GeometryCore.h and others!!!
void lasercal::LaserBeam::SetIntersectionPoints()
{   
    // Load geometry core
    geo::GeometryCore const* Geometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Create the active Volume
    geo::BoxBoundedGeo ActiveVolume( 0,2*Geometry->TPC().ActiveHalfWidth(), // xmin, xmax
				     -Geometry->TPC().ActiveHalfHeight(),Geometry->TPC().ActiveHalfHeight(), // ymin, ymax  
				     0,Geometry->TPC().ActiveLength() // zmin, zmax 
				    );
    
    // Get the active volume and its intersection points
    auto IntersectionPoints = ActiveVolume.GetIntersections(fLaserPosition, fDirection);

    // Check if there is only an exit point (if laser head is in TPC volume)
    if(IntersectionPoints.size() == 1)
    {
	fEntryPoint = TVector3(-9999,-9999,-9999);
	fExitPoint = IntersectionPoints.back();
    }
    else
    {
	fEntryPoint = IntersectionPoints.front();
	fExitPoint = IntersectionPoints.back();
    }
}

void lasercal::LaserBeam::Print() const 
{
    std::cout   << "Laser System        " << fLaserID << "\n"
                << "Laser Event ID      " << fLaserEventID << "\n"
                << "Associate Event ID  " << fAssosiateEventID << "\n"
                << "Attenuator Position " << fPower * 100 << " %\n"
                << "Aperture Position   " << fAperturePosition << "\n"
                << "Mirror Position     ";// << std::endl; 
    fLaserPosition.Print();
    std::cout    << "Mirror Direction    ";// << std::endl;
    fDirection.Print();
    std::cout << "\n" << std::endl;
}

TVector3 lasercal::LaserBeam::GetEntryPoint() const
{
  return fEntryPoint;
}

TVector3 lasercal::LaserBeam::GetExitPoint() const
{
  return fExitPoint;
}