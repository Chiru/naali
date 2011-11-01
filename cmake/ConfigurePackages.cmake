# =============================================================================
# per-dependency configuration macros
#
# All per-dependency configuration (or hacks) should go here. All per-module
# build instructions should go in <Module>/CMakeLists.txt. The rest should
# remain generic.

macro (configure_boost)
    # boost filesystem version used
    # Disabled, as boost filesystem no longer used. If taken back in use, enable.
    # add_definitions(-DBOOST_FILESYSTEM_VERSION=2)

    if (MSVC)
        set(Boost_USE_MULTITHREADED TRUE)
        set(Boost_USE_STATIC_LIBS TRUE)
    else ()
        set(Boost_USE_STATIC_LIBS FALSE)
    endif ()

    if (UNIX)
        set (BOOST_COMPONENTS boost_thread boost_regex)
    else ()
        set (BOOST_COMPONENTS thread regex)
    endif ()
 
    # Set BOOST_ROOT to our deps if not defined so the FindBoost.cmake 
    # macro is used in a any situation inside sagase_configure_package().
    if (MSVC AND "${ENV_BOOST_ROOT}" STREQUAL "")
        if (WIN32)
            # Fallback to the deps boost if a overriding env variable is not set. 
            SET (BOOST_ROOT ${ENV_TUNDRA_DEP_PATH}/Boost)
        else () 
            # For linux etc. boost is found from system. If not it will anyway
            # fallback to brute force search to ENV_TUNDRA_DEP_PATH like before.
            SET (BOOST_ROOT "")
        endif ()
    else ()
        SET (BOOST_ROOT ${ENV_BOOST_ROOT})
    endif()
    
    # BOOST_ROOT
    sagase_configure_package (BOOST 
        NAMES Boost boost
        COMPONENTS ${BOOST_COMPONENTS}
        PREFIXES ${BOOST_ROOT} ${ENV_TUNDRA_DEP_PATH})

    if (APPLE)
        set (BOOST_LIBRARY_DIRS ${ENV_TUNDRA_DEP_PATH}/lib)
        set (BOOST_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/include)
    endif()

    # Setting the BOOST_ROOT will result in linking failures on Visual Studio as found release and debug
    # libs get mixed up. On windows we will empty out the Boost_LIBRARIES list and count on the 
    # auto-linking feature boost provides. http://www.boost.org/doc/libs/1_35_0/more/getting_started/windows.html#auto-linking
    if (MSVC)
        # Reset libraries list so VC will perform auto-linking.
        set (BOOST_LIBRARIES "")
        # Not needed anymore as BOOST_ROOT finds these properly. 
        # Didnt remove yet eiher so nothing breaks, added empty checks instead to not add duplicated.
        if ("${BOOST_INCLUDE_DIRS}" STREQUAL "")
            set (BOOST_INCLUDE_DIRS ${BOOST_INCLUDE_DIRS} $ENV{BOOST_ROOT}/include)
        endif ()
        if ("${BOOST_LIBRARY_DIRS}" STREQUAL "")
            set (BOOST_LIBRARY_DIRS ${BOOST_LIBRARY_DIRS} $ENV{BOOST_ROOT}/lib)
        endif ()
    endif ()

    sagase_configure_report (BOOST)
endmacro (configure_boost)

macro (configure_qt4)
    sagase_configure_package (QT4 
        NAMES Qt4 4.6.1
        COMPONENTS QtCore QtGui QtWebkit QtScript QtScriptTools QtXml QtNetwork QtUiTools QtDeclarative
        PREFIXES ${ENV_TUNDRA_DEP_PATH} ${ENV_QT_DIR})

    # FindQt4.cmake
    if (QT4_FOUND AND QT_USE_FILE)
    
        include (${QT_USE_FILE})
        
        set (QT4_INCLUDE_DIRS 
            ${QT_INCLUDE_DIR}
            ${QT_QTCORE_INCLUDE_DIR}
            ${QT_QTGUI_INCLUDE_DIR}
            ${QT_QTUITOOLS_INCLUDE_DIR}
            ${QT_QTNETWORK_INCLUDE_DIR}
            ${QT_QTXML_INCLUDE_DIR}
            ${QT_QTSCRIPT_INCLUDE_DIR}
            ${QT_DECLARATIVE_INCLUDE_DIR}
            ${QT_QTWEBKIT_INCLUDE_DIR})
            
