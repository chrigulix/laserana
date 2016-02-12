#include "LaserBeam.h"
#include "LaserTrack.h"

namespace lasercal
{
  /**
   * @brief Provides a base class for laser analysis purpose
   * 
   * 
   */
  class Laser
  {
    Laser();
    // Constructor using track (this includes already beam)
    Laser(std::vector<laser::LaserTrack> Track);
    // Constructor using beam and only beam is filled
    Laser(std::vector<laser::Beam> Beam);
    
    std::vector<laser::LaserTrack> fLaserTracks;
    std::vector<laser::Beam> fLaserBeams;
    
    // here beam and track are filled
    void AppendTrack(laser::LaserTrack);
    // only beam is filled
    void AppendBeam(laser::Beam);
    
    laser::LaserTrack GetTrack(const unsigned long int&);
    laser::Beam GetBeam(const unsigned long int&);
    
    unsigned long int GetNumberOfTracks();
    unsigned long int GetNumberOfBeams();
  };
}