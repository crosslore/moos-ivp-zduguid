##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(lib_behaviors-test)
ADD_SUBDIRECTORY(pXRelayTest)
ADD_SUBDIRECTORY(pExampleApp)
ADD_SUBDIRECTORY(pOdometry)
ADD_SUBDIRECTORY(pPrimeFactor)
ADD_SUBDIRECTORY(pCommunicationAngle_zduguid)
ADD_SUBDIRECTORY(pPointAssign)
ADD_SUBDIRECTORY(pGenPath)
ADD_SUBDIRECTORY(pHazardPath)
ADD_SUBDIRECTORY(uFldHazardMgrX)
ADD_SUBDIRECTORY(pOptSensor)
ADD_SUBDIRECTORY(pFrontEstimateX)
ADD_SUBDIRECTORY(pSpiralPath)
ADD_SUBDIRECTORY(pEncircle)
ADD_SUBDIRECTORY(pVehicleDataExchange)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

