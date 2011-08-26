# Define target name and output directory
init_target (EC_Menu)

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
file (GLOB MOC_FILES EC_MenuContainer.h MenuDataItem.h MenuDataModel.h EC_MenuItem.h)

set (SOURCE_FILES ${CPP_FILES} ${H_FILES})
#set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

# Qt4 Wrap
QT4_WRAP_CPP(MOC_SRCS ${MOC_FILES})

use_package_bullet()
use_modules (Core Foundation Interfaces Ui Input Scene Asset OgreRenderingModule TundraLogicModule RexCommon PhysicsModule)

build_library (${TARGET_NAME} STATIC ${SOURCE_FILES} ${MOC_SRCS} )

LinkEntityComponent(EntityComponents/EC_3DCanvas EC_3DCanvas)
LinkEntityComponent(EntityComponents/EC_Mesh EC_Mesh)
LinkEntityComponent(EntityComponents/EC_Light EC_Light)
LinkEntityComponent(EntityComponents/EC_Name EC_Name)

link_ogre()
link_package_bullet()
link_modules (Core Foundation Interfaces Ui Input Scene Asset OgreRenderingModule TundraLogicModule RexCommon PhysicsModule)

SetupCompileFlagsWithPCH()

final_target ()
