# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/jun.zhang/my-udf/over/impala-udf-samples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/jun.zhang/my-udf/over/impala-udf-samples

# Include any dependencies generated for this target.
include CMakeFiles/udfsample.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/udfsample.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/udfsample.dir/flags.make

CMakeFiles/udfsample.dir/udf-sample.cc.o: CMakeFiles/udfsample.dir/flags.make
CMakeFiles/udfsample.dir/udf-sample.cc.o: udf-sample.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /data/jun.zhang/my-udf/over/impala-udf-samples/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/udfsample.dir/udf-sample.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/udfsample.dir/udf-sample.cc.o -c /data/jun.zhang/my-udf/over/impala-udf-samples/udf-sample.cc

CMakeFiles/udfsample.dir/udf-sample.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/udfsample.dir/udf-sample.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /data/jun.zhang/my-udf/over/impala-udf-samples/udf-sample.cc > CMakeFiles/udfsample.dir/udf-sample.cc.i

CMakeFiles/udfsample.dir/udf-sample.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/udfsample.dir/udf-sample.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /data/jun.zhang/my-udf/over/impala-udf-samples/udf-sample.cc -o CMakeFiles/udfsample.dir/udf-sample.cc.s

CMakeFiles/udfsample.dir/udf-sample.cc.o.requires:
.PHONY : CMakeFiles/udfsample.dir/udf-sample.cc.o.requires

CMakeFiles/udfsample.dir/udf-sample.cc.o.provides: CMakeFiles/udfsample.dir/udf-sample.cc.o.requires
	$(MAKE) -f CMakeFiles/udfsample.dir/build.make CMakeFiles/udfsample.dir/udf-sample.cc.o.provides.build
.PHONY : CMakeFiles/udfsample.dir/udf-sample.cc.o.provides

CMakeFiles/udfsample.dir/udf-sample.cc.o.provides.build: CMakeFiles/udfsample.dir/udf-sample.cc.o

# Object files for target udfsample
udfsample_OBJECTS = \
"CMakeFiles/udfsample.dir/udf-sample.cc.o"

# External object files for target udfsample
udfsample_EXTERNAL_OBJECTS =

build/libudfsample.so: CMakeFiles/udfsample.dir/udf-sample.cc.o
build/libudfsample.so: CMakeFiles/udfsample.dir/build.make
build/libudfsample.so: CMakeFiles/udfsample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library build/libudfsample.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/udfsample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/udfsample.dir/build: build/libudfsample.so
.PHONY : CMakeFiles/udfsample.dir/build

CMakeFiles/udfsample.dir/requires: CMakeFiles/udfsample.dir/udf-sample.cc.o.requires
.PHONY : CMakeFiles/udfsample.dir/requires

CMakeFiles/udfsample.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/udfsample.dir/cmake_clean.cmake
.PHONY : CMakeFiles/udfsample.dir/clean

CMakeFiles/udfsample.dir/depend:
	cd /data/jun.zhang/my-udf/over/impala-udf-samples && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/jun.zhang/my-udf/over/impala-udf-samples /data/jun.zhang/my-udf/over/impala-udf-samples /data/jun.zhang/my-udf/over/impala-udf-samples /data/jun.zhang/my-udf/over/impala-udf-samples /data/jun.zhang/my-udf/over/impala-udf-samples/CMakeFiles/udfsample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/udfsample.dir/depend

