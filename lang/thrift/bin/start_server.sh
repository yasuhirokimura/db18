#!/bin/sh

java -cp ../lib/slf4j-api.jar:../lib/log4j-1.2-api.jar:\
../lib/log4j-core.jar:../lib/log4j-api.jar:../lib/log4j-slf4j-impl.jar:\
../lib/libthrift.jar:../lib/db.jar:../lib/db_thrift_interface.jar:\
../lib/db_thrift_server.jar:./ -Djava.library.path=../lib\
	com.sleepycat.server.BdbServer $@
