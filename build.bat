@echo off

gcc -shared Dependencies\sqlite3.c -o sqlite3.dll
g++ -c Source\ExpenseTracker.cpp Source\Application.cpp
g++ ExpenseTracker.o Application.o -o ExpenceTracker -IDependencies -lsqlite3 -lGdi32
del ExpenseTracker.o Application.o