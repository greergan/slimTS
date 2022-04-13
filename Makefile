SUBDIRS = utilities program
default_target: none

all: slim utils

slim:
	@cd program && if [ !CMakeFiles ] || [ !CMakeCache.txt  ] || [ !CMakeLists.txt ] || [ !Makefile ]; then cmake .;fi && make

utils:
	@cd utilities && if [ !CMakeFiles ] || [ !CMakeCache.txt  ] || [ !CMakeLists.txt ] || [ !Makefile ]; then cmake .;fi && make

clean:
	@for dir in $(SUBDIRS); do \
		find $$dir -name Makefile -delete; \
		find $$dir -name CMakeCache.txt -delete; \
		find $$dir -name cmake_install.cmake -delete; \
		find $$dir -type d -name CMakeFiles -print0|xargs -0 rm -rf --; \
	done

cleaner: clean
	@for dir in $(SUBDIRS); do \
		find $$dir -type d -name bin -print0|xargs -0 rm -rf --; \
	done

none:
	@echo Usage: make [:target]
	@echo Targets:
	@echo "\tall"
	@echo "\tslim"
	@echo "\tutils"
	@echo "\tclean"
	@echo "\tcleaner"
