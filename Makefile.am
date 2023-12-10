SUBDIRS = src
SUBDIRS += src/plugins/console
SUBDIRS += src/plugins/graphical_console
slim:
	@cd src && make
plugins: console graphical_console
console:
	@cd src/plugins/console && make
graphical_console:
	@cd src/plugins/graphical_console && make