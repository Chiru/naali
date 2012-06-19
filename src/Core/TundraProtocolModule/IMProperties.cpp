/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 */
 
#include "IMProperties.h"

IMProperties::IMProperties()
{
    enabled_ = false;
    relevance_mode_ = false;
    raycast_mode_ = false;
    euclidean_mode_ = false;
    max_range_ = 40;
    critical_range_ = 20;
    raycast_range_ = 20;
    update_interval_ = 1000000000;
}

IMProperties::~IMProperties()
{

}

bool IMProperties::GetEuclideanMode()             { return euclidean_mode_; }
bool IMProperties::GetRelevanceMode()             { return relevance_mode_; }
bool IMProperties::GetRaycastMode()               { return raycast_mode_;   }
double IMProperties::GetMaxRange()                { return max_range_;      }
double IMProperties::GetCriticalRange()           { return critical_range_; }
int IMProperties::GetUpdateInterval()             { return update_interval_;}
double IMProperties::GetRaycastRange()            { return raycast_range_;  }

void IMProperties::SetEuclideanMode(bool mode)    { euclidean_mode_ = mode; }
void IMProperties::SetRelevanceMode(bool mode)    { relevance_mode_ = mode; }
void IMProperties::SetRaycastMode(bool mode)      { raycast_mode_ = mode;   }
void IMProperties::SetMaximumRange(double r)      { max_range_ = r;         }
void IMProperties::SetCriticalRange(double r)     { critical_range_ = r;    }
void IMProperties::SetUpdateInterval(int i)       { update_interval_ = i;   }
void IMProperties::SetRaycastRange(double r)      { raycast_range_ = r;     }

bool IMProperties::isEnabled()                    { return enabled_;        }
void IMProperties::setEnabled(bool mode)          { enabled_ = mode;        }
