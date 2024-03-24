if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/cpp-serializer-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

# Project is configured with no languages, so tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package cpp-serializer)

install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT cpp-serializer_Development
)

install(
    TARGETS cpp-serializer_cpp-serializer
    EXPORT cpp-serializerTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Allow package maintainers to freely override the path for the configs
set(
    cpp-serializer_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE cpp-serializer_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(cpp-serializer_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${cpp-serializer_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT cpp-serializer_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${cpp-serializer_INSTALL_CMAKEDIR}"
    COMPONENT cpp-serializer_Development
)

install(
    EXPORT cpp-serializerTargets
    NAMESPACE cpp-serializer::
    DESTINATION "${cpp-serializer_INSTALL_CMAKEDIR}"
    COMPONENT cpp-serializer_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
