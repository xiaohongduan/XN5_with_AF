@echo off
python "C:\Documents and Settings\Christian\My Documents\Downloads\pyinstaller-1.5-rc1\pyinstaller-1.5-rc1\Build.py" xpnhelper.spec
copy xpnhelper.ui dist\xpnhelper
mkdir dist\xpnhelper\template
copy conf\template\*.* dist\xpnhelper\template
mkdir dist\xpnhelper\etc\fonts
copy conf\etc\fonts\*.*  dist\xpnhelper\etc\fonts
mkdir dist\xpnhelper\etc\gtk-2.0
copy conf\etc\gtk-2.0\*.*  dist\xpnhelper\etc\gtk-2.0
mkdir dist\xpnhelper\etc\pango
copy conf\etc\pango\*.*  dist\xpnhelper\etc\pango
mkdir dist\xpnhelper\lib\gtk-2.0\2.10.0\engines
copy conf\lib\gtk-2.0\2.10.0\engines\*.*  dist\xpnhelper\lib\gtk-2.0\2.10.0\engines
mkdir dist\xpnhelper\share\themes\Clearlooks\gtk-2.0
copy conf\share\themes\Clearlooks\gtk-2.0\*.* dist\xpnhelper\share\themes\Clearlooks\gtk-2.0
pause