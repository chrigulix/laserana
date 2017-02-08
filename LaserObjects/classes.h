//
// Build a dictionary.
//

// Original author Rob Kutschke
//

#include "LaserBeam.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

/*
namespace {
    struct dictionary {
        lasercal::Time tm;
        lasercal::LaserBeam la;
        art::Wrapper<lasercal::LaserBeam> laser;
    };
}
*/

//template class std::vector< lasercal::LaserBeam >;
//template class art::Wrapper< lasercal::LaserBeam >;
//template class art::Wrapper< std::vector<lasercal::LaserBeam> >;

