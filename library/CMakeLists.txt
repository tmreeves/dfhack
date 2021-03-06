PROJECT (dfapi)
cmake_minimum_required(VERSION 2.8)

## build options
OPTION(BUILD_DEVEL "Install/package files required for development (For SDK)." OFF)
OPTION(BUILD_DOXYGEN "Create/install/package doxygen documentation for DFHack (For SDK)." OFF)
IF(UNIX)
    OPTION(CONSOLE_NO_CATCH "Make the console not catch 'CTRL+C' events for easier debugging." OFF)
ENDIF()

include_directories (include)
include_directories (depends/md5)
include_directories (depends/tinyxml)
include_directories (depends/tthread)
include_directories (private)

SET(PROJECT_HDRS_INTERNAL
    private/ContextShared.h
    private/Internal.h
    private/wdirent.h
)

SET(PROJECT_HDRS
include/DFHack.h
include/dfhack/Console.h
include/dfhack/Core.h
include/dfhack/Error.h
include/dfhack/Export.h
include/dfhack/FakeSDL.h
include/dfhack/MiscUtils.h
include/dfhack/Module.h
include/dfhack/Pragma.h
include/dfhack/Process.h
include/dfhack/TileTypes.h
include/dfhack/Types.h
include/dfhack/Vector.h
include/dfhack/VersionInfo.h
include/dfhack/VersionInfoFactory.h
include/dfhack/Virtual.h
include/dfhack/extra/MapExtras.h
include/dfhack/extra/stopwatch.h
include/dfhack/extra/termutil.h
include/dfhack/modules/Buildings.h
include/dfhack/modules/Constructions.h
include/dfhack/modules/Creatures.h
include/dfhack/modules/Engravings.h
include/dfhack/modules/Gui.h
include/dfhack/modules/Items.h
include/dfhack/modules/Maps.h
include/dfhack/modules/Materials.h
include/dfhack/modules/Notes.h
include/dfhack/modules/Translation.h
include/dfhack/modules/Vegetation.h
include/dfhack/modules/Vermin.h
include/dfhack/modules/World.h
)

SET(PROJECT_SRCS
Core.cpp
PluginManager.cpp
TileTypes.cpp
VersionInfo.cpp
VersionInfoFactory.cpp
Virtual.cpp

depends/md5/md5.cpp
depends/md5/md5wrapper.cpp

depends/tinyxml/tinystr.cpp
depends/tinyxml/tinyxml.cpp
depends/tinyxml/tinyxmlerror.cpp
depends/tinyxml/tinyxmlparser.cpp

depends/tthread/tinythread.cpp

modules/Buildings.cpp
modules/Constructions.cpp
modules/Creatures.cpp
modules/Engravings.cpp
modules/Gui.cpp
modules/Items.cpp
modules/Maps.cpp
modules/Materials.cpp
modules/Notes.cpp
modules/Translation.cpp
modules/Vegetation.cpp
modules/Vermin.cpp
modules/World.cpp
)

SET(PROJECT_HDRS_LINUX
)

SET(PROJECT_HDRS_WINDOWS
)

SET(PROJECT_SRCS_LINUX
Console-linux.cpp
FakeSDL-linux.cpp
Process-linux.cpp
)

SET(PROJECT_SRCS_WINDOWS
Console-windows.cpp
FakeSDL-windows.cpp
Process-windows.cpp
)

IF(UNIX)
    LIST(APPEND PROJECT_HDRS ${PROJECT_HDRS_LINUX})
    LIST(APPEND PROJECT_SRCS ${PROJECT_SRCS_LINUX})
ELSE()
    LIST(APPEND PROJECT_HDRS ${PROJECT_HDRS_WINDOWS})
    LIST(APPEND PROJECT_SRCS ${PROJECT_SRCS_WINDOWS})
ENDIF()

SET_SOURCE_FILES_PROPERTIES( ${PROJECT_HDRS} PROPERTIES HEADER_FILE_ONLY TRUE )

LIST(APPEND PROJECT_SRCS ${PROJECT_HDRS})

ADD_DEFINITIONS(-DBUILD_DFHACK_LIB)

IF(UNIX)
    IF(CONSOLE_NO_CATCH)
        ADD_DEFINITIONS(-DCONSOLE_NO_CATCH)
    ENDIF()
ENDIF()

IF(UNIX)
  add_definitions(-DLINUX_BUILD)
  SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
  SET(CMAKE_CXX_FLAGS "-fvisibility=hidden -m32 -std=c++0x")
  SET(CMAKE_C_FLAGS "-fvisibility=hidden -m32")
  SET(PROJECT_LIBS rt )
ELSE()
  IF(MSVC)
    SET(PROJECT_LIBS psapi ${dfhack_SOURCE_DIR}/library/depends/ntdll/ntdll.lib)
  ENDIF()
ENDIF()

ADD_LIBRARY(dfhack SHARED ${PROJECT_SRCS})

IF(WIN32)
    SET_TARGET_PROPERTIES(dfhack PROPERTIES OUTPUT_NAME "SDL" )
ENDIF()

#effectively disables debug builds...
SET_TARGET_PROPERTIES(dfhack  PROPERTIES DEBUG_POSTFIX "-debug" )

TARGET_LINK_LIBRARIES(dfhack ${PROJECT_LIBS})

IF(UNIX)
    # On linux, copy our version of the df launch script which sets LD_PRELOAD
    install(PROGRAMS ${dfhack_SOURCE_DIR}/package/linux/dfhack
            DESTINATION .)
ELSE()
    # On windows, copy the renamed SDL so DF can still run.
    install(PROGRAMS ${dfhack_SOURCE_DIR}/package/windows/SDLreal.dll
            DESTINATION ${DFHACK_LIBRARY_DESTINATION})
ENDIF()

#install the main lib
install(TARGETS dfhack
        LIBRARY DESTINATION ${DFHACK_LIBRARY_DESTINATION}
        RUNTIME DESTINATION ${DFHACK_LIBRARY_DESTINATION})
#install the offset file
install(FILES ${dfhack_SOURCE_DIR}/Memory.xml
        DESTINATION ${DFHACK_DATA_DESTINATION}) #linux: share/dfhack

if(BUILD_DEVEL)
    if(WIN32)
        install(TARGETS dfhack
                ARCHIVE DESTINATION ${DFHACK_DEVLIB_DESTINATION})
    endif()
    # note the ending '/'. This means *contents* of the directory are installed
    # without the '/', the directory itself is installed
    install(DIRECTORY include/
            DESTINATION ${DFHACK_INCLUDES_DESTINATION}
            FILES_MATCHING PATTERN "*.h" ) #linux: include
    # Building the docs
    IF(BUILD_DOXYGEN)
        add_subdirectory (doc)
    ENDIF()
endif()
