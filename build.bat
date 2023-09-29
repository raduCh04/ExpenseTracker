@echo off

IF NOT EXIST Bin\ mkdir Bin
IF NOT EXIST Obj\ mkdir Obj

REM copy/build dlls to the project
pushd Bin
IF NOT EXIST sqlite3.dll gcc -shared ..\Dependencies\SQLite3\sqlite3.c -o sqlite3.dll
IF NOT EXIST imgui.dll g++ -shared ..\Dependencies\ImGui\*.cpp -o imgui.dll -I..\Dependencies -L..\Dependencies\GLFW -lopengl32 -lglfw3dll
IF NOT EXIST Bin\glfw3.dll copy ..\Dependencies\GLFW\glfw3.dll .
popd

SET compiler=g++
SET flags=-Wall -Wextra -Werror -pedantic
SET source=Source\*.cpp
SET obj=Obj\*.o
SET libs=-lsqlite3 -lopengl32 -lglfw3dll -limgui

pushd Obj
%compiler% -c ..\%source% -I..\Dependencies
popd

pushd Bin
%compiler% ..\%obj% -o ExpenceTracker -L..\Dependencies\GLFW -L.  %libs%
popd