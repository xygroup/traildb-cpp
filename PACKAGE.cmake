include (${WORKSPACE}/traildb/PACKAGE.cmake)

set (TRAILDB-CPP_INC ${INC_ROOT}/traildb-cpp/_/traildb-cpp)
set (TRAILDB-CPP_LIB ${TRAILDB_LIB})

include_directories(${TRAILDB-CPP_INC})
