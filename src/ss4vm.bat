:: ss4vm Windows Batch File

@echo off &setlocal

:: Message
echo ss4vm Virtual Machine

:: File Chooser Dialog
echo Choose ROM File . . .

for /f "delims=" %%I in ('powershell "Add-Type -AssemblyName System.Windows.Forms;$f = new-object Windows.Forms.OpenFileDialog;$f.InitialDirectory = pwd;$f.Filter = 'All Files (*.*)|*.*';$f.ShowHelp = $true;$f.Multiselect = $fa;se;[void]$f.ShowDialog();if ($f.Multiselect) { $f.FileNames } else { $f.FileName }"') do call set "file=%%~I"

echo ROM File: %file%

:: Run ss4as
echo Running ss4as . . .
.\ss4vm.exe %file%

:: Wait For Keypress
pause
