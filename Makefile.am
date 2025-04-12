SUBDIRS = src
SUBDIRS += src/plugins/console
SUBDIRS += src/plugins/fs
SUBDIRS += src/plugins/kafka
SUBDIRS += src/plugins/network
SUBDIRS += src/plugins/process
SUBDIRS += src/plugins/csv
slim:
	@cd src && make
plugins: console kafka network
console:
	@cd src/plugins/console && make
kafka:
	@cd src/plugins/kafka && make
network:
	@cd src/plugins/network && make
