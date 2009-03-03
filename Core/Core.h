// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __incl_Core_h__
#define __incl_Core_h__

// Add all includes here that contain interfaces for the Core
#include "CoreCompileConfig.h"
#include "CoreDefines.h"
#include "CoreTypes.h"

#include "CoreMath.h"
#include "Vector3D.h"
#include "Matrix4.h"
#include "Quaternion.h"


//! Core contains functionality and definitions that are common to all projects in the viewer.
/*! It contains standard includes so you shouldn't need to include specific
    C++ standard library includes in your own code. It also contains
    includes for some of the external libraries used by the viewer, such as
    Boost or PoCo.

    Useful macros are defined, such as LOG(), so adding log statements is
    possible even if we might still lack an actual logger.

    For more information about useful defines, see CoreDefines.h

    Some common types are also specified by the Core, such as shortcuts
    to unsigned int or boost mutex lock guard.

    For more information see CoreTypes.h
*/
namespace Core {}

#endif

