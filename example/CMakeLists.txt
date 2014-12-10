#xuxiduo@zju.edu.cn
SET(INC_DIRS ${PROJECT_SOURCE_DIR}/deplibs/LibObjectness ${PROJECT_SOURCE_DIR}/deplibs/LibLinear)
SET(LIB_DIRS ${PROJECT_BINARY_DIR}/deplibs/LibLinear ${PROJECT_BINARY_DIR}/deplibs/LibObjectness /usr/lib /usr/local/lib)

#OPENCV_DIR should be set as an environment variable in the shell if it is not
#present in the standard directories
IF (DEFINED ENV{OPENCV_DIR})
  LIST(APPEND INC_DIRS ${OPENCV_DIR}/include)
  LIST(APPEND LIB_DIRS ${OPENCV_DIR}/lib)
ENDIF (DEFINED ENV{OPENCV_DIR})

INCLUDE_DIRECTORIES(${INC_DIRS})
LINK_DIRECTORIES(${LIB_DIRS})

IF(${CMAKE_COMPILER_IS_GNUCXX})
	LIST( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -fopenmp -ftree-vectorize -march=native")
ENDIF (${CMAKE_COMPILER_IS_GNUCXX})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENMP_FLAG} -O3")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${OpenCV_LIBS} libobjectness.a  liblinear.a  libblas.a)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION 	bin)
