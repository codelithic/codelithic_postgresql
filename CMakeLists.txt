cmake_minimum_required(VERSION 3.5)
project(pgmod)

# set(CMAKE_MACOSX_RPATH OFF)
set (CMAKE_CXX_STANDARD 17)


# FIND_PACKAGE(PostgreSQL)
# include_directories($PostgreSQL_INCLUDE_DIRS)
# link_directories($PostgreSQL_LIBRARIES)


include_directories("include")

SET(MAIN_FILE
        include/postgresql.hpp
        include/PersistenceEntity.hpp source/PersistenceEntity.cpp
        include/querymask.hpp
        include/connectionfactory.hpp

		)

add_library(pgmod  STATIC ${MAIN_FILE}   )
TARGET_LINK_LIBRARIES(pgmod pq z)
		

# set(PERSISTENCE_VERSION_MAJOR 1)
# set(PERSISTENCE_VERSION_MINOR 1)
# set(PERSISTENCE_VERSION_PATCH 1)
# set(PERSISTENCE_VERSION ${LSD_VERSION_MAJOR}.${LSD_VERSION_MINOR}.${LSD_VERSION_PATCH})

# add_library(t2pcpersistence STATIC ${MAIN_FILE})
# set_target_properties(t2pcpersistence PROPERTIES VERSION ${PERSISTENCE_VERSION}  SOVERSION ${PERSISTENCE_VERSION_MAJOR})
# #
# ##SET_TARGET_PROPERTIES(codelithic PROPERTIES IMPORTED_LOCATION ${CGICC_STATIC_LIB})
# TARGET_LINK_LIBRARIES(t2pcpersistence  pq pthread)


