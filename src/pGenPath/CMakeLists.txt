#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              Zach Duguid
#--------------------------------------------------------

SET(SRC
  GenPath.cpp
  GenPath_Info.cpp
  Point.cpp
  main.cpp
)

ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
   ${MOOS_LIBRARIES}
   geometry
   apputil
   mbutil
   m
   pthread)

