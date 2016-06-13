#ifndef lasercal_LaserParameters_H
#define lasercal_LaserParameters_H

#include  "art/Utilities/InputTag.h"

#include <string>

// This structure contains all ficl parameters which are needed for the laser calibration.
// It is a simple way to pass all the parameters to the classes

namespace lasercal
{
    struct LaserRecoParameters
    {
	// Activate the Wire Map generator (LaserReco/LaserDataMerger)
	bool WireMapGenerator;
	
	// Activate/deactivate ROI in LaserHits
	bool LaserROIFlag;
	
	// Minimum channel status number (DetectorProperties)
	int MinAllowedChanStatus;
	
	// Peak threshold for hits on different planes (LaserHits/HitAna)
	float UHitThreshold;
	float VHitThreshold;
	float YHitThreshold;
	
	// Hit amplitude-to-width ratio
	float UAmplitudeToWidthRatio;
	float VAmplitudeToWidthRatio;
	float YAmplitudeToWidthRatio;
	
	// Hit amplitude-to-width ratio
	int UHitWidthThreshold;
	int VHitWidthThreshold;
	int YHitWidthThreshold;
	
	// Special V-plane hit cuts
	int VRMSThreshold;
	float VAmplitudeToRMSRatio;
	
	// High amplitude threshold (for all planes the same)
	float HighAmplitudeThreshold;
	
	// Hit box size (LaserROI)
	float HitBoxSize;
	
	// Input tag for raw digits (LaserReco)
	art::InputTag RawDigitTag;
	
	// Laser beam tag construction
	std::string LaserDataMergerModuleLabel;
	std::string LaserBeamInstanceLabel;
	
	art::InputTag GetLaserBeamTag()
	{
	    return art::InputTag(LaserDataMergerModuleLabel, LaserBeamInstanceLabel);
	}
	
    }; // struct
    
    
    // Module label for laser hits of every plane
// 	std::string HitModuleLabel;
	
	// Instance names for laser hits of every plane
// 	std::string UPlaneHitInstanceLabel;
// // 	std::string VPlaneHitInstanceLabel;
// 	std::string YPlaneHitInstanceLabel;

// Get input tag for U-plane laser hits
// 	art::InputTag GetUPlaneHitTag(HitModuleLabel,UPlaneHitInstanceLabel)
// 	{
// 	    return art::InputTag(HitModuleLabel,UPlaneHitInstanceLabel);
// 	}
	
	// Get input tag for V-plane laser hits
// 	art::InputTag GetVPlaneHitTag(HitModuleLabel,VPlaneHitInstanceLabel)
// 	{
// 	    return art::InputTag(HitModuleLabel,VPlaneHitInstanceLabel);
// 	}
	
	// Get input tag for Y-plane laser hits
// 	art::InputTag GetYPlaneHitTag(HitModuleLabel,YPlaneHitInstanceLabel)
// 	{
// 	    return art::InputTag(HitModuleLabel,YPlaneHitInstanceLabel);
// 	}

} // namespace



#endif