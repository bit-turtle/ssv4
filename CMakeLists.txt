cmake_minimum_required( VERSION 3.5 )
project( ssv4 LANGUAGES CXX )

# Build ss4as
add_executable( ss4as src/ss4as.cpp )
target_compile_features( ss4as PRIVATE cxx_std_17 )

# Copy ss4as.bat if on Windows
if (WIN32)
   configure_file( ${CMAKE_SOURCE_DIR}/src/ss4as.bat ${CMAKE_CURRENT_BINARY_DIR}/Release/ss4as.bat COPYONLY )
endif()

# Build ss4vm
add_executable( ss4vm src/ss4vm.cpp )
target_compile_features( ss4vm PRIVATE cxx_std_17 )

# Link pthread and X11 if on Linux
if (UNIX)
   find_package(Threads REQUIRED)
   target_link_libraries( ss4vm ${CMAKE_THREAD_LIBS_INIT} )
   find_package(X11 REQUIRED)
   target_link_libraries( ss4vm ${X11_LIBRARIES} )
endif()

# Copy ss4vm.bat if on Windows
if (WIN32)
   configure_file( ${CMAKE_SOURCE_DIR}/src/ss4vm.bat ${CMAKE_CURRENT_BINARY_DIR}/Release/ss4vm.bat COPYONLY )
endif()
