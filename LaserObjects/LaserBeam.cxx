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
    // Load detector properties
    detinfo::DetectorProperties const *DetProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();

    // Load geometry core

    //geo::GeometryCore const* Geometry = &*(art::ServiceHandle<geo::Geometry>());
    auto const* Geometry = lar::providerFrom<geo::Geometry>();

    // Create the active Volume
    geo::BoxBoundedGeo ActiveVolume( 0,2*Geometry->TPC().ActiveHalfWidth(), // xmin, xmax
				     -Geometry->TPC().ActiveHalfHeight(),Geometry->TPC().ActiveHalfHeight(), // ymin, ymax  
				     0,Geometry->TPC().ActiveLength() // zmin, zmax 
				    );
    
    // Get the active volume and its intersection points
    auto IntersectionPoints = ActiveVolume.GetIntersections(fLaserPosition, fDirection);

    // Check if there is only an exit point (if laser head is in TPC volume)
    if(IntersectionPoints.size() == 0)
    {
	fEntryPoint = TVector3(0,0,0);
	fExitPoint = TVector3(0,0,0);
    }
    else
    {
	fEntryPoint = IntersectionPoints.front();
	fExitPoint = IntersectionPoints.back();
    }

    // Entry ticks should be the same on all wires
    fEntryTick = (uint) DetProperties->ConvertXToTicks(fEntryPoint.X(), geo::PlaneID(0, 0, 0));
    fExitTick = (uint) DetProperties->ConvertXToTicks(fExitPoint.X(), geo::PlaneID(0, 0, 0));

    // Fill entry wires for all planes based on the calculated entry and exit points
    for (unsigned int plane = 0; plane < Geometry->Nplanes(); plane++) {
        auto plane_id = geo::PlaneID(0, 0, plane);
        auto entry_wire = Geometry->NearestWire(fEntryPoint, plane_id);
        auto exit_wire = Geometry->NearestWire(fExitPoint, plane_id);

        fEntryWire.push_back(geo::WireID(plane_id, entry_wire));
        fExitWire.push_back(geo::WireID(plane_id, exit_wire));
    }
}

void lasercal::LaserBeam::Print() const 
{
    std::cout   << "Laser System        " << fLaserID << "\n"
                << "Laser Event ID      " << fLaserEventID << "\n"
                << "Associate Event ID  " << fAssosiateEventID << "\n"
                << "Attenuator Position " << fPower * 100 << " %\n"
                << "Aperture Position   " << fAperturePosition << "\n";
    std::cout   << "Mirror Position     "; fLaserPosition.Print();
    std::cout   << "Mirror Direction    "; fDirection.Print();
    std::cout   << "EntryPoint          "; fEntryPoint.Print();
    std::cout   << "ExitPoint           "; fExitPoint.Print();
    std::cout   << "EntryTick           " << fEntryTick << std::endl;
    std::cout   << "ExitTick            " << fExitTick  << std::endl;

    std::cout  << std::endl;
}

TVector3 lasercal::LaserBeam::GetEntryPoint() const
{
    return fEntryPoint;
}

TVector3 lasercal::LaserBeam::GetExitPoint() const
{
    return fExitPoint;
}

TVector3 lasercal::LaserBeam::GetLaserDirection() const
{
    return fDirection;
}

TVector3 lasercal::LaserBeam::GetLaserPosition() const
{
    return fLaserPosition;
}

const std::vector<geo::WireID> &lasercal::LaserBeam::getEntryWire() const {
    return fEntryWire;
}

const geo::WireID &lasercal::LaserBeam::getEntryWire(uint plane_id) const {
    return fEntryWire.at(plane_id);
}

const std::vector<geo::WireID> &lasercal::LaserBeam::getExitWire() const {
    return fExitWire;
}

const geo::WireID &lasercal::LaserBeam::getExitWire(uint plane_id) const {
    return fExitWire.at(plane_id);
}

uint lasercal::LaserBeam::getEntryTick() const {
    return fEntryTick;
}

uint lasercal::LaserBeam::getExitTick() const {
    return fExitTick;
}
