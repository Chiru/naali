# Define target name and output directory
init_target (QMLUIModule OUTPUT modules/core)

MocFolder ()

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
file (GLOB XML_FILES *.xml)
file (GLOB QML_FILES qml/*.qml)
file (GLOB MOC_FILES QMLUIModule.h QMLWidget.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

# Qt4 Wrap
QT4_WRAP_CPP(MOC_SRCS ${MOC_FILES})

use_modules (Core Foundation Interfaces Input UiServiceModule MobilityModule)

build_library (${TARGET_NAME} SHARED ${SOURCE_FILES} ${MOC_SRCS} ${QML_FILES})

link_modules (Core Foundation Interfaces Input UiServiceModule MobilityModule)
link_ogre()
link_package_qtdeclarative()

SetupCompileFlagsWithPCH()
CopyModuleXMLFile()
CopyModuleQMLFiles()

final_target ()
