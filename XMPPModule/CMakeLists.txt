# Define target name and output directory
init_target (XMPPModule OUTPUT modules/core)

MocFolder ()

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
file (GLOB XML_FILES *.xml)
#file (GLOB MOC_FILES XMPPModule.h)

AddSourceFolder (Client)

set (SOURCE_FILES ${SOURCE_FILES} ${CPP_FILES} ${H_FILES})
set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

# Qt4 Wrap 
QT4_WRAP_CPP(MOC_SRCS ${H_FILES})

configure_qxmpp()
use_package (QXMPP)


use_modules (XMPPModule Core Foundation Interfaces RexCommon ProtocolUtilities Scene Console Audio TundraLogicModule)

build_library (${TARGET_NAME} SHARED ${SOURCE_FILES} ${MOC_SRCS} ${UI_SRCS} ${RESOURCE_SRCS})

link_modules (Core Foundation Interfaces RexCommon ProtocolUtilities Scene Console Audio TundraLogicModule)
link_package (QXMPP)

SetupCompileFlagsWithPCH()
CopyModuleXMLFile()

final_target ()

