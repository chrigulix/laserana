/**
 * @file   LaserTrack.h
 * @brief  Provides a base class for laser analysis purpose
 * @author Christoph Rudolf von Rohr (crohr@fnal.gov)
 * @date   August 19th, 2015
 */

/// C/C++ standard library
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <string>
#include <iomanip>

/// Root library
#include <TH3.h>
#include <TVector3.h>
#include <TF1.h>

/// Larsoft library
#include "LaserBeam.h"

#ifndef LASERTRACK_H
#define LASERTRACK_H

namespace lasercal
{
  /**
   * @brief Provides a base class for laser analysis purpose
   * 
   * 
   */
  class LaserTrack : public lasercal::LaserBeam // (could also inherit from recob::track)
  {
    protected:
      std::vector<TVector3> fReconstructed;
      std::vector<TVector3> fCorrection;
  

    public:
      
      /**
     * @brief Default constructor: sets an empty volume
     * @see SetPosition
     * @see SetDirection
     * 
     * We allow for a default construction since the derived object might need
     * some specific action before being aware of the laser properties.
     * In that case, SetPosition() and SetDirection() will set the properties.
     */
      LaserTrack();
      
      /**
     * @brief Constructor: sets laser position and laser direction
     * @param LaserPosition start position of the laser 
     * @param LaserDirection direction of the laser beam
     * @see SetPosition
     * @see SetDirection
     
     * This constructor loads the start position and direction of the laser beam
     */
      LaserTrack(TVector3& LaserPosition, TVector3& LaserDirection);
      
      
      
      /**
     * @brief Sets laser Position
     * @param LaserPosition start position of the laser 
     */
      void SetPosition(TVector3& LaserPosition);
      
     /**
     * @brief Sets laser direction
     * @param LaserDirection direction of the laser beam
     */
      void SetDirection(TVector3& LaserDirection);
      
      void CorrectTrack(unsigned int MethodNumber);
      void AddToCorrection(TVector3&, unsigned long);
  
      std::array<float,2> GetAngles();
      unsigned long GetNumberOfSamples() const;
      TVector3 GetDirection();
      TVector3 GetSamplePosition(const unsigned int&) const;
      TVector3 GetCorrection(const unsigned int&) const;
      void AppendSample(TVector3&);
      void AppendSample(float,float,float);
      void AppendSample(TVector3&,TVector3&);
      void AppendSample(float,float,float,float,float,float);
//       LaserTrack(std::array<float,2>&, TVector3&, TPCVolumeHandler&);
//       LaserTrack(const unsigned int,std::array<float,2>&, TVector3&, TPCVolumeHandler&);
//       void DistortTrack(std::string, TPCVolumeHandler&);  
//       static void DistortTracks(std::vector<LaserTrack>&, const std::string&, TPCVolumeHandler&);  
  };
}

#endif