rem net use n: \\ban2\c
rem net use n: \\enlighteva\d
rem net use n: \\enlight2002\d
rem net use n: \\cheri\e
rem net use n: \\dennis\c
rem net use n: \\juliet\d
rem net use n: \\felix\scan
net use n: \\m0n\c
rem net use n: \\chris\c
rem net use n: \\fiona\d
rem net use n: \\ronald\e
rem net use n: \\dennis1\d

md n:\am2
md n:\am2\tutorial
md n:\am2\firm
md n:\am2\firmsml
md n:\am2\resource
md n:\am2\sprite
md n:\am2\image
md n:\am2\sound
md n:\am2\save
md n:\am2\campaign

xcopy /d release\am2.exe n:\am2
xcopy /d am2.exe n:\am2
xcopy /d /s /i x:\am2\tutorial n:\am2\tutorial
xcopy /d /s /i resource n:\am2\resource
xcopy /d /s /i sprite n:\am2\sprite
xcopy /d /s /i image n:\am2\image
xcopy /d /s /i sound n:\am2\sound
xcopy /d /s /i firm n:\am2\firm
xcopy /d /s /i firmsml n:\am2\firmsml
xcopy /d /s /i x:\am2\campaign n:\am2\campaign

rem del n:\am2\sprite\dragon.spr
rem del n:\am2\sprite\kukulcan.spr
rem del n:\am2\sprite\pheonix.spr
rem del n:\am2\sprite\chdragon.spr
rem del n:\am2\sprite\pehealer.spr
rem del n:\am2\sprite\firekin.spr
rem del n:\am2\sprite\gnoll.spr

rem xcopy x:\am2\am2.exe n:\am2
rem xcopy x:\am2\resource n:\am2\resource
rem xcopy x:\am2\sprite n:\am2\sprite
rem xcopy x:\am2\image n:\am2\image
rem xcopy x:\am2\sound n:\am2\sound
rem xcopy x:\am2\firm n:\am2\sound

rem xcopy /d am2.exe n:\7K2
rem xcopy /d /s /i resource n:\7K2\resource
rem xcopy /d /s /i sprite n:\7K2\sprite
rem xcopy /d /s /i image n:\7K2\image
rem xcopy /d /s /i sound n:\7K2\sound
rem xcopy /d /s /i firm n:\7K2\firm
rem xcopy /d /s /i campaign n:\7K2\campaign

rem md n:\7K2BAN
rem md n:\7K2BAN\firm
rem md n:\7K2BAN\resource
rem md n:\7K2BAN\sprite
rem md n:\7K2BAN\image
rem md n:\7K2BAN\sound
rem md n:\7K2BAN\save
rem md n:\7K2BAN\campaign

rem xcopy /d am2.exe n:\7K2BAN
rem xcopy /d /s /i resource n:\7K2BAN\resource
rem xcopy /d /s /i sprite n:\7K2BAN\sprite
rem xcopy /d /s /i image n:\7K2BAN\image
rem xcopy /d /s /i sound n:\7K2BAN\sound
rem xcopy /d /s /i firm n:\7K2BAN\firm
rem xcopy /d /s /i campaign n:\7K2BAN\campaign

