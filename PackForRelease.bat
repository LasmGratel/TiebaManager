@echo off

if not exist Release\TiebaManager.exe (
move Release\TiebaManager�ɰ�.exe Release\TiebaManager.exe >nul
move Release\TiebaManager�ɰ�.pdb Release\TiebaManager.pdb >nul
)
if not exist Release\Helper.dll (
move Release\Helper�ɰ�.dll Release\Helper.dll >nul
move Release\Helper�ɰ�.pdb Release\Helper.pdb >nul
)
if not exist Release\TiebaAPI.dll (
move Release\TiebaAPI�ɰ�.dll Release\TiebaAPI.dll >nul
move Release\TiebaAPI�ɰ�.pdb Release\TiebaAPI.pdb >nul
)
if not exist Release\TiebaManagerCore.dll (
move Release\TiebaManagerCore�ɰ�.dll Release\TiebaManagerCore.dll >nul
move Release\TiebaManagerCore�ɰ�.pdb Release\TiebaManagerCore.pdb >nul
)

ren ������ ���ɹ�����
move Release\TiebaManager.exe ���ɹ����� >nul
move Release\Helper.dll ���ɹ����� >nul
move Release\TiebaAPI.dll ���ɹ����� >nul
move Release\TiebaManagerCore.dll ���ɹ����� >nul
del ���ɹ�����.zip >nul 2>nul
"F:\Program Files\7-Zip\7z.exe" a -tzip ���ɹ����� ���ɹ�����

:Restore
ping 127.0.0.1 -n 3 >nul
move ���ɹ�����\TiebaManager.exe Release\TiebaManager�ɰ�.exe >nul 2>nul || goto Restore
move Release\TiebaManager.pdb Release\TiebaManager�ɰ�.pdb >nul 2>nul || goto Restore
move ���ɹ�����\Helper.dll Release\Helper�ɰ�.dll >nul 2>nul || goto Restore
move Release\Helper.pdb Release\Helper�ɰ�.pdb >nul 2>nul || goto Restore
move ���ɹ�����\TiebaAPI.dll Release\TiebaAPI�ɰ�.dll >nul 2>nul || goto Restore
move Release\TiebaAPI.pdb Release\TiebaAPI�ɰ�.pdb >nul 2>nul || goto Restore
move ���ɹ�����\TiebaManagerCore.dll Release\TiebaManagerCore�ɰ�.dll >nul 2>nul || goto Restore
move Release\TiebaManagerCore.pdb Release\TiebaManagerCore�ɰ�.pdb >nul 2>nul || goto Restore
ren ���ɹ����� ������ 2>nul || goto Restore
