@echo off

copy 3rdParty\libcurl\bin\*.dll ���ɹ�����
copy 3rdParty\tinyxml2\bin\Release\*.dll ���ɹ�����
copy 3rdParty\opencv\bin\Release\*.dll ���ɹ�����

md Debug
copy 3rdParty\libcurl\bin\*.dll Debug
copy 3rdParty\tinyxml2\bin\Debug\*.dll Debug
copy 3rdParty\opencv\bin\Debug\*.dll Debug

md Release
copy 3rdParty\libcurl\bin\*.dll Release
copy 3rdParty\tinyxml2\bin\Release\*.dll Release
copy 3rdParty\opencv\bin\Release\*.dll Release

pause
