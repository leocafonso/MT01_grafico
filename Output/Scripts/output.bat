SET PROJNAME=MT02
SET OUTPUTPATH=C:\Workspace\e2studio\git\MT02_grafico\Output
SET PROJPATH=C:\Workspace\e2studio\git\MT02_grafico
SET WORKSPACEPATH=C:\Workspace\e2studio\git
SET "targetLine=57"
SET "file=%PROJPATH%\r_flash_loader_rx\src\r_fl_app_header.c"

SET /A "skipTo=%targetLine%-1"
for /f "skip=%skipTo% tokens=*" %%a In (%file%) do SET "VERSION=%%a" & GoTo:next
:next
SET VERSION=%VERSION:,=.%
echo %VERSION%
SET PROJPATHNAME=%PROJPATH%\%PROJNAME%_build\%PROJNAME%_proj.mot
SET GRAPHICPATH=C:\Users\lafonso01\AppData\Roaming\Nextion Editor\bianyi
SET PROJOUTPUT=%OUTPUTPATH%\bin\%PROJNAME%v%VERSION%.bin
SET GRAPHICOUTPUT=%OUTPUTPATH%\bin\%PROJNAME%v%VERSION%.tft

del /q "%OUTPUTPATH%\bin\*.*"

%OUTPUTPATH%\Scripts\objcopy -I srec -O binary --gap-fill=0xFF "%PROJPATHNAME%" "%PROJOUTPUT%"

%OUTPUTPATH%\Scripts\srec_cat %WORKSPACEPATH%\MT01_Bootloader\Debug\MT01_Bootloader.mot -exclude -within %PROJPATHNAME% %PROJPATHNAME% -o %OUTPUTPATH%\bin\%PROJNAME%v%VERSION%_ba.mot

copy "%GRAPHICPATH%\%PROJNAME%.tft" "%GRAPHICOUTPUT%" 

for /d %%X in (%OUTPUTPATH%\bin) do (for /d %%a in (%%X) do ( "C:\Program Files\7-Zip\7z.exe" a -tzip "%OUTPUTPATH%\bin\%PROJNAME%v%VERSION%.zip" "%OUTPUTPATH%\bin" ))