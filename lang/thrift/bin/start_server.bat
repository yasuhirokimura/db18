@echo off

set CPATH=..\jars\db_thrift_interface.jar;..\jars\db_thrift_server.jar
set CPATH=..\jars\log4j-api.jar;..\jars\log4j-1.2-api.jar;%CD%;%CPATH%
set CPATH=..\jars\log4j-core.jar;..\jars\libthrift.jar;..\jars\db.jar;%CPATH%
set CPATH=..\jars\slf4j-api.jar;..\jars\log4j-slf4j-impl.jar;%CPATH%;%CLASSPATH%

set LPATH=..\bin;..\..\..\build_windows\Win32\Debug
set LPATH=..\..\..\build_windows\Win32\Release;%LPATH
set LPATH=..\..\..\build_windows\x64\Debug;%LPATH
set LPATH=..\..\..\build_windows\x64\Release;%LPATH
set LPATH=%LPATH%;%PATH%

java -cp "%CPATH%" -Djava.library.path="%LPATH%" com.sleepycat.server.BdbServer %*
