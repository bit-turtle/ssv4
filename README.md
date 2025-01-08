# ssv4: Super Seal Version 4
[ssv4 instruction sheet](https://docs.google.com/spreadsheets/d/1uOwEtzu-f8msr8yjz1aJrdfqklxtp7U_aX8blZt-izU/edit?usp=sharing)

The ssv4 repo contains two projects, ss4as and ss4vm
## ss4as: the assember for the ssv4 cpu
Using ss4as:
1. ss4as is a command line tool, run ss4as --help for more information
2. On Windows, `ss4as.bat` can be used to run ss4as with a file selector gui
## ss4vm: the virtual machine for the ssv4 cpu
## Building
All Platforms (Start by opening a terminal in this folder):
1. `mkdir build`
2. `cd build`
3. `cmake ..`  

Windows (Use VS 2022 Terminal):
1. `MSBuild.exe .\ALL_BUILD.vxproj -p:Configuration=Release`

Linux/Unix:
1. `make all`
