@echo off

ren ������ ���ɹ�����
move Release\TiebaManager.exe ���ɹ����� >nul
del ���ɹ�����.zip >nul 2>nul
"F:\Program Files\7-Zip\7z.exe" a -tzip ���ɹ����� ���ɹ�����

:Restore
ping 127.0.0.1 -n 3 >nul
move ���ɹ�����\TiebaManager.exe Release >nul 2>nul || goto Restore
ren ���ɹ����� ������ 2>nul || goto Restore
