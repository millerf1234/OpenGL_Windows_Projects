
//Need to have an entire implementation file so I can initialize the one static member variable...


#include "Timepoint.h"


std::multiset<Timepoint, std::less<Timepoint>> Timepoint::mMasterTimepointRecord_;
