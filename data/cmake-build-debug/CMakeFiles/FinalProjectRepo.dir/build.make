# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/FinalProjectRepo.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/FinalProjectRepo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/FinalProjectRepo.dir/flags.make

CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.o: CMakeFiles/FinalProjectRepo.dir/flags.make
CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.o: ../DataControllerTests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.o -c "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/DataControllerTests.cpp"

CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/DataControllerTests.cpp" > CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.i

CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/DataControllerTests.cpp" -o CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.s

CMakeFiles/FinalProjectRepo.dir/DataController.cpp.o: CMakeFiles/FinalProjectRepo.dir/flags.make
CMakeFiles/FinalProjectRepo.dir/DataController.cpp.o: ../DataController.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/FinalProjectRepo.dir/DataController.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/FinalProjectRepo.dir/DataController.cpp.o -c "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/DataController.cpp"

CMakeFiles/FinalProjectRepo.dir/DataController.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FinalProjectRepo.dir/DataController.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/DataController.cpp" > CMakeFiles/FinalProjectRepo.dir/DataController.cpp.i

CMakeFiles/FinalProjectRepo.dir/DataController.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FinalProjectRepo.dir/DataController.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/DataController.cpp" -o CMakeFiles/FinalProjectRepo.dir/DataController.cpp.s

# Object files for target FinalProjectRepo
FinalProjectRepo_OBJECTS = \
"CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.o" \
"CMakeFiles/FinalProjectRepo.dir/DataController.cpp.o"

# External object files for target FinalProjectRepo
FinalProjectRepo_EXTERNAL_OBJECTS =

FinalProjectRepo: CMakeFiles/FinalProjectRepo.dir/DataControllerTests.cpp.o
FinalProjectRepo: CMakeFiles/FinalProjectRepo.dir/DataController.cpp.o
FinalProjectRepo: CMakeFiles/FinalProjectRepo.dir/build.make
FinalProjectRepo: CMakeFiles/FinalProjectRepo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable FinalProjectRepo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FinalProjectRepo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/FinalProjectRepo.dir/build: FinalProjectRepo
.PHONY : CMakeFiles/FinalProjectRepo.dir/build

CMakeFiles/FinalProjectRepo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/FinalProjectRepo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/FinalProjectRepo.dir/clean

CMakeFiles/FinalProjectRepo.dir/depend:
	cd "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data" "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data" "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug" "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug" "/Users/simonsandrew/Documents/RPI/RPI/Parallel Programming/FinalProjectRepo/data/cmake-build-debug/CMakeFiles/FinalProjectRepo.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/FinalProjectRepo.dir/depend

