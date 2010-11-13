# Microsoft Developer Studio Project File - Name="am2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=am2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "am2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "am2.mak" CFG="am2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "am2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "am2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "am2 - Win32 German" (based on "Win32 (x86) Application")
!MESSAGE "am2 - Win32 French" (based on "Win32 (x86) Application")
!MESSAGE "am2 - Win32 Italian" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "am2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpeg.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "am2___Win32_Debug"
# PROP BASE Intermediate_Dir "am2___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /W3 /Gm /GX /ZI /Od /I "d:\am2" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "DEBUG" /D "BETA" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib immultr5.lib jpeg.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "am2 - Win32 German"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "am2___Win32_German"
# PROP BASE Intermediate_Dir "am2___Win32_German"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "German"
# PROP Intermediate_Dir "German"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /D "GERMAN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"Release\am2.res" /d "NDEBUG"
# ADD RSC /l 0x409 /fo"Release\am2.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpeg.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpegd.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "am2 - Win32 French"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "am2___Win32_French"
# PROP BASE Intermediate_Dir "am2___Win32_French"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "French"
# PROP Intermediate_Dir "French"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /D "GERMAN" /YX /FD /c
# ADD CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /D "FRENCH" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"Release\am2.res" /d "NDEBUG"
# ADD RSC /l 0x409 /fo"Release\am2.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpegd.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpegd.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ELSEIF  "$(CFG)" == "am2 - Win32 Italian"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "am2___Win32_Italian"
# PROP BASE Intermediate_Dir "am2___Win32_Italian"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Italian"
# PROP Intermediate_Dir "Italian"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Oy /I "d:\am2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FLIP" /D "NO_MEM_CLASS" /D "ITALIAN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpeg.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib jpeg.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes /debug

!ENDIF 

# Begin Target

# Name "am2 - Win32 Release"
# Name "am2 - Win32 Debug"
# Name "am2 - Win32 German"
# Name "am2 - Win32 French"
# Name "am2 - Win32 Italian"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Am.cpp
# End Source File
# Begin Source File

SOURCE=.\Am2.rc
# End Source File
# Begin Source File

SOURCE=.\Oai_act.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_act2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_attk.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_buil.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_cap2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_capt.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_defe.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_dipl.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_eco.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_gran.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_info.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_main.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_mili.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_quer.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_seek.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_spy.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_talk.cpp
# End Source File
# Begin Source File

SOURCE=.\Oai_unit.cpp
# End Source File
# Begin Source File

SOURCE=.\Oanline.cpp
# End Source File
# Begin Source File

SOURCE=.\Oattack.cpp
# End Source File
# Begin Source File

SOURCE=.\Oaudio.cpp
# End Source File
# Begin Source File

SOURCE=.\Ob_flame.cpp
# End Source File
# Begin Source File

SOURCE=.\Ob_homin.cpp
# End Source File
# Begin Source File

SOURCE=.\Ob_prog.cpp
# End Source File
# Begin Source File

SOURCE=.\Ob_proj.cpp
# End Source File
# Begin Source File

SOURCE=.\Obaseoba.cpp
# End Source File
# Begin Source File

SOURCE=.\Obaseobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Obattle.cpp
# End Source File
# Begin Source File

SOURCE=.\Obitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Obitmapw.cpp
# End Source File
# Begin Source File

SOURCE=.\Oblob.cpp
# End Source File
# Begin Source File

SOURCE=.\Oblob2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oblob2w.cpp
# End Source File
# Begin Source File

SOURCE=.\Obox.cpp
# End Source File
# Begin Source File

SOURCE=.\Obullet.cpp
# End Source File
# Begin Source File

SOURCE=.\Obulleta.cpp
# End Source File
# Begin Source File

SOURCE=.\Obutt3d.cpp
# End Source File
# Begin Source File

SOURCE=.\Obuttcus.cpp
# End Source File
# Begin Source File

SOURCE=.\Obutton.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas10.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas11.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas12.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas13.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas14.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas15.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas16.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas17.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas18.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas19.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas20.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas21.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas22.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas23.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas24.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas25.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas26.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas27.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eas28.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east1.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east3.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east4.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east5.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east6.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east7.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east8.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_east9.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_easte.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_eastp.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla1.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla2.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla3.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla4.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla5.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla6.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pla7.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_plc1.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_plc2.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_plc3.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_plc4.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pld1.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pld2.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pld3.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pld4.cpp
# End Source File
# Begin Source File

