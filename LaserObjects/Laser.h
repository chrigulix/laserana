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
    Laser(std::vector<lasercal::LaserTrack> Track);
    // Constructor using beam and only beam is filled
    Laser(std::vector<lasercal::LaserBeam> LaserBeam);
    
    std::vector<lasercal::LaserTrack> fLaserTracks;
    std::vector<lasercal::LaserBeam> fLaserBeams;
    
    // here beam and track are filled
    void AppendTrack(lasercal::LaserTrack);
    // only beam is filled
    void AppendBeam(lasercal::LaserBeam);
    
    lasercal::LaserTrack GetTrack(const unsigned long int&);
    lasercal::LaserBeam GetBeam(const unsigned long int&);
    
    unsigned long int GetNumberOfTracks();
    unsigned long int GetNumberOfBeams();
  };
}