cmake_minimum_required( VERSION 3.5 )
project( ssv4 LANGUAGES CXX )

# Build ss4as
add_executable( ss4as src/ss4as.cpp )
target_compile_features( ss4as PRIVATE cxx_std_17 )

# Build ss4vm
add_executable( ss4vm src/ss4vm.cpp )
target_compile_features( ss4vm PRIVATE cxx_std_17 )
