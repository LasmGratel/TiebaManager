@echo off

ren ������ ���ɹ�����
move Release\TiebaManager.exe ���ɹ����� >nul
move Release\TiebaManager.exe Helper.dll >nul
del ���ɹ�����.zip >nul 2>nul
"F:\Program Files\7-Zip\7z.exe" a -tzip ���ɹ����� ���ɹ�����

:Restore
ping 127.0.0.1 -n 3 >nul
move ���ɹ�����\TiebaManager.exe Release\TiebaManager�ɰ�.exe >nul 2>nul || goto Restore
move Release\TiebaManager.pdb Release\TiebaManager�ɰ�.pdb >nul 2>nul || goto Restore
move ���ɹ�����\Helper.dll Release\Helper�ɰ�.dll >nul 2>nul || goto Restore
move Release\Helper.pdb Release\Helper�ɰ�.pdb >nul 2>nul || goto Restore
ren ���ɹ����� ������ 2>nul || goto Restore
