cmake_minimum_required( VERSION 3.5 )
project( ssv4 LANGUAGES CXX )

# SFML for Graphics
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Build ss4as
add_executable( ss4as src/ss4as.cpp )
target_compile_features( ss4as PRIVATE cxx_std_17 )

# Build ss4vm
add_executable( ss4vm src/ss4vm.cpp )
target_link_libraries( ss4vm PRIVATE sfml-graphics )
target_compile_features( ss4vm PRIVATE cxx_std_17 )
