# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/erik/Projects/c++/placeScan

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/erik/Projects/c++/placeScan

# Include any dependencies generated for this target.
include CMakeFiles/placeScan.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/placeScan.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/placeScan.dir/flags.make

CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o: CMakeFiles/placeScan.dir/flags.make
CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o: placeScanHelper2.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/erik/Projects/c++/placeScan/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o"
	/usr/bin/g++-4.9   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o -c /home/erik/Projects/c++/placeScan/placeScanHelper2.cpp

CMakeFiles/placeScan.dir/placeScanHelper2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/placeScan.dir/placeScanHelper2.cpp.i"
	/usr/bin/g++-4.9  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/erik/Projects/c++/placeScan/placeScanHelper2.cpp > CMakeFiles/placeScan.dir/placeScanHelper2.cpp.i

CMakeFiles/placeScan.dir/placeScanHelper2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/placeScan.dir/placeScanHelper2.cpp.s"
	/usr/bin/g++-4.9  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/erik/Projects/c++/placeScan/placeScanHelper2.cpp -o CMakeFiles/placeScan.dir/placeScanHelper2.cpp.s

CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.requires:
.PHONY : CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.requires

CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.provides: CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.requires
	$(MAKE) -f CMakeFiles/placeScan.dir/build.make CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.provides.build
.PHONY : CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.provides

CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.provides.build: CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o

CMakeFiles/placeScan.dir/placeScanHelper.cpp.o: CMakeFiles/placeScan.dir/flags.make
CMakeFiles/placeScan.dir/placeScanHelper.cpp.o: placeScanHelper.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/erik/Projects/c++/placeScan/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/placeScan.dir/placeScanHelper.cpp.o"
	/usr/bin/g++-4.9   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/placeScan.dir/placeScanHelper.cpp.o -c /home/erik/Projects/c++/placeScan/placeScanHelper.cpp

CMakeFiles/placeScan.dir/placeScanHelper.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/placeScan.dir/placeScanHelper.cpp.i"
	/usr/bin/g++-4.9  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/erik/Projects/c++/placeScan/placeScanHelper.cpp > CMakeFiles/placeScan.dir/placeScanHelper.cpp.i

CMakeFiles/placeScan.dir/placeScanHelper.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/placeScan.dir/placeScanHelper.cpp.s"
	/usr/bin/g++-4.9  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/erik/Projects/c++/placeScan/placeScanHelper.cpp -o CMakeFiles/placeScan.dir/placeScanHelper.cpp.s

CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.requires:
.PHONY : CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.requires

CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.provides: CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.requires
	$(MAKE) -f CMakeFiles/placeScan.dir/build.make CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.provides.build
.PHONY : CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.provides

CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.provides.build: CMakeFiles/placeScan.dir/placeScanHelper.cpp.o

CMakeFiles/placeScan.dir/placeScan.cpp.o: CMakeFiles/placeScan.dir/flags.make
CMakeFiles/placeScan.dir/placeScan.cpp.o: placeScan.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/erik/Projects/c++/placeScan/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/placeScan.dir/placeScan.cpp.o"
	/usr/bin/g++-4.9   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/placeScan.dir/placeScan.cpp.o -c /home/erik/Projects/c++/placeScan/placeScan.cpp

CMakeFiles/placeScan.dir/placeScan.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/placeScan.dir/placeScan.cpp.i"
	/usr/bin/g++-4.9  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/erik/Projects/c++/placeScan/placeScan.cpp > CMakeFiles/placeScan.dir/placeScan.cpp.i

CMakeFiles/placeScan.dir/placeScan.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/placeScan.dir/placeScan.cpp.s"
	/usr/bin/g++-4.9  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/erik/Projects/c++/placeScan/placeScan.cpp -o CMakeFiles/placeScan.dir/placeScan.cpp.s

CMakeFiles/placeScan.dir/placeScan.cpp.o.requires:
.PHONY : CMakeFiles/placeScan.dir/placeScan.cpp.o.requires

CMakeFiles/placeScan.dir/placeScan.cpp.o.provides: CMakeFiles/placeScan.dir/placeScan.cpp.o.requires
	$(MAKE) -f CMakeFiles/placeScan.dir/build.make CMakeFiles/placeScan.dir/placeScan.cpp.o.provides.build
.PHONY : CMakeFiles/placeScan.dir/placeScan.cpp.o.provides

