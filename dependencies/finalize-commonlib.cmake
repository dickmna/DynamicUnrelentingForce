include(CMakePackageConfigHelpers)
if(NOT COMMONLIB_SOURCE OR NOT COMMONLIB_PREFIX)
  message(FATAL_ERROR "Provide -DCOMMONLIB_SOURCE=<extracted source> and -DCOMMONLIB_PREFIX=<installed prefix>")
endif()
write_basic_package_version_file(
  "${COMMONLIB_PREFIX}/lib/cmake/CommonLibSSE/CommonLibSSEConfigVersion.cmake"
  VERSION 10.0.1
  COMPATIBILITY SameMajorVersion)
file(COPY "${COMMONLIB_SOURCE}/cmake/CommonLibSSE.cmake"
  DESTINATION "${COMMONLIB_PREFIX}/lib/cmake/CommonLibSSE")
