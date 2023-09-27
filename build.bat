@echo off

REM copy/add dll to the project
IF NOT EXIST sqlite3.dll gcc -shared Dependencies\SQLite3\sqlite3.c -o sqlite3.dll
IF NOT EXIST imgui.dll g++ -shared Dependencies\ImGui\*.cpp -o imgui.dll -IDependencies -LDependencies\GLFW -lopengl32 -lglfw3dll
IF NOT EXIST glfw3.dll copy Dependencies\GLFW\glfw3.dll .

SET compiler=g++
SET flags=-Wall -Wextra -Werror -pedantic
SET source=Source\*.cpp
SET obj=.\*.o
SET libs=-LDependencies\GLFW -L. -lsqlite3 -lopengl32 -lglfw3dll -limgui

%compiler% -c %source% -IDependencies
%compiler% %obj% -o ExpenceTracker  %libs%
del %obj%