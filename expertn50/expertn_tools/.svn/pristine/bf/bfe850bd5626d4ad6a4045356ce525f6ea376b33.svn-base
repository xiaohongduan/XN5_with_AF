@echo off
python "C:\Documents and Settings\Christian\My Documents\Downloads\pyinstaller-1.5-rc1\pyinstaller-1.5-rc1\Build.py" cdbplot.spec
copy cdbplot.ui dist\cdbplot
copy cdbplotWindow.ui dist\cdbplot
copy cdb.pdf dist\cdbplot
copy cdb.odp dist\cdbplot
copy cdbplot64x64.png dist\cdbplot
copy cdbplot32x32.png dist\cdbplot
copy cdbplot16x16.png dist\cdbplot
copy cdbplot32x32.ico dist\cdbplot
copy cdbplot32x32.ico dist\cdbplot\cdbplot.ico
copy cdbplot.ini dist\cdbplot
mkdir dist\cdbplot\etc\fonts
copy conf\etc\fonts\*.*  dist\cdbplot\etc\fonts
mkdir dist\cdbplot\etc\gtk-2.0
copy conf\etc\gtk-2.0\*.*  dist\cdbplot\etc\gtk-2.0
mkdir dist\cdbplot\etc\pango
copy conf\etc\pango\*.*  dist\cdbplot\etc\pango
mkdir dist\cdbplot\lib\gtk-2.0\2.10.0\engines
copy conf\lib\gtk-2.0\2.10.0\engines\*.*  dist\cdbplot\lib\gtk-2.0\2.10.0\engines
mkdir dist\cdbplot\share\themes\Clearlooks\gtk-2.0
copy conf\share\themes\Clearlooks\gtk-2.0\*.* dist\cdbplot\share\themes\Clearlooks\gtk-2.0
pause