#            ${QT_QTSCRIPTTOOLS_INCLUDE_DIR}
#            ${QT_PHONON_INCLUDE_DIR}

        
        set (QT4_LIBRARY_DIR  
            ${QT_LIBRARY_DIR})
        
        set (QT4_LIBRARIES 
            ${QT_LIBRARIES}
            ${QT_QTCORE_LIBRARY}
            ${QT_QTGUI_LIBRARY}
            ${QT_QTUITOOLS_LIBRARY}
            ${QT_QTNETWORK_LIBRARY}
            ${QT_QTXML_LIBRARY}
            ${QT_QTSCRIPT_LIBRARY}
            ${QT_DECLARATIVE_LIBRARY}
            ${QT_QTWEBKIT_LIBRARY})
            
#            ${QT_QTSCRIPTTOOLS_LIBRARY}
#            ${QT_PHONON_LIBRARY}
        
    endif ()
    
    sagase_configure_report (QT4)
endmacro (configure_qt4)

macro (configure_python)
    sagase_configure_package (PYTHON
        NAMES PythonLibs Python python Python26 python26 Python2.6 python2.6
        COMPONENTS Python python Python26 python Python2.6 python2.6
        PREFIXES ${ENV_TUNDRA_DEP_PATH})

    # FindPythonLibs.cmake
    if (PYTHONLIBS_FOUND)
        set (PYTHON_LIBRARIES ${PYTHON_LIBRARY})
        set (PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_PATH})
        #unset (PYTHON_DEBUG_LIBRARIES ${PYTHON_DEBUG_LIBRARY})
    endif ()
    
    # FindPythonLibs.cmake prefers the system-wide Python, which does not
    # include debug libraries, so we force to TUNDRA_DEP_PATH.
    if (MSVC)
        set (PYTHON_LIBRARY_DIRS ${ENV_TUNDRA_DEP_PATH}/Python/lib)
        set (PYTHON_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/Python/include)
        set (PYTHON_LIBRARIES python26)
        set (PYTHON_DEBUG_LIBRARIES python26_d)
    endif()
    
    sagase_configure_report (PYTHON)
endmacro (configure_python)

macro (configure_python_qt)
    sagase_configure_package (PYTHON_QT
        NAMES PythonQt
        COMPONENTS PythonQt PythonQt_QtAll
        PREFIXES ${ENV_TUNDRA_DEP_PATH})

    sagase_configure_report (PYTHON_QT)
endmacro (configure_python_qt)

macro (configure_skyx)
    # Prioritize env variable SKYX_HOME to be searched first
    # to allow custom skyx builds agains a custom ogre (potentially from OGRE_HOME)
    sagase_configure_package (SKYX
      NAMES SkyX SKYX skyx
      COMPONENTS SkyX SKYX skyx
      PREFIXES ${ENV_SKYX_HOME} ${ENV_TUNDRA_DEP_PATH})

    if (NOT WIN32)
      set (SKYX_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/include/SkyX)
    endif ()
    
    sagase_configure_report (SKYX)
endmacro (configure_skyx)

macro (configure_hydrax)
    # Prioritize env variable HYDRAX_HOME to be searched first
    # to allow custom hydrax builds agains a custom ogre (potentially from OGRE_HOME)
    sagase_configure_package (HYDRAX
        NAMES Hydrax HYDRAX hydrax
        COMPONENTS Hydrax HYDRAX hydrax
        PREFIXES ${ENV_HYDRAX_HOME} ${ENV_TUNDRA_DEP_PATH})

    if (NOT WIN32)
       set (HYDRAX_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/include/Hydrax)
    endif ()

    sagase_configure_report (HYDRAX)
