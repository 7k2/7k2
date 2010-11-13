net use x: \\gilbert2\d
xcopy /d am2.exe x:\am2
xcopy /d /i release\am2.exe x:\am2\7k2.exe
rem xcopy /d german\am2.exe x:\am2_germ\7k2.exe

xcopy /d /s /i resource x:\am2\resource
xcopy /d /s /i sprite x:\am2\sprite
xcopy /d /s /i firm x:\am2\firm
xcopy /d /s /i image x:\am2\image
xcopy /d /s /i sound x:\am2\sound
xcopy /d /s /i campaign x:\am2\campaign
xcopy /d /s /i scenario x:\am2\scenario
xcopy /d /s /i tutorial x:\am2\tutorial

