:: ss4as Windows Batch File

@echo off &setlocal

:: Message
echo ss4as Assembler

:: File Chooser Dialog
echo Choose Source Files . . .

for /f "delims=" %%I in ('powershell "Add-Type -AssemblyName System.Windows.Forms;$f = new-object Windows.Forms.OpenFileDialog;$f.InitialDirectory = pwd;$f.Filter = 'All Files (*.*)|*.*';$f.ShowHelp = $true;$f.Multiselect = $true;[void]$f.ShowDialog();if ($f.Multiselect) { $f.FileNames } else { $f.FileName }"') do call set "files=%%files%% %%~I"

echo Source Files: %files%

:: Save File Dialog
echo Choose Output File . . .

set chooser=powershell "Add-Type -AssemblyName System.windows.forms|Out-Null;$f=New-Object System.Windows.Forms.SaveFileDialog;$f.InitialDirectory='%cd%';$f.Filter='All Files (*.*)|*.*';$f.showHelp=$true;$f.ShowDialog()|Out-Null;$f.FileName"

:: capture choice to a variable
for /f "delims=" %%I in ('%chooser%') do set "filename=%%I"

echo Output File: %filename%

:: Run ss4as
echo Running ss4as . . .
.\ss4as.exe %files% -o %file%

:: Wait For Keypress
pause