SOURCE=.\oc_pld5.cpp
# End Source File
# Begin Source File

SOURCE=.\Oc_plot.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocampgn.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocampgn2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocampgn3.cpp
# End Source File
# Begin Source File

SOURCE=.\ocampgn4.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocnation.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocoltbl.cpp
# End Source File
# Begin Source File

SOURCE=.\Oconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocrc_sto.cpp
# End Source File
# Begin Source File

SOURCE=.\Odate.cpp
# End Source File
# Begin Source File

SOURCE=.\Odb.cpp
# End Source File
# Begin Source File

SOURCE=.\Odir.cpp
# End Source File
# Begin Source File

SOURCE=.\Odplay.cpp
# End Source File
# Begin Source File

SOURCE=.\Odynarr.cpp
# End Source File
# Begin Source File

SOURCE=.\Odynarrb.cpp
# End Source File
# Begin Source File

SOURCE=.\Oeffect.cpp
# End Source File
# Begin Source File

SOURCE=.\Oend_con.cpp
# End Source File
# Begin Source File

SOURCE=.\Oerrctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Oerror.cpp
# End Source File
# Begin Source File

SOURCE=.\oevent.cpp
# End Source File
# Begin Source File

SOURCE=.\Oexpmask.cpp
# End Source File
# Begin Source File

SOURCE=.\of_alch.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_alcha.cpp
# End Source File
# Begin Source File

SOURCE=.\of_alchi.cpp
# End Source File
# Begin Source File

SOURCE=.\of_anim.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_anima.cpp
# End Source File
# Begin Source File

SOURCE=.\of_animi.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_base.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_basea.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_basei.cpp
# End Source File
# Begin Source File

SOURCE=.\of_basew.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_camp.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_campa.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_campb.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_campi.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_camps.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_campu.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_fact.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_facta.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_facti.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_fort.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_forta.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_forti.cpp
# End Source File
# Begin Source File

SOURCE=.\of_incu.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_incua.cpp
# End Source File
# Begin Source File

SOURCE=.\of_incui.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_inn.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_inna.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_inni.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_lair.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_laira.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_lairi.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_lara2.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_lish.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_lishi.cpp
# End Source File
# Begin Source File

SOURCE=.\of_magi.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_magia.cpp
# End Source File
# Begin Source File

SOURCE=.\of_magii.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_mark.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_marka.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_marki.cpp
# End Source File
# Begin Source File

SOURCE=.\of_mfori.cpp
# End Source File
# Begin Source File

SOURCE=.\of_mfork.cpp
# End Source File
# Begin Source File

SOURCE=.\of_mfort.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_mine.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_minea.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_minei.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_mtrn.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_mtrna.cpp
# End Source File
# Begin Source File

SOURCE=.\of_mtrni.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_off.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_off2.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_off2i.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_offi.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_rese.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_resea.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_resei.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_spec.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_speca.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_speci.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_spy.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_spya.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_spyi.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_traia.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_traii.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_train.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_war.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_wara.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_wari.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_work.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_worka.cpp
# End Source File
# Begin Source File

SOURCE=.\Of_worki.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofile.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofiletxt.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirm.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirma.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmai.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmat.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmdie.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmdrw.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmif.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmlnk.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oflame.cpp
# End Source File
# Begin Source File

SOURCE=.\Oflc.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofont.cpp
# End Source File
# Begin Source File

SOURCE=.\Oformat.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamcamp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamcred.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamdemo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogame.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamecon.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamemp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamend.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogameset.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamhall.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogammain.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamsce2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamscen.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamsing.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogenhill.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogenmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogenmap2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oget.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogeta.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogf_v1.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile3.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfilea.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogodres.cpp
# End Source File
# Begin Source File

SOURCE=.\Ohelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Oherores.cpp
# End Source File
# Begin Source File

SOURCE=.\Ohillres.cpp
# End Source File
# Begin Source File

SOURCE=.\Ohsetres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oimgres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oimmplay.cpp
# End Source File
# Begin Source File

SOURCE=.\Oinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Oingmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Oisoarea.cpp
# End Source File
# Begin Source File

SOURCE=.\Oitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Oitemres.cpp
# End Source File
# Begin Source File

SOURCE=.\ojpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\Olightn.cpp
# End Source File
# Begin Source File

SOURCE=.\Olightn2.cpp
# End Source File
# Begin Source File

