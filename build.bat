@echo off

gcc -shared Dependencies\sqlite3.c -o sqlite3.dll
g++ -c Source\ExpenseTracker.cpp Source\Main.cpp
g++ ExpenseTracker.o -o ExpenceTracker -IDependencies -lsqlite3
del ExpenseTracker.o