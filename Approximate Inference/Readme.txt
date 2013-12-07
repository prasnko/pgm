
The code is written in Visual C++ using Windows 7 x64 platform using g++ compiler.

Steps to compile:
1. Go to folder where all .cpp files are present and give command

g++ *.cpp -o <output-file>

eg:

g++ *.cpp -o  SamplingVE.cpp

Steps to execute:

1. Go to folder of SamplingVE.exe and give command

SamplingVE.exe <uaifile> <evidfile> <num_samples> <w> <a=1:0 (adaptive:wcutset)>

eg:

SamplingVE.exe C:\Prasanna\Spring13\PGM\HW3.5\Data\all\404.wcsp.uai C:\Prasanna\Spring13\PGM\HW3.5\Data\all\404.wcsp.uai.evid 100 1 0




