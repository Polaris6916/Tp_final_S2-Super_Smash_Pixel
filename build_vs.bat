rem Use this batch file to build for Visual Studio 17
rmdir /s /q _build_vs
mkdir _build_vs
cd _build_vs
cmake .. -G "Visual Studio 18 2026" -A x64