SOURCE=.\Olinear.cpp
# End Source File
# Begin Source File

SOURCE=.\Olocate.cpp
# End Source File
# Begin Source File

SOURCE=.\Olog.cpp
# End Source File
# Begin Source File

SOURCE=.\Olonglog.cpp
# End Source File
# Begin Source File

SOURCE=.\Olzw.cpp
# End Source File
# Begin Source File

SOURCE=.\Omagic.cpp
# End Source File
# Begin Source File

SOURCE=.\Omatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Omatrix2.cpp
# End Source File
# Begin Source File

SOURCE=.\Omem.cpp
# End Source File
# Begin Source File

SOURCE=.\Omisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Omlink.cpp
# End Source File
# Begin Source File

SOURCE=.\Omodeid.cpp
# End Source File
# Begin Source File

SOURCE=.\Omonsres.cpp
# End Source File
# Begin Source File

SOURCE=.\Omouse.cpp
# End Source File
# Begin Source File

SOURCE=.\Omousecr.cpp
# End Source File
# Begin Source File

SOURCE=.\Omousefr.cpp
# End Source File
# Begin Source File

SOURCE=.\Omousege.cpp
# End Source File
# Begin Source File

SOURCE=.\omouseit.cpp
# End Source File
# Begin Source File

SOURCE=.\omouseng.cpp
# End Source File
# Begin Source File

SOURCE=.\Omousesp.cpp
# End Source File
# Begin Source File

SOURCE=.\Omp_crc.cpp
# End Source File
# Begin Source File

SOURCE=.\Omusic.cpp
# End Source File
# Begin Source File

SOURCE=.\On_human.cpp
# End Source File
# Begin Source File

SOURCE=.\On_mons.cpp
# End Source File
# Begin Source File

SOURCE=.\Onationa.cpp
# End Source File
# Begin Source File

SOURCE=.\Onationb.cpp
# End Source File
# Begin Source File

SOURCE=.\Onews.cpp
# End Source File
# Begin Source File

SOURCE=.\Onews2.cpp
# End Source File
# Begin Source File

SOURCE=.\Onewseng.cpp
# End Source File
# Begin Source File

SOURCE=.\Ooptmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Opfind.cpp
# End Source File
# Begin Source File

SOURCE=.\Oplace.cpp
# End Source File
# Begin Source File

SOURCE=.\Oplant.cpp
# End Source File
# Begin Source File

SOURCE=.\Oplasma.cpp
# End Source File
# Begin Source File

SOURCE=.\Opower.cpp
# End Source File
# Begin Source File

SOURCE=.\Opower2.cpp
# End Source File
# Begin Source File

SOURCE=.\Opreuse.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprofil2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprofile.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_eco.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_mil.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_nat.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_news.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_rank.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_spy.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_tech.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_town.cpp
# End Source File
# Begin Source File

SOURCE=.\Or_trade.cpp
# End Source File
# Begin Source File

SOURCE=.\Oraceres.cpp
# End Source File
# Begin Source File

SOURCE=.\Orain1.cpp
# End Source File
# Begin Source File

SOURCE=.\Orain2.cpp
# End Source File
# Begin Source File

SOURCE=.\Orain3.cpp
# End Source File
# Begin Source File

SOURCE=.\Orawres.cpp
# End Source File
# Begin Source File

SOURCE=.\Orebel.cpp
# End Source File
# Begin Source File

SOURCE=.\Oregion.cpp
# End Source File
# Begin Source File

SOURCE=.\Oregions.cpp
# End Source File
# Begin Source File

SOURCE=.\Oremote.cpp
# End Source File
# Begin Source File

SOURCE=.\Oremote2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oremotem.cpp
# End Source File
# Begin Source File

SOURCE=.\Oremoteq.cpp
# End Source File
# Begin Source File

SOURCE=.\Ores.cpp
# End Source File
# Begin Source File

SOURCE=.\Oresdb.cpp
# End Source File
# Begin Source File

SOURCE=.\Orespal.cpp
# End Source File
# Begin Source File

SOURCE=.\Orestxt.cpp
# End Source File
# Begin Source File

SOURCE=.\Oresx.cpp
# End Source File
# Begin Source File

SOURCE=.\orle.cpp
# End Source File
# Begin Source File

SOURCE=.\Orock.cpp
# End Source File
# Begin Source File

SOURCE=.\Orockres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oscroll.cpp
# End Source File
# Begin Source File

