# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /var/lib/snapd/snap/clion/129/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /var/lib/snapd/snap/clion/129/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/danilo/Scrivania/iiw_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage

# Include any dependencies generated for this target.
include CMakeFiles/iiw_client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/iiw_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/iiw_client.dir/flags.make

CMakeFiles/iiw_client.dir/main.c.o: CMakeFiles/iiw_client.dir/flags.make
CMakeFiles/iiw_client.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/iiw_client.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/iiw_client.dir/main.c.o   -c /home/danilo/Scrivania/iiw_client/main.c

CMakeFiles/iiw_client.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/iiw_client.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/danilo/Scrivania/iiw_client/main.c > CMakeFiles/iiw_client.dir/main.c.i

CMakeFiles/iiw_client.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/iiw_client.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/danilo/Scrivania/iiw_client/main.c -o CMakeFiles/iiw_client.dir/main.c.s

# Object files for target iiw_client
iiw_client_OBJECTS = \
"CMakeFiles/iiw_client.dir/main.c.o"

# External object files for target iiw_client
iiw_client_EXTERNAL_OBJECTS =

iiw_client: CMakeFiles/iiw_client.dir/main.c.o
iiw_client: CMakeFiles/iiw_client.dir/build.make
iiw_client: CMakeFiles/iiw_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable iiw_client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iiw_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/iiw_client.dir/build: iiw_client

.PHONY : CMakeFiles/iiw_client.dir/build

CMakeFiles/iiw_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/iiw_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/iiw_client.dir/clean

CMakeFiles/iiw_client.dir/depend:
	cd /home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/danilo/Scrivania/iiw_client /home/danilo/Scrivania/iiw_client /home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage /home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage /home/danilo/Scrivania/iiw_client/cmake-build-debug-coverage/CMakeFiles/iiw_client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/iiw_client.dir/depend