endmacro (configure_hydrax)

macro (configure_qtpropertybrowser)
    sagase_configure_package (QT_PROPERTY_BROWSER
        NAMES QtPropertyBrowser QtSolutions_PropertyBrowser-2.5
        COMPONENTS QtPropertyBrowser QtSolutions_PropertyBrowser-2.5
        PREFIXES ${ENV_TUNDRA_DEP_PATH})
    
    sagase_configure_report (QT_PROPERTY_BROWSER)
endmacro (configure_qtpropertybrowser)

macro (configure_openal)
    sagase_configure_package(OPENAL
        NAMES OpenAL openal
        COMPONENTS al OpenAL32
        PREFIXES ${ENV_TUNDRA_DEP_PATH}/OpenAL ${ENV_TUNDRA_DEP_PATH}/OpenAL/libs/Win32)

        if (OPENAL_FOUND)
            set (OPENAL_LIBRARIES ${OPENAL_LIBRARY})
            set (OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
        endif()

        # Force include dir on MSVC
        if (MSVC)
             set (OPENAL_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/OpenAL/include)
        endif ()
    sagase_configure_report (OPENAL)
endmacro (configure_openal)

macro (configure_ogg)
    sagase_configure_package(OGG
        NAMES ogg libogg
        COMPONENTS ogg libogg
        PREFIXES ${ENV_TUNDRA_DEP_PATH}/libogg)
        
        # Force include dir on MSVC
        if (MSVC)
             set (OGG_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/libogg/include)
        endif ()
    sagase_configure_report (OGG)
endmacro (configure_ogg)

macro (configure_vorbis)
if (APPLE)
    sagase_configure_package(VORBIS
        NAMES vorbisfile vorbis libvorbis libvorbisfile
        COMPONENTS vorbis libvorbis vorbisfile libvorbisfile
        PREFIXES ${ENV_TUNDRA_DEP_PATH}/libvorbis)
else()
    sagase_configure_package(VORBIS
        NAMES vorbisfile vorbis libvorbis
        COMPONENTS vorbis libvorbis libvorbisfile
        PREFIXES ${ENV_TUNDRA_DEP_PATH}/libvorbis)
endif()
        # Force include dir on MSVC
        if (MSVC)
             set (VORBIS_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/libvorbis/include)
        endif ()
    sagase_configure_report (VORBIS)
endmacro (configure_vorbis)

macro (configure_theora)
    sagase_configure_package(THEORA
        NAMES theora libtheora
        COMPONENTS theora libtheora
        PREFIXES ${ENV_TUNDRA_DEP_PATH}/libtheora)
        
        # Force include dir on MSVC
        if (MSVC)
             set (THEORA_INCLUDE_DIRS ${ENV_TUNDRA_DEP_PATH}/libtheora/include)
        endif ()
    sagase_configure_report (THEORA)
endmacro (configure_theora)

macro (configure_sparkle)
    FIND_LIBRARY (SPARKLE_LIBRARY NAMES Sparkle)
    set (SPARKLE_INCLUDE_DIRS ${SPARKLE_LIBRARY}/Headers)
    set (SPARKLE_LIBRARIES ${SPARKLE_LIBRARY})
endmacro (configure_sparkle)

macro(use_package_knet)
    message ("** Configuring KNET")
    
    # Use KNET_DIR_QT47 if there, fallback to TUNDRA_DEP_PATH
    if ("${ENV_KNET_DIR_QT47}" STREQUAL "")
        set (KNET_DIR ${ENV_TUNDRA_DEP_PATH}/kNet)
    else ()
        message (STATUS "-- Using from env variable KNET_DIR_QT47")
        set (KNET_DIR ${ENV_KNET_DIR_QT47})
    endif ()
    
    # Report findings
    include_directories (${KNET_DIR}/include)
    message (STATUS "-- Include Directories:")
    message (STATUS "       " ${KNET_DIR}/include)
    link_directories (${KNET_DIR}/lib)
    message (STATUS "-- Library Directories:")
    message (STATUS "       " ${KNET_DIR}/lib)
    message (STATUS "-- Libraries:")
    message (STATUS "       kNet")
    message ("")
    
    if (UNIX)    
        add_definitions (-DUNIX)
    endif()
endmacro()

macro(link_package_knet)
    target_link_libraries(${TARGET_NAME} optimized kNet)
    target_link_libraries(${TARGET_NAME} debug kNet)
endmacro()

macro(use_package_bullet)
    # todo: convert to sagase_configure_package and sagase_report or custom FindBullet.cmake
    message(STATUS "todo: fix use_package_bullet() from hardcoded one to more robust using sagase.")
    if (WIN32)
        if ("${ENV_BULLET_DIR}" STREQUAL "")
            set(BULLET_DIR ${ENV_TUNDRA_DEP_PATH}/Bullet)
        else ()
            message (STATUS "-- Using from env variable BULLET_DIR")
            set(BULLET_DIR ${ENV_BULLET_DIR})
        endif ()
        include_directories(${BULLET_DIR}/include)
        link_directories(${BULLET_DIR}/lib)
    else() # Linux, note: mac will also come here..
        if ("${ENV_BULLET_DIR}" STREQUAL "")
            set(BULLET_DIR ${ENV_TUNDRA_DEP_PATH})
        else ()
            message (STATUS "-- Using from env variable BULLET_DIR")
            set(BULLET_DIR ${ENV_BULLET_DIR})
        endif ()
        include_directories(${BULLET_DIR}/include/bullet)
        link_directories(${BULLET_DIR}/lib)
    endif()
endmacro()

macro(link_package_bullet)
    target_link_libraries(${TARGET_NAME} optimized LinearMath optimized BulletDynamics optimized BulletCollision)
    if (WIN32)
        target_link_libraries(${TARGET_NAME} debug LinearMath_d debug BulletDynamics_d debug BulletCollision_d)
    endif()
endmacro()

macro(use_package_assimp)
    # todo: convert to sagase_configure_package and sagase_report or custom FindAssimp.cmake
    message(STATUS "todo: fix use_package_assimp() from hardcoded one to more robust using sagase.")
    if (WIN32)
        if ("${ENV_ASSIMP_DIR}" STREQUAL "")
           set(ASSIMP_DIR ${ENV_TUNDRA_DEP_PATH}/assimp)
        else ()
            message (STATUS "-- Using from env variable ASSIMP_DIR")
            set(ASSIMP_DIR ${ENV_ASSIMP_DIR})
        endif()
        include_directories(${ASSIMP_DIR}/include)
        link_directories(${ASSIMP_DIR}/lib/assimp_debug_Win32)
        link_directories(${ASSIMP_DIR}/lib/assimp_release_Win32)
    else() # Linux, note: mac will also come here..
        if ("${ENV_ASSIMP_DIR}" STREQUAL "")
            set(ASSIMP_DIR ${ENV_TUNDRA_DEP_PATH})
        else ()
            message (STATUS "-- Using from env variable ASSIMP_DIR")
            set(ASSIMP_DIR ${ENV_ASSIMP_DIR})
        endif()
        include_directories(${ASSIMP_DIR}/include/assimp)
        link_directories(${ASSIMP_DIR}/lib)
    endif()
endmacro()

macro(link_package_assimp)
    target_link_libraries(${TARGET_NAME} optimized assimp)
    if (WIN32)
        target_link_libraries(${TARGET_NAME} debug assimpd)
    endif()
endmacro()