SOURCE=.\Ose.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_e.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_f.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_h.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_m.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_o.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_p.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_r.cpp
# End Source File
# Begin Source File

SOURCE=.\Osedit_v.cpp
# End Source File
# Begin Source File

SOURCE=.\Oseditor.cpp
# End Source File
# Begin Source File

SOURCE=.\Oseres.cpp
# End Source File
# Begin Source File

SOURCE=.\oserial.cpp
# End Source File
# Begin Source File

SOURCE=.\Osfrmres.cpp
# End Source File
# Begin Source File

SOURCE=.\Osite.cpp
# End Source File
# Begin Source File

SOURCE=.\Ositedrw.cpp
# End Source File
# Begin Source File

SOURCE=.\Oskill.cpp
# End Source File
# Begin Source File

SOURCE=.\Oslidcus.cpp
# End Source File
# Begin Source File

SOURCE=.\Oslider.cpp
# End Source File
# Begin Source File

SOURCE=.\Osnow1.cpp
# End Source File
# Begin Source File

SOURCE=.\Osnow2.cpp
# End Source File
# Begin Source File

SOURCE=.\Osnowg.cpp
# End Source File
# Begin Source File

SOURCE=.\Osnowres.cpp
# End Source File
# Begin Source File

SOURCE=.\Osoldier.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospin_s.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospinner.cpp
# End Source File
# Begin Source File

SOURCE=.\Osprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Osprite2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospritea.cpp
# End Source File
# Begin Source File

SOURCE=.\Osprtres.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospy.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospy2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospya.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostate.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostr.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysdet.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysinit.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysproc.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysseed.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysview.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_basic.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_camp.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_endc.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_firm.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_gmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_item.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_news.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_reps.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_sedit.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_talk.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_tutor.cpp
# End Source File
# Begin Source File

SOURCE=.\ot_unit.cpp
# End Source File
# Begin Source File

SOURCE=.\Otalkeng.cpp
# End Source File
# Begin Source File

SOURCE=.\Otalkmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Otalkres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oteam.cpp
# End Source File
# Begin Source File

SOURCE=.\Otechres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oterrain.cpp
# End Source File
# Begin Source File

SOURCE=.\otips.cpp
# End Source File
# Begin Source File

SOURCE=.\Otornado.cpp
# End Source File
# Begin Source File

SOURCE=.\Otown.cpp
# End Source File
# Begin Source File

SOURCE=.\Otowna.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownai.cpp
# End Source File
# Begin Source File

SOURCE=.\otownat.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownbld.cpp
# End Source File
# Begin Source File

SOURCE=.\Otowndef.cpp
# End Source File
# Begin Source File

SOURCE=.\Otowndrw.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownif.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownind.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownlnk.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownpop.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownres.cpp
# End Source File
# Begin Source File

SOURCE=.\Otownwal.cpp
# End Source File
# Begin Source File

SOURCE=.\Otrainee.cpp
# End Source File
# Begin Source File

SOURCE=.\Otransl.cpp
# End Source File
# Begin Source File

SOURCE=.\Otutor.cpp
# End Source File
# Begin Source File

SOURCE=.\Otutor2.cpp
# End Source File
# Begin Source File

SOURCE=.\otutor3.cpp
# End Source File
# Begin Source File

SOURCE=.\Otwalres.cpp
# End Source File
# Begin Source File

SOURCE=.\otxtres.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_cara.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_caraa.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_carai.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_caras.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_carat.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_cart.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_god.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_god2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_godi.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_mons.cpp
# End Source File
# Begin Source File

SOURCE=.\Ou_wagon.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_act.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_ai2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_atk.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_atk2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_drw.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_grp.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_hero.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_ind.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_info.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_init.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_item.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_mode.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_ord.cpp
# End Source File
# Begin Source File

SOURCE=.\Oun_proc.cpp
# End Source File
# Begin Source File

SOURCE=.\oun_spab.cpp
# End Source File
# Begin Source File

SOURCE=.\Ounadraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Ounita.cpp
# End Source File
# Begin Source File

SOURCE=.\Ounitb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ounitres.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovbrowi2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovbrowif.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovbrowse.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovga.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovga2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovgabuf.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovgabuf2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovgalock.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovideo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovolume.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\Ow_fire.cpp
# End Source File
# Begin Source File

