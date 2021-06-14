@echo off
python "C:\PyInstaller\pyinstaller-2.0\pyinstaller.py" expertn_gui.spec
copy expertn_gui.ini dist\expertn_gui
copy expertn_gui_main.ui dist\expertn_gui
copy models.def dist\expertn_gui
copy result.def dist\expertn_gui
copy GUI_start_final.png dist\expertn_gui
copy xnoplot.ui dist\expertn_gui
copy xnoplotWindow.ui dist\expertn_gui
copy xnoplot.ini dist\expertn_gui
copy atmosphere.png dist\expertn_gui
copy control32x32.png dist\expertn_gui
copy heat.png dist\expertn_gui
copy management32x32.png dist\expertn_gui
copy nitrogen.png dist\expertn_gui
copy plant.png dist\expertn_gui
copy water.png dist\expertn_gui
copy xnoplot.ini dist\expertn_gui
copy statistic.png dist\expertn_gui
copy statistic_formula.pdf dist\expertn_gui
: copy missing dlls
copy libcroco-0.6-3.dll dist\expertn_gui
copy librsvg-2-2.dll dist\expertn_gui
copy libgsf-win32-1-114.dll dist\expertn_gui
copy libgsf-1-114.dll dist\expertn_gui
copy libxml2-2.dll dist\expertn_gui
mkdir dist\expertn_gui\etc\fonts
copy conf\etc\fonts\*.*  dist\expertn_gui\etc\fonts
mkdir dist\expertn_gui\etc\gtk-2.0
copy conf\etc\gtk-2.0\*.*  dist\expertn_gui\etc\gtk-2.0
mkdir dist\expertn_gui\etc\pango
copy conf\etc\pango\*.*  dist\expertn_gui\etc\pango
mkdir dist\expertn_gui\lib\gtk-2.0\2.10.0\engines
copy conf\lib\gtk-2.0\2.10.0\engines\*.*  dist\expertn_gui\lib\gtk-2.0\2.10.0\engines
mkdir dist\expertn_gui\share\themes\Clearlooks\gtk-2.0
copy conf\share\themes\Clearlooks\gtk-2.0\*.* dist\expertn_gui\share\themes\Clearlooks\gtk-2.0
pause
