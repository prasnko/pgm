The code is written in Visual C++ 2012 using Windows 7 x64 platform and compiled using g++.

Steps to compile:

1. Extract code files from LearnBN.zip and place them in a folder.
2. cd to the folder and give command g++ *.cpp -o LearnBN.exe


Steps to run and see output:
1. Change extension of LearnBN.txt to LearnBN.exe

2. cd to to folder where LearnBN.exe is kept.

3. Type LearnBN.exe <program-no> <input-uai-file> <training-data> <test-data> <output-uai-file>

 where program-no = 1 for FOD-param
		  = 2 for POD-param
		  = 3 for Chow-Liu

for eg for FOD-param:
  LearnBN.exe 1 C:\Prasanna\Spring13\PGM\HW5\hw5-data\1.uai.data\1.uai, C:\Prasanna\Spring13\PGM\HW5\hw5-data\1.uai.data\train-f-1.txt, C:\Prasanna\Spring13\PGM\HW5\hw5-data\1.uai.data\test.txt C:\Prasanna\Spring13\PGM\HW5\hw5-data\1.uai.data\fodModel.txt