SOURCE=.\Ow_plant.cpp
# End Source File
# Begin Source File

SOURCE=.\Ow_rock.cpp
# End Source File
# Begin Source File

SOURCE=.\Ow_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\ow_trv.cpp
# End Source File
# Begin Source File

SOURCE=.\Ow_wall.cpp
# End Source File
# Begin Source File

SOURCE=.\Owallres.cpp
# End Source File
# Begin Source File

SOURCE=.\Owarpt.cpp
# End Source File
# Begin Source File

SOURCE=.\Owaypnt.cpp
# End Source File
# Begin Source File

SOURCE=.\Oweather.cpp
# End Source File
# Begin Source File

SOURCE=.\Oworld.cpp
# End Source File
# Begin Source File

SOURCE=.\Oworld_m.cpp
# End Source File
# Begin Source File

SOURCE=.\Oworld_z.cpp
# End Source File
# Begin Source File

SOURCE=.\Tfanimat.c
# End Source File
# Begin Source File

SOURCE=.\Tfbuffer.c
# End Source File
# Begin Source File

SOURCE=.\Tfframe.c
# End Source File
# Begin Source File

SOURCE=.\Tflib.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\7kii-02.ico"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ij_tr.obj
# End Source File
# Begin Source File

SOURCE=.\i_alpha.obj
# End Source File
# Begin Source File

SOURCE=.\I_BAR.obj
# End Source File
# Begin Source File

SOURCE=.\I_BRIGHT.obj
# End Source File
# Begin Source File

SOURCE=.\I_CTRL.obj
# End Source File
# Begin Source File

SOURCE=.\I_EFFECT.obj
# End Source File
# Begin Source File

SOURCE=.\I_LINE.obj
# End Source File
# Begin Source File

SOURCE=.\I_READ.obj
# End Source File
# Begin Source File

SOURCE=.\IB.obj
# End Source File
# Begin Source File

SOURCE=.\IB_A.obj
# End Source File
# Begin Source File

SOURCE=.\ib_ahrd.obj
# End Source File
# Begin Source File

SOURCE=.\ib_ahrdm.obj
# End Source File
# Begin Source File

SOURCE=.\IB_AR.obj
# End Source File
# Begin Source File

SOURCE=.\IB_ARM.obj
# End Source File
# Begin Source File

SOURCE=.\IB_AT.obj
# End Source File
# Begin Source File

SOURCE=.\IB_ATK.obj
# End Source File
# Begin Source File

SOURCE=.\IB_ATR.obj
# End Source File
# Begin Source File

SOURCE=.\IB_ATRD.obj
# End Source File
# Begin Source File

SOURCE=.\IB_ATRDM.obj
# End Source File
# Begin Source File

SOURCE=.\IB_ATRM.obj
# End Source File
# Begin Source File

SOURCE=.\ib_bar.obj
# End Source File
# Begin Source File

SOURCE=.\ib_barm.obj
# End Source File
# Begin Source File

SOURCE=.\ib_br.obj
# End Source File
# Begin Source File

SOURCE=.\IB_BRM.obj
# End Source File
# Begin Source File

SOURCE=.\ib_hrd.obj
# End Source File
# Begin Source File

SOURCE=.\ib_hrdm.obj
# End Source File
# Begin Source File

SOURCE=.\IB_R.obj
# End Source File
# Begin Source File

SOURCE=.\IB_RM.obj
# End Source File
# Begin Source File

SOURCE=.\IB_T.obj
# End Source File
# Begin Source File

SOURCE=.\IB_TK.obj
# End Source File
# Begin Source File

SOURCE=.\IB_TR.obj
# End Source File
# Begin Source File

SOURCE=.\IB_TRD.obj
# End Source File
# Begin Source File

SOURCE=.\IB_TRDM.obj
# End Source File
# Begin Source File

SOURCE=.\IB_TRM.obj
# End Source File
# Begin Source File

SOURCE=.\ib_war.obj
# End Source File
# Begin Source File

SOURCE=.\ib_warm.obj
# End Source File
# Begin Source File

SOURCE=.\ib_wr.obj
# End Source File
# Begin Source File

SOURCE=.\ib_wrm.obj
# End Source File
# Begin Source File

SOURCE=.\IC.obj
# End Source File
# Begin Source File

SOURCE=.\crc.obj
# End Source File
# Begin Source File

SOURCE=.\I_SCROLL.obj
# End Source File
# End Target
# End Project
