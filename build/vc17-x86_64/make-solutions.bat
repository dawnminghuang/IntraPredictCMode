@echo off
::
:: run this batch file to create a Visual Studio solution file for this project.
:: See the cmake documentation for other generator targets
::
cmake -G "Visual Studio 15 2017 Win64" %~dp0..\..\source && cmake-gui %~dp0..\..\source
