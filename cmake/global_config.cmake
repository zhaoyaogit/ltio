
ADD_DEFINITIONS(
  #-DNET_ENABLE_REUSER_PORT
  -DUSE_TCMALLOC
)

SET(USE_TCMALLOC TRUE)

IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

