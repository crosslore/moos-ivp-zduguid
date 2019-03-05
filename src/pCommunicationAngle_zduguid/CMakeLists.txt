#--------------------------------------------------------
# The CMakeLists.txt for:                       pCommunicationAngle_zduguid
# Author(s):                              Zach Duguid
#--------------------------------------------------------

SET(SRC
  CommunicationAngle_zduguid.cpp
  CommunicationAngle_zduguid_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pCommunicationAngle_zduguid ${SRC})

TARGET_LINK_LIBRARIES(pCommunicationAngle_zduguid
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

