SUBDIRS = src
SUBDIRS += src/plugins/console
SUBDIRS += src/plugins/network
slim:
	@cd src && make
plugins: console network
console:
	@cd src/plugins/console && make
network:
	@cd src/plugins/network && make
