#xuxiduo@zju.edu.cn
INCLUDE_DIRECTORIES(../LibLinear)
IF(${CMAKE_COMPILER_IS_GNUCXX})
	LIST( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -fopenmp -ftree-vectorize -march=native")
ENDIF (${CMAKE_COMPILER_IS_GNUCXX})
AUX_SOURCE_DIRECTORY(. OBJ_SRC_LIST)
#FOREACH(F ${OBJ_SRC_LIST})
#	MESSAGE("Find : " ${F})
#ENDFOREACH(F)
ADD_LIBRARY(objectness ${OBJ_SRC_LIST})

INSTALL(FILES CmFile.h  CmShow.h  CmTimer.h  DataSetVOC.h  FilterTIG.h  ImgContrastBB.h  kyheader.h  Objectness.h  ValStructVec.h DESTINATION include/bing/LibObjectness)
INSTALL(TARGETS objectness ARCHIVE DESTINATION lib)

