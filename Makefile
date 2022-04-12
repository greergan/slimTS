SUBDIRS = utilities program
default_target: none

all: slim utils

slim:
	@cd program && if [ !CMakeFiles ] || [ !CMakeCache.txt  ] || [ !CMakeLists.txt ] || [ !Makefile ]; then cmake .;fi && make

utils:
	@cd utilities && if [ !CMakeFiles ] || [ !CMakeCache.txt  ] || [ !CMakeLists.txt ] || [ !Makefile ]; then cmake .;fi && make

clean:
	@for dir in $(SUBDIRS); do \
		find . -name CMakeCache.txt -delete; \
		find . -name cmake_install.cmake -delete; \
		find . -type d -name CMakeFiles -print0|xargs -0 rm -rf --; \
	done

cleaner: clean
	@find utilities -type d -name bin -print0|xargs -0 rm -rf --
	@find program -type d -name bin -print0|xargs -0 rm -rf --

none:
	@echo Usage: make [:target]
	@echo Targets:
	@echo "\tall"
	@echo "\tslim"
	@echo "\tutils"
	@echo "\tclean"
	@echo "\tcleaner"
	@echo "\tcleanest"