CMakeFiles/placeScan.dir/placeScan.cpp.o.provides.build: CMakeFiles/placeScan.dir/placeScan.cpp.o

# Object files for target placeScan
placeScan_OBJECTS = \
"CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o" \
"CMakeFiles/placeScan.dir/placeScanHelper.cpp.o" \
"CMakeFiles/placeScan.dir/placeScan.cpp.o"

# External object files for target placeScan
placeScan_EXTERNAL_OBJECTS =

placeScan: CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o
placeScan: CMakeFiles/placeScan.dir/placeScanHelper.cpp.o
placeScan: CMakeFiles/placeScan.dir/placeScan.cpp.o
placeScan: CMakeFiles/placeScan.dir/build.make
placeScan: /usr/local/lib/libopencv_viz.so.3.0.0
placeScan: /usr/local/lib/libopencv_videostab.so.3.0.0
placeScan: /usr/local/lib/libopencv_videoio.so.3.0.0
placeScan: /usr/local/lib/libopencv_video.so.3.0.0
placeScan: /usr/local/lib/libopencv_superres.so.3.0.0
placeScan: /usr/local/lib/libopencv_stitching.so.3.0.0
placeScan: /usr/local/lib/libopencv_shape.so.3.0.0
placeScan: /usr/local/lib/libopencv_photo.so.3.0.0
placeScan: /usr/local/lib/libopencv_objdetect.so.3.0.0
placeScan: /usr/local/lib/libopencv_ml.so.3.0.0
placeScan: /usr/local/lib/libopencv_imgproc.so.3.0.0
placeScan: /usr/local/lib/libopencv_imgcodecs.so.3.0.0
placeScan: /usr/local/lib/libopencv_highgui.so.3.0.0
placeScan: /usr/local/lib/libopencv_hal.a
placeScan: /usr/local/lib/libopencv_flann.so.3.0.0
placeScan: /usr/local/lib/libopencv_features2d.so.3.0.0
placeScan: /usr/local/lib/libopencv_core.so.3.0.0
placeScan: /usr/local/lib/libopencv_calib3d.so.3.0.0
placeScan: /usr/local/lib/libgflags.a
placeScan: /opt/gurobi650/linux64/lib/libgurobi_c++.a
placeScan: /opt/gurobi650/linux64/lib/libgurobi65.so
placeScan: /usr/local/lib/libopencv_features2d.so.3.0.0
placeScan: /usr/local/lib/libopencv_ml.so.3.0.0
placeScan: /usr/local/lib/libopencv_highgui.so.3.0.0
placeScan: /usr/local/lib/libopencv_videoio.so.3.0.0
placeScan: /usr/local/lib/libopencv_imgcodecs.so.3.0.0
placeScan: /usr/local/lib/libopencv_flann.so.3.0.0
placeScan: /usr/local/lib/libopencv_video.so.3.0.0
placeScan: /usr/local/lib/libopencv_imgproc.so.3.0.0
placeScan: /usr/local/lib/libopencv_core.so.3.0.0
placeScan: /usr/local/lib/libopencv_hal.a
placeScan: /usr/local/share/OpenCV/3rdparty/lib/libippicv.a
placeScan: CMakeFiles/placeScan.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable placeScan"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/placeScan.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/placeScan.dir/build: placeScan
.PHONY : CMakeFiles/placeScan.dir/build

CMakeFiles/placeScan.dir/requires: CMakeFiles/placeScan.dir/placeScanHelper2.cpp.o.requires
CMakeFiles/placeScan.dir/requires: CMakeFiles/placeScan.dir/placeScanHelper.cpp.o.requires
CMakeFiles/placeScan.dir/requires: CMakeFiles/placeScan.dir/placeScan.cpp.o.requires
.PHONY : CMakeFiles/placeScan.dir/requires

CMakeFiles/placeScan.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/placeScan.dir/cmake_clean.cmake
.PHONY : CMakeFiles/placeScan.dir/clean

CMakeFiles/placeScan.dir/depend:
	cd /home/erik/Projects/c++/placeScan && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/erik/Projects/c++/placeScan /home/erik/Projects/c++/placeScan /home/erik/Projects/c++/placeScan /home/erik/Projects/c++/placeScan /home/erik/Projects/c++/placeScan/CMakeFiles/placeScan.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/placeScan.dir/depend

