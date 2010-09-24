# Microsoft Developer Studio Generated NMAKE File, Based on am2.dsp
!IF "$(CFG)" == ""
CFG=am2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to am2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "am2 - Win32 Release" && "$(CFG)" != "am2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "am2.mak" CFG="am2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "am2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "am2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "am2 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\am2.exe"

!ELSE 

ALL : "$(OUTDIR)\am2.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Am.obj"
	-@erase "$(INTDIR)\Oai_act.obj"
	-@erase "$(INTDIR)\Oai_act2.obj"
	-@erase "$(INTDIR)\Oai_attk.obj"
	-@erase "$(INTDIR)\Oai_buil.obj"
	-@erase "$(INTDIR)\Oai_cap2.obj"
	-@erase "$(INTDIR)\Oai_capt.obj"
	-@erase "$(INTDIR)\Oai_defe.obj"
	-@erase "$(INTDIR)\Oai_dipl.obj"
	-@erase "$(INTDIR)\Oai_eco.obj"
	-@erase "$(INTDIR)\Oai_god.obj"
	-@erase "$(INTDIR)\Oai_gran.obj"
	-@erase "$(INTDIR)\Oai_info.obj"
	-@erase "$(INTDIR)\Oai_main.obj"
	-@erase "$(INTDIR)\Oai_mar2.obj"
	-@erase "$(INTDIR)\Oai_mar3.obj"
	-@erase "$(INTDIR)\Oai_mari.obj"
	-@erase "$(INTDIR)\Oai_mili.obj"
	-@erase "$(INTDIR)\Oai_mons.obj"
	-@erase "$(INTDIR)\Oai_quer.obj"
	-@erase "$(INTDIR)\Oai_seek.obj"
	-@erase "$(INTDIR)\Oai_spy.obj"
	-@erase "$(INTDIR)\Oai_talk.obj"
	-@erase "$(INTDIR)\Oai_town.obj"
	-@erase "$(INTDIR)\Oai_trad.obj"
	-@erase "$(INTDIR)\Oai_unit.obj"
	-@erase "$(INTDIR)\Oanline.obj"
	-@erase "$(INTDIR)\Oaudio.obj"
	-@erase "$(INTDIR)\Ob_flame.obj"
	-@erase "$(INTDIR)\Ob_homin.obj"
	-@erase "$(INTDIR)\Ob_proj.obj"
	-@erase "$(INTDIR)\Obattle.obj"
	-@erase "$(INTDIR)\obitmap.obj"
	-@erase "$(INTDIR)\Oblob.obj"
	-@erase "$(INTDIR)\oblob2.obj"
	-@erase "$(INTDIR)\Obox.obj"
	-@erase "$(INTDIR)\Obullet.obj"
	-@erase "$(INTDIR)\Obulleta.obj"
	-@erase "$(INTDIR)\Obutt3d.obj"
	-@erase "$(INTDIR)\Obuttcus.obj"
	-@erase "$(INTDIR)\Obutton.obj"
	-@erase "$(INTDIR)\Ocoltbl.obj"
	-@erase "$(INTDIR)\Oconfig.obj"
	-@erase "$(INTDIR)\Ocrc_sto.obj"
	-@erase "$(INTDIR)\Odate.obj"
	-@erase "$(INTDIR)\Odb.obj"
	-@erase "$(INTDIR)\Odir.obj"
	-@erase "$(INTDIR)\Odplay.obj"
	-@erase "$(INTDIR)\Odynarr.obj"
	-@erase "$(INTDIR)\Odynarrb.obj"
	-@erase "$(INTDIR)\Oeffect.obj"
	-@erase "$(INTDIR)\Oerrctrl.obj"
	-@erase "$(INTDIR)\Oerror.obj"
	-@erase "$(INTDIR)\Oexpmask.obj"
	-@erase "$(INTDIR)\Of_base.obj"
	-@erase "$(INTDIR)\Of_base2.obj"
	-@erase "$(INTDIR)\Of_camp.obj"
	-@erase "$(INTDIR)\Of_camp2.obj"
	-@erase "$(INTDIR)\Of_fact.obj"
	-@erase "$(INTDIR)\Of_fact2.obj"
	-@erase "$(INTDIR)\Of_harb.obj"
	-@erase "$(INTDIR)\Of_harb2.obj"
	-@erase "$(INTDIR)\Of_inn.obj"
	-@erase "$(INTDIR)\Of_inn2.obj"
	-@erase "$(INTDIR)\Of_mark.obj"
	-@erase "$(INTDIR)\Of_mark2.obj"
	-@erase "$(INTDIR)\Of_mine.obj"
	-@erase "$(INTDIR)\Of_mine2.obj"
	-@erase "$(INTDIR)\Of_mons.obj"
	-@erase "$(INTDIR)\Of_rese.obj"
	-@erase "$(INTDIR)\Of_rese2.obj"
	-@erase "$(INTDIR)\Of_war.obj"
	-@erase "$(INTDIR)\Of_war2.obj"
	-@erase "$(INTDIR)\Ofile.obj"
	-@erase "$(INTDIR)\Ofiletxt.obj"
	-@erase "$(INTDIR)\Ofirm.obj"
	-@erase "$(INTDIR)\Ofirm2.obj"
	-@erase "$(INTDIR)\Ofirma.obj"
	-@erase "$(INTDIR)\Ofirmai.obj"
	-@erase "$(INTDIR)\Ofirmdie.obj"
	-@erase "$(INTDIR)\Ofirmdrw.obj"
	-@erase "$(INTDIR)\Ofirmif.obj"
	-@erase "$(INTDIR)\Ofirmif2.obj"
	-@erase "$(INTDIR)\Ofirmif3.obj"
	-@erase "$(INTDIR)\Ofirmres.obj"
	-@erase "$(INTDIR)\Oflame.obj"
	-@erase "$(INTDIR)\Ofont.obj"
	-@erase "$(INTDIR)\Ogamcred.obj"
	-@erase "$(INTDIR)\Ogamdemo.obj"
	-@erase "$(INTDIR)\Ogame.obj"
	-@erase "$(INTDIR)\Ogamemp.obj"
	-@erase "$(INTDIR)\Ogamency.obj"
	-@erase "$(INTDIR)\Ogamend.obj"
	-@erase "$(INTDIR)\Ogameset.obj"
	-@erase "$(INTDIR)\Ogamhall.obj"
	-@erase "$(INTDIR)\Ogammain.obj"
	-@erase "$(INTDIR)\Ogammenu.obj"
	-@erase "$(INTDIR)\Ogamsce2.obj"
	-@erase "$(INTDIR)\Ogamscen.obj"
	-@erase "$(INTDIR)\Ogamsing.obj"
	-@erase "$(INTDIR)\Ogenhill.obj"
	-@erase "$(INTDIR)\Ogenmap.obj"
	-@erase "$(INTDIR)\Ogenmap2.obj"
	-@erase "$(INTDIR)\Oget.obj"
	-@erase "$(INTDIR)\Ogeta.obj"
	-@erase "$(INTDIR)\Ogf_v1.obj"
	-@erase "$(INTDIR)\Ogfile.obj"
	-@erase "$(INTDIR)\Ogfile2.obj"
	-@erase "$(INTDIR)\Ogfile3.obj"
	-@erase "$(INTDIR)\Ogfilea.obj"
	-@erase "$(INTDIR)\Ogodres.obj"
	-@erase "$(INTDIR)\Ogrpsel.obj"
	-@erase "$(INTDIR)\Ohelp.obj"
	-@erase "$(INTDIR)\Ohillres.obj"
	-@erase "$(INTDIR)\Ohsetres.obj"
	-@erase "$(INTDIR)\Oimgres.obj"
	-@erase "$(INTDIR)\Oimmplay.obj"
	-@erase "$(INTDIR)\Oinfo.obj"
	-@erase "$(INTDIR)\Oingmenu.obj"
	-@erase "$(INTDIR)\Oisoarea.obj"
	-@erase "$(INTDIR)\Olightn.obj"
	-@erase "$(INTDIR)\Olightn2.obj"
	-@erase "$(INTDIR)\olocbmp.obj"
	-@erase "$(INTDIR)\Olog.obj"
	-@erase "$(INTDIR)\Olonglog.obj"
	-@erase "$(INTDIR)\Olzw.obj"
	-@erase "$(INTDIR)\Omatrix.obj"
	-@erase "$(INTDIR)\omatrix2.obj"
	-@erase "$(INTDIR)\Omem.obj"
	-@erase "$(INTDIR)\Omisc.obj"
	-@erase "$(INTDIR)\Omlink.obj"
	-@erase "$(INTDIR)\Omonsres.obj"
	-@erase "$(INTDIR)\Omouse.obj"
	-@erase "$(INTDIR)\Omousecr.obj"
	-@erase "$(INTDIR)\Omousefr.obj"
	-@erase "$(INTDIR)\Omousege.obj"
	-@erase "$(INTDIR)\Omousesp.obj"
	-@erase "$(INTDIR)\Omp_crc.obj"
	-@erase "$(INTDIR)\Omusic.obj"
	-@erase "$(INTDIR)\Onationa.obj"
	-@erase "$(INTDIR)\Onationb.obj"
	-@erase "$(INTDIR)\Onews.obj"
	-@erase "$(INTDIR)\Onews2.obj"
	-@erase "$(INTDIR)\Onewseng.obj"
	-@erase "$(INTDIR)\Onewsfre.obj"
	-@erase "$(INTDIR)\Onewsger.obj"
	-@erase "$(INTDIR)\Onewsspa.obj"
	-@erase "$(INTDIR)\Ooptmenu.obj"
	-@erase "$(INTDIR)\Oplant.obj"
	-@erase "$(INTDIR)\Oplasma.obj"
	-@erase "$(INTDIR)\Opower.obj"
	-@erase "$(INTDIR)\Or_ai.obj"
	-@erase "$(INTDIR)\Or_eco.obj"
	-@erase "$(INTDIR)\Or_mil.obj"
	-@erase "$(INTDIR)\Or_nat.obj"
	-@erase "$(INTDIR)\Or_news.obj"
	-@erase "$(INTDIR)\Or_rank.obj"
	-@erase "$(INTDIR)\Or_spy.obj"
	-@erase "$(INTDIR)\Or_tech.obj"
	-@erase "$(INTDIR)\Or_town.obj"
	-@erase "$(INTDIR)\Or_trade.obj"
	-@erase "$(INTDIR)\Oraceres.obj"
	-@erase "$(INTDIR)\Orain1.obj"
	-@erase "$(INTDIR)\Orain2.obj"
	-@erase "$(INTDIR)\Orain3.obj"
	-@erase "$(INTDIR)\Orawres.obj"
	-@erase "$(INTDIR)\Orebel.obj"
	-@erase "$(INTDIR)\Oregion.obj"
	-@erase "$(INTDIR)\Oregions.obj"
	-@erase "$(INTDIR)\Oremote.obj"
	-@erase "$(INTDIR)\Oremote2.obj"
	-@erase "$(INTDIR)\Oremotem.obj"
	-@erase "$(INTDIR)\Oremoteq.obj"
	-@erase "$(INTDIR)\Ores.obj"
	-@erase "$(INTDIR)\Oresdb.obj"
	-@erase "$(INTDIR)\Orestxt.obj"
	-@erase "$(INTDIR)\Oresx.obj"
	-@erase "$(INTDIR)\Orock.obj"
	-@erase "$(INTDIR)\Orockres.obj"
	-@erase "$(INTDIR)\Oscroll.obj"
	-@erase "$(INTDIR)\Ose.obj"
	-@erase "$(INTDIR)\Oseres.obj"
	-@erase "$(INTDIR)\Osfrmres.obj"
	-@erase "$(INTDIR)\Osite.obj"
	-@erase "$(INTDIR)\Ositedrw.obj"
	-@erase "$(INTDIR)\Oskill.obj"
	-@erase "$(INTDIR)\Oslidcus.obj"
	-@erase "$(INTDIR)\Oslider.obj"
	-@erase "$(INTDIR)\Osnow1.obj"
	-@erase "$(INTDIR)\Osnow2.obj"
	-@erase "$(INTDIR)\Osnowg.obj"
	-@erase "$(INTDIR)\Osnowres.obj"
	-@erase "$(INTDIR)\Ospath.obj"
	-@erase "$(INTDIR)\Ospathbt.obj"
	-@erase "$(INTDIR)\Ospinner.obj"
	-@erase "$(INTDIR)\Ospredbg.obj"
	-@erase "$(INTDIR)\Ospreoff.obj"
	-@erase "$(INTDIR)\Ospresmo.obj"
	-@erase "$(INTDIR)\Ospreuse.obj"
	-@erase "$(INTDIR)\Osprite.obj"
	-@erase "$(INTDIR)\Osprite2.obj"
	-@erase "$(INTDIR)\Ospritea.obj"
	-@erase "$(INTDIR)\Osprtres.obj"
	-@erase "$(INTDIR)\Ospy.obj"
	-@erase "$(INTDIR)\Ospy2.obj"
	-@erase "$(INTDIR)\Ospya.obj"
	-@erase "$(INTDIR)\Ostr.obj"
	-@erase "$(INTDIR)\Osys.obj"
	-@erase "$(INTDIR)\Osys2.obj"
	-@erase "$(INTDIR)\Osys3.obj"
	-@erase "$(INTDIR)\Otalkeng.obj"
	-@erase "$(INTDIR)\Otalkfre.obj"
	-@erase "$(INTDIR)\Otalkger.obj"
	-@erase "$(INTDIR)\Otalkmsg.obj"
	-@erase "$(INTDIR)\Otalkres.obj"
	-@erase "$(INTDIR)\Otalkspa.obj"
	-@erase "$(INTDIR)\Otechres.obj"
	-@erase "$(INTDIR)\Oterrain.obj"
	-@erase "$(INTDIR)\Otornado.obj"
	-@erase "$(INTDIR)\Otown.obj"
	-@erase "$(INTDIR)\Otowna.obj"
	-@erase "$(INTDIR)\Otownai.obj"
	-@erase "$(INTDIR)\Otownbld.obj"
	-@erase "$(INTDIR)\Otowndrw.obj"
	-@erase "$(INTDIR)\Otownif.obj"
	-@erase "$(INTDIR)\Otownind.obj"
	-@erase "$(INTDIR)\Otownres.obj"
	-@erase "$(INTDIR)\Otransl.obj"
	-@erase "$(INTDIR)\Otutor.obj"
	-@erase "$(INTDIR)\Otutor2.obj"
	-@erase "$(INTDIR)\Ou_cara.obj"
	-@erase "$(INTDIR)\Ou_cara2.obj"
	-@erase "$(INTDIR)\Ou_caras.obj"
	-@erase "$(INTDIR)\Ou_carat.obj"
	-@erase "$(INTDIR)\Ou_cart.obj"
	-@erase "$(INTDIR)\Ou_god.obj"
	-@erase "$(INTDIR)\Ou_god2.obj"
	-@erase "$(INTDIR)\Ou_mari.obj"
	-@erase "$(INTDIR)\Ou_mari2.obj"
	-@erase "$(INTDIR)\Ou_marif.obj"
	-@erase "$(INTDIR)\Ou_maris.obj"
	-@erase "$(INTDIR)\Ou_marit.obj"
	-@erase "$(INTDIR)\Ou_mons.obj"
	-@erase "$(INTDIR)\Ou_vehi.obj"
	-@erase "$(INTDIR)\Ounit.obj"
	-@erase "$(INTDIR)\Ounit2.obj"
	-@erase "$(INTDIR)\Ounita.obj"
	-@erase "$(INTDIR)\Ounitaac.obj"
	-@erase "$(INTDIR)\Ounitaat.obj"
	-@erase "$(INTDIR)\Ounitac.obj"
	-@erase "$(INTDIR)\Ounitai.obj"
	-@erase "$(INTDIR)\Ounitam.obj"
	-@erase "$(INTDIR)\Ounitamt.obj"
	-@erase "$(INTDIR)\Ounitat.obj"
	-@erase "$(INTDIR)\Ounitat2.obj"
	-@erase "$(INTDIR)\Ounitat3.obj"
	-@erase "$(INTDIR)\Ounitatb.obj"
	-@erase "$(INTDIR)\Ounitd.obj"
	-@erase "$(INTDIR)\Ounitdrw.obj"
	-@erase "$(INTDIR)\Ounithb.obj"
	-@erase "$(INTDIR)\Ouniti.obj"
	-@erase "$(INTDIR)\Ounitif.obj"
	-@erase "$(INTDIR)\Ounitind.obj"
	-@erase "$(INTDIR)\Ounitm.obj"
	-@erase "$(INTDIR)\Ounitres.obj"
	-@erase "$(INTDIR)\Ounits.obj"
	-@erase "$(INTDIR)\Ovbrowif.obj"
	-@erase "$(INTDIR)\Ovbrowse.obj"
	-@erase "$(INTDIR)\Ovga.obj"
	-@erase "$(INTDIR)\Ovga2.obj"
	-@erase "$(INTDIR)\Ovgabuf.obj"
	-@erase "$(INTDIR)\Ovgabuf2.obj"
	-@erase "$(INTDIR)\Ovgalock.obj"
	-@erase "$(INTDIR)\Ovideo.obj"
	-@erase "$(INTDIR)\Ovolume.obj"
	-@erase "$(INTDIR)\Ovqueue.obj"
	-@erase "$(INTDIR)\Ow_fire.obj"
	-@erase "$(INTDIR)\Ow_plant.obj"
	-@erase "$(INTDIR)\Ow_rock.obj"
	-@erase "$(INTDIR)\Ow_sound.obj"
	-@erase "$(INTDIR)\Ow_wall.obj"
	-@erase "$(INTDIR)\Owallres.obj"
	-@erase "$(INTDIR)\Owarpt.obj"
	-@erase "$(INTDIR)\Oweather.obj"
	-@erase "$(INTDIR)\Oworld.obj"
	-@erase "$(INTDIR)\Oworld_m.obj"
	-@erase "$(INTDIR)\Oworld_z.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\am2.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Od /I "d:\am2" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "IMAGICMP" /Fp"$(INTDIR)\am2.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\am2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib dxguid.lib ddraw.lib\
 dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib immultr5.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\am2.pdb" /machine:I386\
 /out:"$(OUTDIR)\am2.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Am.obj" \
	"$(INTDIR)\Oai_act.obj" \
	"$(INTDIR)\Oai_act2.obj" \
	"$(INTDIR)\Oai_attk.obj" \
	"$(INTDIR)\Oai_buil.obj" \
	"$(INTDIR)\Oai_cap2.obj" \
	"$(INTDIR)\Oai_capt.obj" \
	"$(INTDIR)\Oai_defe.obj" \
	"$(INTDIR)\Oai_dipl.obj" \
	"$(INTDIR)\Oai_eco.obj" \
	"$(INTDIR)\Oai_god.obj" \
	"$(INTDIR)\Oai_gran.obj" \
	"$(INTDIR)\Oai_info.obj" \
	"$(INTDIR)\Oai_main.obj" \
	"$(INTDIR)\Oai_mar2.obj" \
	"$(INTDIR)\Oai_mar3.obj" \
	"$(INTDIR)\Oai_mari.obj" \
	"$(INTDIR)\Oai_mili.obj" \
	"$(INTDIR)\Oai_mons.obj" \
	"$(INTDIR)\Oai_quer.obj" \
	"$(INTDIR)\Oai_seek.obj" \
	"$(INTDIR)\Oai_spy.obj" \
	"$(INTDIR)\Oai_talk.obj" \
	"$(INTDIR)\Oai_town.obj" \
	"$(INTDIR)\Oai_trad.obj" \
	"$(INTDIR)\Oai_unit.obj" \
	"$(INTDIR)\Oanline.obj" \
	"$(INTDIR)\Oaudio.obj" \
	"$(INTDIR)\Ob_flame.obj" \
	"$(INTDIR)\Ob_homin.obj" \
	"$(INTDIR)\Ob_proj.obj" \
	"$(INTDIR)\Obattle.obj" \
	"$(INTDIR)\obitmap.obj" \
	"$(INTDIR)\Oblob.obj" \
	"$(INTDIR)\oblob2.obj" \
	"$(INTDIR)\Obox.obj" \
	"$(INTDIR)\Obullet.obj" \
	"$(INTDIR)\Obulleta.obj" \
	"$(INTDIR)\Obutt3d.obj" \
	"$(INTDIR)\Obuttcus.obj" \
	"$(INTDIR)\Obutton.obj" \
	"$(INTDIR)\Ocoltbl.obj" \
	"$(INTDIR)\Oconfig.obj" \
	"$(INTDIR)\Ocrc_sto.obj" \
	"$(INTDIR)\Odate.obj" \
	"$(INTDIR)\Odb.obj" \
	"$(INTDIR)\Odir.obj" \
	"$(INTDIR)\Odplay.obj" \
	"$(INTDIR)\Odynarr.obj" \
	"$(INTDIR)\Odynarrb.obj" \
	"$(INTDIR)\Oeffect.obj" \
	"$(INTDIR)\Oerrctrl.obj" \
	"$(INTDIR)\Oerror.obj" \
	"$(INTDIR)\Oexpmask.obj" \
	"$(INTDIR)\Of_base.obj" \
	"$(INTDIR)\Of_base2.obj" \
	"$(INTDIR)\Of_camp.obj" \
	"$(INTDIR)\Of_camp2.obj" \
	"$(INTDIR)\Of_fact.obj" \
	"$(INTDIR)\Of_fact2.obj" \
	"$(INTDIR)\Of_harb.obj" \
	"$(INTDIR)\Of_harb2.obj" \
	"$(INTDIR)\Of_inn.obj" \
	"$(INTDIR)\Of_inn2.obj" \
	"$(INTDIR)\Of_mark.obj" \
	"$(INTDIR)\Of_mark2.obj" \
	"$(INTDIR)\Of_mine.obj" \
	"$(INTDIR)\Of_mine2.obj" \
	"$(INTDIR)\Of_mons.obj" \
	"$(INTDIR)\Of_rese.obj" \
	"$(INTDIR)\Of_rese2.obj" \
	"$(INTDIR)\Of_war.obj" \
	"$(INTDIR)\Of_war2.obj" \
	"$(INTDIR)\Ofile.obj" \
	"$(INTDIR)\Ofiletxt.obj" \
	"$(INTDIR)\Ofirm.obj" \
	"$(INTDIR)\Ofirm2.obj" \
	"$(INTDIR)\Ofirma.obj" \
	"$(INTDIR)\Ofirmai.obj" \
	"$(INTDIR)\Ofirmdie.obj" \
	"$(INTDIR)\Ofirmdrw.obj" \
	"$(INTDIR)\Ofirmif.obj" \
	"$(INTDIR)\Ofirmif2.obj" \
	"$(INTDIR)\Ofirmif3.obj" \
	"$(INTDIR)\Ofirmres.obj" \
	"$(INTDIR)\Oflame.obj" \
	"$(INTDIR)\Ofont.obj" \
	"$(INTDIR)\Ogamcred.obj" \
	"$(INTDIR)\Ogamdemo.obj" \
	"$(INTDIR)\Ogame.obj" \
	"$(INTDIR)\Ogamemp.obj" \
	"$(INTDIR)\Ogamency.obj" \
	"$(INTDIR)\Ogamend.obj" \
	"$(INTDIR)\Ogameset.obj" \
	"$(INTDIR)\Ogamhall.obj" \
	"$(INTDIR)\Ogammain.obj" \
	"$(INTDIR)\Ogammenu.obj" \
	"$(INTDIR)\Ogamsce2.obj" \
	"$(INTDIR)\Ogamscen.obj" \
	"$(INTDIR)\Ogamsing.obj" \
	"$(INTDIR)\Ogenhill.obj" \
	"$(INTDIR)\Ogenmap.obj" \
	"$(INTDIR)\Ogenmap2.obj" \
	"$(INTDIR)\Oget.obj" \
	"$(INTDIR)\Ogeta.obj" \
	"$(INTDIR)\Ogf_v1.obj" \
	"$(INTDIR)\Ogfile.obj" \
	"$(INTDIR)\Ogfile2.obj" \
	"$(INTDIR)\Ogfile3.obj" \
	"$(INTDIR)\Ogfilea.obj" \
	"$(INTDIR)\Ogodres.obj" \
	"$(INTDIR)\Ogrpsel.obj" \
	"$(INTDIR)\Ohelp.obj" \
	"$(INTDIR)\Ohillres.obj" \
	"$(INTDIR)\Ohsetres.obj" \
	"$(INTDIR)\Oimgres.obj" \
	"$(INTDIR)\Oimmplay.obj" \
	"$(INTDIR)\Oinfo.obj" \
	"$(INTDIR)\Oingmenu.obj" \
	"$(INTDIR)\Oisoarea.obj" \
	"$(INTDIR)\Olightn.obj" \
	"$(INTDIR)\Olightn2.obj" \
	"$(INTDIR)\olocbmp.obj" \
	"$(INTDIR)\Olog.obj" \
	"$(INTDIR)\Olonglog.obj" \
	"$(INTDIR)\Olzw.obj" \
	"$(INTDIR)\Omatrix.obj" \
	"$(INTDIR)\omatrix2.obj" \
	"$(INTDIR)\Omem.obj" \
	"$(INTDIR)\Omisc.obj" \
	"$(INTDIR)\Omlink.obj" \
	"$(INTDIR)\Omonsres.obj" \
	"$(INTDIR)\Omouse.obj" \
	"$(INTDIR)\Omousecr.obj" \
	"$(INTDIR)\Omousefr.obj" \
	"$(INTDIR)\Omousege.obj" \
	"$(INTDIR)\Omousesp.obj" \
	"$(INTDIR)\Omp_crc.obj" \
	"$(INTDIR)\Omusic.obj" \
	"$(INTDIR)\Onationa.obj" \
	"$(INTDIR)\Onationb.obj" \
	"$(INTDIR)\Onews.obj" \
	"$(INTDIR)\Onews2.obj" \
	"$(INTDIR)\Onewseng.obj" \
	"$(INTDIR)\Onewsfre.obj" \
	"$(INTDIR)\Onewsger.obj" \
	"$(INTDIR)\Onewsspa.obj" \
	"$(INTDIR)\Ooptmenu.obj" \
	"$(INTDIR)\Oplant.obj" \
	"$(INTDIR)\Oplasma.obj" \
	"$(INTDIR)\Opower.obj" \
	"$(INTDIR)\Or_ai.obj" \
	"$(INTDIR)\Or_eco.obj" \
	"$(INTDIR)\Or_mil.obj" \
	"$(INTDIR)\Or_nat.obj" \
	"$(INTDIR)\Or_news.obj" \
	"$(INTDIR)\Or_rank.obj" \
	"$(INTDIR)\Or_spy.obj" \
	"$(INTDIR)\Or_tech.obj" \
	"$(INTDIR)\Or_town.obj" \
	"$(INTDIR)\Or_trade.obj" \
	"$(INTDIR)\Oraceres.obj" \
	"$(INTDIR)\Orain1.obj" \
	"$(INTDIR)\Orain2.obj" \
	"$(INTDIR)\Orain3.obj" \
	"$(INTDIR)\Orawres.obj" \
	"$(INTDIR)\Orebel.obj" \
	"$(INTDIR)\Oregion.obj" \
	"$(INTDIR)\Oregions.obj" \
	"$(INTDIR)\Oremote.obj" \
	"$(INTDIR)\Oremote2.obj" \
	"$(INTDIR)\Oremotem.obj" \
	"$(INTDIR)\Oremoteq.obj" \
	"$(INTDIR)\Ores.obj" \
	"$(INTDIR)\Oresdb.obj" \
	"$(INTDIR)\Orestxt.obj" \
	"$(INTDIR)\Oresx.obj" \
	"$(INTDIR)\Orock.obj" \
	"$(INTDIR)\Orockres.obj" \
	"$(INTDIR)\Oscroll.obj" \
	"$(INTDIR)\Ose.obj" \
	"$(INTDIR)\Oseres.obj" \
	"$(INTDIR)\Osfrmres.obj" \
	"$(INTDIR)\Osite.obj" \
	"$(INTDIR)\Ositedrw.obj" \
	"$(INTDIR)\Oskill.obj" \
	"$(INTDIR)\Oslidcus.obj" \
	"$(INTDIR)\Oslider.obj" \
	"$(INTDIR)\Osnow1.obj" \
	"$(INTDIR)\Osnow2.obj" \
	"$(INTDIR)\Osnowg.obj" \
	"$(INTDIR)\Osnowres.obj" \
	"$(INTDIR)\Ospath.obj" \
	"$(INTDIR)\Ospathbt.obj" \
	"$(INTDIR)\Ospinner.obj" \
	"$(INTDIR)\Ospredbg.obj" \
	"$(INTDIR)\Ospreoff.obj" \
	"$(INTDIR)\Ospresmo.obj" \
	"$(INTDIR)\Ospreuse.obj" \
	"$(INTDIR)\Osprite.obj" \
	"$(INTDIR)\Osprite2.obj" \
	"$(INTDIR)\Ospritea.obj" \
	"$(INTDIR)\Osprtres.obj" \
	"$(INTDIR)\Ospy.obj" \
	"$(INTDIR)\Ospy2.obj" \
	"$(INTDIR)\Ospya.obj" \
	"$(INTDIR)\Ostr.obj" \
	"$(INTDIR)\Osys.obj" \
	"$(INTDIR)\Osys2.obj" \
	"$(INTDIR)\Osys3.obj" \
	"$(INTDIR)\Otalkeng.obj" \
	"$(INTDIR)\Otalkfre.obj" \
	"$(INTDIR)\Otalkger.obj" \
	"$(INTDIR)\Otalkmsg.obj" \
	"$(INTDIR)\Otalkres.obj" \
	"$(INTDIR)\Otalkspa.obj" \
	"$(INTDIR)\Otechres.obj" \
	"$(INTDIR)\Oterrain.obj" \
	"$(INTDIR)\Otornado.obj" \
	"$(INTDIR)\Otown.obj" \
	"$(INTDIR)\Otowna.obj" \
	"$(INTDIR)\Otownai.obj" \
	"$(INTDIR)\Otownbld.obj" \
	"$(INTDIR)\Otowndrw.obj" \
	"$(INTDIR)\Otownif.obj" \
	"$(INTDIR)\Otownind.obj" \
	"$(INTDIR)\Otownres.obj" \
	"$(INTDIR)\Otransl.obj" \
	"$(INTDIR)\Otutor.obj" \
	"$(INTDIR)\Otutor2.obj" \
	"$(INTDIR)\Ou_cara.obj" \
	"$(INTDIR)\Ou_cara2.obj" \
	"$(INTDIR)\Ou_caras.obj" \
	"$(INTDIR)\Ou_carat.obj" \
	"$(INTDIR)\Ou_cart.obj" \
	"$(INTDIR)\Ou_god.obj" \
	"$(INTDIR)\Ou_god2.obj" \
	"$(INTDIR)\Ou_mari.obj" \
	"$(INTDIR)\Ou_mari2.obj" \
	"$(INTDIR)\Ou_marif.obj" \
	"$(INTDIR)\Ou_maris.obj" \
	"$(INTDIR)\Ou_marit.obj" \
	"$(INTDIR)\Ou_mons.obj" \
	"$(INTDIR)\Ou_vehi.obj" \
	"$(INTDIR)\Ounit.obj" \
	"$(INTDIR)\Ounit2.obj" \
	"$(INTDIR)\Ounita.obj" \
	"$(INTDIR)\Ounitaac.obj" \
	"$(INTDIR)\Ounitaat.obj" \
	"$(INTDIR)\Ounitac.obj" \
	"$(INTDIR)\Ounitai.obj" \
	"$(INTDIR)\Ounitam.obj" \
	"$(INTDIR)\Ounitamt.obj" \
	"$(INTDIR)\Ounitat.obj" \
	"$(INTDIR)\Ounitat2.obj" \
	"$(INTDIR)\Ounitat3.obj" \
	"$(INTDIR)\Ounitatb.obj" \
	"$(INTDIR)\Ounitd.obj" \
	"$(INTDIR)\Ounitdrw.obj" \
	"$(INTDIR)\Ounithb.obj" \
	"$(INTDIR)\Ouniti.obj" \
	"$(INTDIR)\Ounitif.obj" \
	"$(INTDIR)\Ounitind.obj" \
	"$(INTDIR)\Ounitm.obj" \
	"$(INTDIR)\Ounitres.obj" \
	"$(INTDIR)\Ounits.obj" \
	"$(INTDIR)\Ovbrowif.obj" \
	"$(INTDIR)\Ovbrowse.obj" \
	"$(INTDIR)\Ovga.obj" \
	"$(INTDIR)\Ovga2.obj" \
	"$(INTDIR)\Ovgabuf.obj" \
	"$(INTDIR)\Ovgabuf2.obj" \
	"$(INTDIR)\Ovgalock.obj" \
	"$(INTDIR)\Ovideo.obj" \
	"$(INTDIR)\Ovolume.obj" \
	"$(INTDIR)\Ovqueue.obj" \
	"$(INTDIR)\Ow_fire.obj" \
	"$(INTDIR)\Ow_plant.obj" \
	"$(INTDIR)\Ow_rock.obj" \
	"$(INTDIR)\Ow_sound.obj" \
	"$(INTDIR)\Ow_wall.obj" \
	"$(INTDIR)\Owallres.obj" \
	"$(INTDIR)\Owarpt.obj" \
	"$(INTDIR)\Oweather.obj" \
	"$(INTDIR)\Oworld.obj" \
	"$(INTDIR)\Oworld_m.obj" \
	"$(INTDIR)\Oworld_z.obj" \
	".\CRC.obj" \
	".\I_BAR.obj" \
	".\I_BLACK.obj" \
	".\I_CTRL.obj" \
	".\I_EMASK.obj" \
	".\I_EREMAP.obj" \
	".\I_FONT.obj" \
	".\I_FREMAP.obj" \
	".\I_LINE.obj" \
	".\I_PIXEL.obj" \
	".\I_READ.obj" \
	".\I_SNOW.obj" \
	".\I_XOR.obj" \
	".\IB.obj" \
	".\IB2.obj" \
	".\IB_32.obj" \
	".\IB_A.obj" \
	".\IB_AR.obj" \
	".\IB_AT.obj" \
	".\IB_ATD.obj" \
	".\IB_ATDM.obj" \
	".\IB_ATM.obj" \
	".\IB_ATR.obj" \
	".\IB_ATRD.obj" \
	".\IB_ATRDM.obj" \
	".\IB_DW.obj" \
	".\IB_R.obj" \
	".\IB_RD.obj" \
	".\IB_T.obj" \
	".\IB_TD.obj" \
	".\IB_TDM.obj" \
	".\IB_TM1.obj" \
	".\IB_TM2.obj" \
	".\IB_TM3.obj" \
	".\IB_TR.obj" \
	".\IB_TRD.obj" \
	".\IB_TRDM.obj" \
	".\IC.obj" \
	".\IC_R.obj" \
	".\IJ_T.obj" \
	".\IR.obj" \
	".\IR_A.obj" \
	".\IR_AM.obj" \
	".\IR_BAR.obj" \
	".\IR_M.obj"

"$(OUTDIR)\am2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

OUTDIR=.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\am2.exe"

!ELSE 

ALL : "$(OUTDIR)\am2.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Am.obj"
	-@erase "$(INTDIR)\Oai_act.obj"
	-@erase "$(INTDIR)\Oai_act2.obj"
	-@erase "$(INTDIR)\Oai_attk.obj"
	-@erase "$(INTDIR)\Oai_buil.obj"
	-@erase "$(INTDIR)\Oai_cap2.obj"
	-@erase "$(INTDIR)\Oai_capt.obj"
	-@erase "$(INTDIR)\Oai_defe.obj"
	-@erase "$(INTDIR)\Oai_dipl.obj"
	-@erase "$(INTDIR)\Oai_eco.obj"
	-@erase "$(INTDIR)\Oai_god.obj"
	-@erase "$(INTDIR)\Oai_gran.obj"
	-@erase "$(INTDIR)\Oai_info.obj"
	-@erase "$(INTDIR)\Oai_main.obj"
	-@erase "$(INTDIR)\Oai_mar2.obj"
	-@erase "$(INTDIR)\Oai_mar3.obj"
	-@erase "$(INTDIR)\Oai_mari.obj"
	-@erase "$(INTDIR)\Oai_mili.obj"
	-@erase "$(INTDIR)\Oai_mons.obj"
	-@erase "$(INTDIR)\Oai_quer.obj"
	-@erase "$(INTDIR)\Oai_seek.obj"
	-@erase "$(INTDIR)\Oai_spy.obj"
	-@erase "$(INTDIR)\Oai_talk.obj"
	-@erase "$(INTDIR)\Oai_town.obj"
	-@erase "$(INTDIR)\Oai_trad.obj"
	-@erase "$(INTDIR)\Oai_unit.obj"
	-@erase "$(INTDIR)\Oanline.obj"
	-@erase "$(INTDIR)\Oaudio.obj"
	-@erase "$(INTDIR)\Ob_flame.obj"
	-@erase "$(INTDIR)\Ob_homin.obj"
	-@erase "$(INTDIR)\Ob_proj.obj"
	-@erase "$(INTDIR)\Obattle.obj"
	-@erase "$(INTDIR)\obitmap.obj"
	-@erase "$(INTDIR)\Oblob.obj"
	-@erase "$(INTDIR)\oblob2.obj"
	-@erase "$(INTDIR)\Obox.obj"
	-@erase "$(INTDIR)\Obullet.obj"
	-@erase "$(INTDIR)\Obulleta.obj"
	-@erase "$(INTDIR)\Obutt3d.obj"
	-@erase "$(INTDIR)\Obuttcus.obj"
	-@erase "$(INTDIR)\Obutton.obj"
	-@erase "$(INTDIR)\Ocoltbl.obj"
	-@erase "$(INTDIR)\Oconfig.obj"
	-@erase "$(INTDIR)\Ocrc_sto.obj"
	-@erase "$(INTDIR)\Odate.obj"
	-@erase "$(INTDIR)\Odb.obj"
	-@erase "$(INTDIR)\Odir.obj"
	-@erase "$(INTDIR)\Odplay.obj"
	-@erase "$(INTDIR)\Odynarr.obj"
	-@erase "$(INTDIR)\Odynarrb.obj"
	-@erase "$(INTDIR)\Oeffect.obj"
	-@erase "$(INTDIR)\Oerrctrl.obj"
	-@erase "$(INTDIR)\Oerror.obj"
	-@erase "$(INTDIR)\Oexpmask.obj"
	-@erase "$(INTDIR)\Of_base.obj"
	-@erase "$(INTDIR)\Of_base2.obj"
	-@erase "$(INTDIR)\Of_camp.obj"
	-@erase "$(INTDIR)\Of_camp2.obj"
	-@erase "$(INTDIR)\Of_fact.obj"
	-@erase "$(INTDIR)\Of_fact2.obj"
	-@erase "$(INTDIR)\Of_harb.obj"
	-@erase "$(INTDIR)\Of_harb2.obj"
	-@erase "$(INTDIR)\Of_inn.obj"
	-@erase "$(INTDIR)\Of_inn2.obj"
	-@erase "$(INTDIR)\Of_mark.obj"
	-@erase "$(INTDIR)\Of_mark2.obj"
	-@erase "$(INTDIR)\Of_mine.obj"
	-@erase "$(INTDIR)\Of_mine2.obj"
	-@erase "$(INTDIR)\Of_mons.obj"
	-@erase "$(INTDIR)\Of_rese.obj"
	-@erase "$(INTDIR)\Of_rese2.obj"
	-@erase "$(INTDIR)\Of_war.obj"
	-@erase "$(INTDIR)\Of_war2.obj"
	-@erase "$(INTDIR)\Ofile.obj"
	-@erase "$(INTDIR)\Ofiletxt.obj"
	-@erase "$(INTDIR)\Ofirm.obj"
	-@erase "$(INTDIR)\Ofirm2.obj"
	-@erase "$(INTDIR)\Ofirma.obj"
	-@erase "$(INTDIR)\Ofirmai.obj"
	-@erase "$(INTDIR)\Ofirmdie.obj"
	-@erase "$(INTDIR)\Ofirmdrw.obj"
	-@erase "$(INTDIR)\Ofirmif.obj"
	-@erase "$(INTDIR)\Ofirmif2.obj"
	-@erase "$(INTDIR)\Ofirmif3.obj"
	-@erase "$(INTDIR)\Ofirmres.obj"
	-@erase "$(INTDIR)\Oflame.obj"
	-@erase "$(INTDIR)\Ofont.obj"
	-@erase "$(INTDIR)\Ogamcred.obj"
	-@erase "$(INTDIR)\Ogamdemo.obj"
	-@erase "$(INTDIR)\Ogame.obj"
	-@erase "$(INTDIR)\Ogamemp.obj"
	-@erase "$(INTDIR)\Ogamency.obj"
	-@erase "$(INTDIR)\Ogamend.obj"
	-@erase "$(INTDIR)\Ogameset.obj"
	-@erase "$(INTDIR)\Ogamhall.obj"
	-@erase "$(INTDIR)\Ogammain.obj"
	-@erase "$(INTDIR)\Ogammenu.obj"
	-@erase "$(INTDIR)\Ogamsce2.obj"
	-@erase "$(INTDIR)\Ogamscen.obj"
	-@erase "$(INTDIR)\Ogamsing.obj"
	-@erase "$(INTDIR)\Ogenhill.obj"
	-@erase "$(INTDIR)\Ogenmap.obj"
	-@erase "$(INTDIR)\Ogenmap2.obj"
	-@erase "$(INTDIR)\Oget.obj"
	-@erase "$(INTDIR)\Ogeta.obj"
	-@erase "$(INTDIR)\Ogf_v1.obj"
	-@erase "$(INTDIR)\Ogfile.obj"
	-@erase "$(INTDIR)\Ogfile2.obj"
	-@erase "$(INTDIR)\Ogfile3.obj"
	-@erase "$(INTDIR)\Ogfilea.obj"
	-@erase "$(INTDIR)\Ogodres.obj"
	-@erase "$(INTDIR)\Ogrpsel.obj"
	-@erase "$(INTDIR)\Ohelp.obj"
	-@erase "$(INTDIR)\Ohillres.obj"
	-@erase "$(INTDIR)\Ohsetres.obj"
	-@erase "$(INTDIR)\Oimgres.obj"
	-@erase "$(INTDIR)\Oimmplay.obj"
	-@erase "$(INTDIR)\Oinfo.obj"
	-@erase "$(INTDIR)\Oingmenu.obj"
	-@erase "$(INTDIR)\Oisoarea.obj"
	-@erase "$(INTDIR)\Olightn.obj"
	-@erase "$(INTDIR)\Olightn2.obj"
	-@erase "$(INTDIR)\olocbmp.obj"
	-@erase "$(INTDIR)\Olog.obj"
	-@erase "$(INTDIR)\Olonglog.obj"
	-@erase "$(INTDIR)\Olzw.obj"
	-@erase "$(INTDIR)\Omatrix.obj"
	-@erase "$(INTDIR)\omatrix2.obj"
	-@erase "$(INTDIR)\Omem.obj"
	-@erase "$(INTDIR)\Omisc.obj"
	-@erase "$(INTDIR)\Omlink.obj"
	-@erase "$(INTDIR)\Omonsres.obj"
	-@erase "$(INTDIR)\Omouse.obj"
	-@erase "$(INTDIR)\Omousecr.obj"
	-@erase "$(INTDIR)\Omousefr.obj"
	-@erase "$(INTDIR)\Omousege.obj"
	-@erase "$(INTDIR)\Omousesp.obj"
	-@erase "$(INTDIR)\Omp_crc.obj"
	-@erase "$(INTDIR)\Omusic.obj"
	-@erase "$(INTDIR)\Onationa.obj"
	-@erase "$(INTDIR)\Onationb.obj"
	-@erase "$(INTDIR)\Onews.obj"
	-@erase "$(INTDIR)\Onews2.obj"
	-@erase "$(INTDIR)\Onewseng.obj"
	-@erase "$(INTDIR)\Onewsfre.obj"
	-@erase "$(INTDIR)\Onewsger.obj"
	-@erase "$(INTDIR)\Onewsspa.obj"
	-@erase "$(INTDIR)\Ooptmenu.obj"
	-@erase "$(INTDIR)\Oplant.obj"
	-@erase "$(INTDIR)\Oplasma.obj"
	-@erase "$(INTDIR)\Opower.obj"
	-@erase "$(INTDIR)\Or_ai.obj"
	-@erase "$(INTDIR)\Or_eco.obj"
	-@erase "$(INTDIR)\Or_mil.obj"
	-@erase "$(INTDIR)\Or_nat.obj"
	-@erase "$(INTDIR)\Or_news.obj"
	-@erase "$(INTDIR)\Or_rank.obj"
	-@erase "$(INTDIR)\Or_spy.obj"
	-@erase "$(INTDIR)\Or_tech.obj"
	-@erase "$(INTDIR)\Or_town.obj"
	-@erase "$(INTDIR)\Or_trade.obj"
	-@erase "$(INTDIR)\Oraceres.obj"
	-@erase "$(INTDIR)\Orain1.obj"
	-@erase "$(INTDIR)\Orain2.obj"
	-@erase "$(INTDIR)\Orain3.obj"
	-@erase "$(INTDIR)\Orawres.obj"
	-@erase "$(INTDIR)\Orebel.obj"
	-@erase "$(INTDIR)\Oregion.obj"
	-@erase "$(INTDIR)\Oregions.obj"
	-@erase "$(INTDIR)\Oremote.obj"
	-@erase "$(INTDIR)\Oremote2.obj"
	-@erase "$(INTDIR)\Oremotem.obj"
	-@erase "$(INTDIR)\Oremoteq.obj"
	-@erase "$(INTDIR)\Ores.obj"
	-@erase "$(INTDIR)\Oresdb.obj"
	-@erase "$(INTDIR)\Orestxt.obj"
	-@erase "$(INTDIR)\Oresx.obj"
	-@erase "$(INTDIR)\Orock.obj"
	-@erase "$(INTDIR)\Orockres.obj"
	-@erase "$(INTDIR)\Oscroll.obj"
	-@erase "$(INTDIR)\Ose.obj"
	-@erase "$(INTDIR)\Oseres.obj"
	-@erase "$(INTDIR)\Osfrmres.obj"
	-@erase "$(INTDIR)\Osite.obj"
	-@erase "$(INTDIR)\Ositedrw.obj"
	-@erase "$(INTDIR)\Oskill.obj"
	-@erase "$(INTDIR)\Oslidcus.obj"
	-@erase "$(INTDIR)\Oslider.obj"
	-@erase "$(INTDIR)\Osnow1.obj"
	-@erase "$(INTDIR)\Osnow2.obj"
	-@erase "$(INTDIR)\Osnowg.obj"
	-@erase "$(INTDIR)\Osnowres.obj"
	-@erase "$(INTDIR)\Ospath.obj"
	-@erase "$(INTDIR)\Ospathbt.obj"
	-@erase "$(INTDIR)\Ospinner.obj"
	-@erase "$(INTDIR)\Ospredbg.obj"
	-@erase "$(INTDIR)\Ospreoff.obj"
	-@erase "$(INTDIR)\Ospresmo.obj"
	-@erase "$(INTDIR)\Ospreuse.obj"
	-@erase "$(INTDIR)\Osprite.obj"
	-@erase "$(INTDIR)\Osprite2.obj"
	-@erase "$(INTDIR)\Ospritea.obj"
	-@erase "$(INTDIR)\Osprtres.obj"
	-@erase "$(INTDIR)\Ospy.obj"
	-@erase "$(INTDIR)\Ospy2.obj"
	-@erase "$(INTDIR)\Ospya.obj"
	-@erase "$(INTDIR)\Ostr.obj"
	-@erase "$(INTDIR)\Osys.obj"
	-@erase "$(INTDIR)\Osys2.obj"
	-@erase "$(INTDIR)\Osys3.obj"
	-@erase "$(INTDIR)\Otalkeng.obj"
	-@erase "$(INTDIR)\Otalkfre.obj"
	-@erase "$(INTDIR)\Otalkger.obj"
	-@erase "$(INTDIR)\Otalkmsg.obj"
	-@erase "$(INTDIR)\Otalkres.obj"
	-@erase "$(INTDIR)\Otalkspa.obj"
	-@erase "$(INTDIR)\Otechres.obj"
	-@erase "$(INTDIR)\Oterrain.obj"
	-@erase "$(INTDIR)\Otornado.obj"
	-@erase "$(INTDIR)\Otown.obj"
	-@erase "$(INTDIR)\Otowna.obj"
	-@erase "$(INTDIR)\Otownai.obj"
	-@erase "$(INTDIR)\Otownbld.obj"
	-@erase "$(INTDIR)\Otowndrw.obj"
	-@erase "$(INTDIR)\Otownif.obj"
	-@erase "$(INTDIR)\Otownind.obj"
	-@erase "$(INTDIR)\Otownres.obj"
	-@erase "$(INTDIR)\Otransl.obj"
	-@erase "$(INTDIR)\Otutor.obj"
	-@erase "$(INTDIR)\Otutor2.obj"
	-@erase "$(INTDIR)\Ou_cara.obj"
	-@erase "$(INTDIR)\Ou_cara2.obj"
	-@erase "$(INTDIR)\Ou_caras.obj"
	-@erase "$(INTDIR)\Ou_carat.obj"
	-@erase "$(INTDIR)\Ou_cart.obj"
	-@erase "$(INTDIR)\Ou_god.obj"
	-@erase "$(INTDIR)\Ou_god2.obj"
	-@erase "$(INTDIR)\Ou_mari.obj"
	-@erase "$(INTDIR)\Ou_mari2.obj"
	-@erase "$(INTDIR)\Ou_marif.obj"
	-@erase "$(INTDIR)\Ou_maris.obj"
	-@erase "$(INTDIR)\Ou_marit.obj"
	-@erase "$(INTDIR)\Ou_mons.obj"
	-@erase "$(INTDIR)\Ou_vehi.obj"
	-@erase "$(INTDIR)\Ounit.obj"
	-@erase "$(INTDIR)\Ounit2.obj"
	-@erase "$(INTDIR)\Ounita.obj"
	-@erase "$(INTDIR)\Ounitaac.obj"
	-@erase "$(INTDIR)\Ounitaat.obj"
	-@erase "$(INTDIR)\Ounitac.obj"
	-@erase "$(INTDIR)\Ounitai.obj"
	-@erase "$(INTDIR)\Ounitam.obj"
	-@erase "$(INTDIR)\Ounitamt.obj"
	-@erase "$(INTDIR)\Ounitat.obj"
	-@erase "$(INTDIR)\Ounitat2.obj"
	-@erase "$(INTDIR)\Ounitat3.obj"
	-@erase "$(INTDIR)\Ounitatb.obj"
	-@erase "$(INTDIR)\Ounitd.obj"
	-@erase "$(INTDIR)\Ounitdrw.obj"
	-@erase "$(INTDIR)\Ounithb.obj"
	-@erase "$(INTDIR)\Ouniti.obj"
	-@erase "$(INTDIR)\Ounitif.obj"
	-@erase "$(INTDIR)\Ounitind.obj"
	-@erase "$(INTDIR)\Ounitm.obj"
	-@erase "$(INTDIR)\Ounitres.obj"
	-@erase "$(INTDIR)\Ounits.obj"
	-@erase "$(INTDIR)\Ovbrowif.obj"
	-@erase "$(INTDIR)\Ovbrowse.obj"
	-@erase "$(INTDIR)\Ovga.obj"
	-@erase "$(INTDIR)\Ovga2.obj"
	-@erase "$(INTDIR)\Ovgabuf.obj"
	-@erase "$(INTDIR)\Ovgabuf2.obj"
	-@erase "$(INTDIR)\Ovgalock.obj"
	-@erase "$(INTDIR)\Ovideo.obj"
	-@erase "$(INTDIR)\Ovolume.obj"
	-@erase "$(INTDIR)\Ovqueue.obj"
	-@erase "$(INTDIR)\Ow_fire.obj"
	-@erase "$(INTDIR)\Ow_plant.obj"
	-@erase "$(INTDIR)\Ow_rock.obj"
	-@erase "$(INTDIR)\Ow_sound.obj"
	-@erase "$(INTDIR)\Ow_wall.obj"
	-@erase "$(INTDIR)\Owallres.obj"
	-@erase "$(INTDIR)\Owarpt.obj"
	-@erase "$(INTDIR)\Oweather.obj"
	-@erase "$(INTDIR)\Oworld.obj"
	-@erase "$(INTDIR)\Oworld_m.obj"
	-@erase "$(INTDIR)\Oworld_z.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\am2.exe"
	-@erase "$(OUTDIR)\am2.pdb"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Od /I "d:\am2" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "IMAGICMP" /D "DEBUG" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\am2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib dxguid.lib ddraw.lib\
 dplayx.lib dinput.lib dsound.lib strmiids.lib winmm.lib immultr5.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\am2.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\am2.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Am.obj" \
	"$(INTDIR)\Oai_act.obj" \
	"$(INTDIR)\Oai_act2.obj" \
	"$(INTDIR)\Oai_attk.obj" \
	"$(INTDIR)\Oai_buil.obj" \
	"$(INTDIR)\Oai_cap2.obj" \
	"$(INTDIR)\Oai_capt.obj" \
	"$(INTDIR)\Oai_defe.obj" \
	"$(INTDIR)\Oai_dipl.obj" \
	"$(INTDIR)\Oai_eco.obj" \
	"$(INTDIR)\Oai_god.obj" \
	"$(INTDIR)\Oai_gran.obj" \
	"$(INTDIR)\Oai_info.obj" \
	"$(INTDIR)\Oai_main.obj" \
	"$(INTDIR)\Oai_mar2.obj" \
	"$(INTDIR)\Oai_mar3.obj" \
	"$(INTDIR)\Oai_mari.obj" \
	"$(INTDIR)\Oai_mili.obj" \
	"$(INTDIR)\Oai_mons.obj" \
	"$(INTDIR)\Oai_quer.obj" \
	"$(INTDIR)\Oai_seek.obj" \
	"$(INTDIR)\Oai_spy.obj" \
	"$(INTDIR)\Oai_talk.obj" \
	"$(INTDIR)\Oai_town.obj" \
	"$(INTDIR)\Oai_trad.obj" \
	"$(INTDIR)\Oai_unit.obj" \
	"$(INTDIR)\Oanline.obj" \
	"$(INTDIR)\Oaudio.obj" \
	"$(INTDIR)\Ob_flame.obj" \
	"$(INTDIR)\Ob_homin.obj" \
	"$(INTDIR)\Ob_proj.obj" \
	"$(INTDIR)\Obattle.obj" \
	"$(INTDIR)\obitmap.obj" \
	"$(INTDIR)\Oblob.obj" \
	"$(INTDIR)\oblob2.obj" \
	"$(INTDIR)\Obox.obj" \
	"$(INTDIR)\Obullet.obj" \
	"$(INTDIR)\Obulleta.obj" \
	"$(INTDIR)\Obutt3d.obj" \
	"$(INTDIR)\Obuttcus.obj" \
	"$(INTDIR)\Obutton.obj" \
	"$(INTDIR)\Ocoltbl.obj" \
	"$(INTDIR)\Oconfig.obj" \
	"$(INTDIR)\Ocrc_sto.obj" \
	"$(INTDIR)\Odate.obj" \
	"$(INTDIR)\Odb.obj" \
	"$(INTDIR)\Odir.obj" \
	"$(INTDIR)\Odplay.obj" \
	"$(INTDIR)\Odynarr.obj" \
	"$(INTDIR)\Odynarrb.obj" \
	"$(INTDIR)\Oeffect.obj" \
	"$(INTDIR)\Oerrctrl.obj" \
	"$(INTDIR)\Oerror.obj" \
	"$(INTDIR)\Oexpmask.obj" \
	"$(INTDIR)\Of_base.obj" \
	"$(INTDIR)\Of_base2.obj" \
	"$(INTDIR)\Of_camp.obj" \
	"$(INTDIR)\Of_camp2.obj" \
	"$(INTDIR)\Of_fact.obj" \
	"$(INTDIR)\Of_fact2.obj" \
	"$(INTDIR)\Of_harb.obj" \
	"$(INTDIR)\Of_harb2.obj" \
	"$(INTDIR)\Of_inn.obj" \
	"$(INTDIR)\Of_inn2.obj" \
	"$(INTDIR)\Of_mark.obj" \
	"$(INTDIR)\Of_mark2.obj" \
	"$(INTDIR)\Of_mine.obj" \
	"$(INTDIR)\Of_mine2.obj" \
	"$(INTDIR)\Of_mons.obj" \
	"$(INTDIR)\Of_rese.obj" \
	"$(INTDIR)\Of_rese2.obj" \
	"$(INTDIR)\Of_war.obj" \
	"$(INTDIR)\Of_war2.obj" \
	"$(INTDIR)\Ofile.obj" \
	"$(INTDIR)\Ofiletxt.obj" \
	"$(INTDIR)\Ofirm.obj" \
	"$(INTDIR)\Ofirm2.obj" \
	"$(INTDIR)\Ofirma.obj" \
	"$(INTDIR)\Ofirmai.obj" \
	"$(INTDIR)\Ofirmdie.obj" \
	"$(INTDIR)\Ofirmdrw.obj" \
	"$(INTDIR)\Ofirmif.obj" \
	"$(INTDIR)\Ofirmif2.obj" \
	"$(INTDIR)\Ofirmif3.obj" \
	"$(INTDIR)\Ofirmres.obj" \
	"$(INTDIR)\Oflame.obj" \
	"$(INTDIR)\Ofont.obj" \
	"$(INTDIR)\Ogamcred.obj" \
	"$(INTDIR)\Ogamdemo.obj" \
	"$(INTDIR)\Ogame.obj" \
	"$(INTDIR)\Ogamemp.obj" \
	"$(INTDIR)\Ogamency.obj" \
	"$(INTDIR)\Ogamend.obj" \
	"$(INTDIR)\Ogameset.obj" \
	"$(INTDIR)\Ogamhall.obj" \
	"$(INTDIR)\Ogammain.obj" \
	"$(INTDIR)\Ogammenu.obj" \
	"$(INTDIR)\Ogamsce2.obj" \
	"$(INTDIR)\Ogamscen.obj" \
	"$(INTDIR)\Ogamsing.obj" \
	"$(INTDIR)\Ogenhill.obj" \
	"$(INTDIR)\Ogenmap.obj" \
	"$(INTDIR)\Ogenmap2.obj" \
	"$(INTDIR)\Oget.obj" \
	"$(INTDIR)\Ogeta.obj" \
	"$(INTDIR)\Ogf_v1.obj" \
	"$(INTDIR)\Ogfile.obj" \
	"$(INTDIR)\Ogfile2.obj" \
	"$(INTDIR)\Ogfile3.obj" \
	"$(INTDIR)\Ogfilea.obj" \
	"$(INTDIR)\Ogodres.obj" \
	"$(INTDIR)\Ogrpsel.obj" \
	"$(INTDIR)\Ohelp.obj" \
	"$(INTDIR)\Ohillres.obj" \
	"$(INTDIR)\Ohsetres.obj" \
	"$(INTDIR)\Oimgres.obj" \
	"$(INTDIR)\Oimmplay.obj" \
	"$(INTDIR)\Oinfo.obj" \
	"$(INTDIR)\Oingmenu.obj" \
	"$(INTDIR)\Oisoarea.obj" \
	"$(INTDIR)\Olightn.obj" \
	"$(INTDIR)\Olightn2.obj" \
	"$(INTDIR)\olocbmp.obj" \
	"$(INTDIR)\Olog.obj" \
	"$(INTDIR)\Olonglog.obj" \
	"$(INTDIR)\Olzw.obj" \
	"$(INTDIR)\Omatrix.obj" \
	"$(INTDIR)\omatrix2.obj" \
	"$(INTDIR)\Omem.obj" \
	"$(INTDIR)\Omisc.obj" \
	"$(INTDIR)\Omlink.obj" \
	"$(INTDIR)\Omonsres.obj" \
	"$(INTDIR)\Omouse.obj" \
	"$(INTDIR)\Omousecr.obj" \
	"$(INTDIR)\Omousefr.obj" \
	"$(INTDIR)\Omousege.obj" \
	"$(INTDIR)\Omousesp.obj" \
	"$(INTDIR)\Omp_crc.obj" \
	"$(INTDIR)\Omusic.obj" \
	"$(INTDIR)\Onationa.obj" \
	"$(INTDIR)\Onationb.obj" \
	"$(INTDIR)\Onews.obj" \
	"$(INTDIR)\Onews2.obj" \
	"$(INTDIR)\Onewseng.obj" \
	"$(INTDIR)\Onewsfre.obj" \
	"$(INTDIR)\Onewsger.obj" \
	"$(INTDIR)\Onewsspa.obj" \
	"$(INTDIR)\Ooptmenu.obj" \
	"$(INTDIR)\Oplant.obj" \
	"$(INTDIR)\Oplasma.obj" \
	"$(INTDIR)\Opower.obj" \
	"$(INTDIR)\Or_ai.obj" \
	"$(INTDIR)\Or_eco.obj" \
	"$(INTDIR)\Or_mil.obj" \
	"$(INTDIR)\Or_nat.obj" \
	"$(INTDIR)\Or_news.obj" \
	"$(INTDIR)\Or_rank.obj" \
	"$(INTDIR)\Or_spy.obj" \
	"$(INTDIR)\Or_tech.obj" \
	"$(INTDIR)\Or_town.obj" \
	"$(INTDIR)\Or_trade.obj" \
	"$(INTDIR)\Oraceres.obj" \
	"$(INTDIR)\Orain1.obj" \
	"$(INTDIR)\Orain2.obj" \
	"$(INTDIR)\Orain3.obj" \
	"$(INTDIR)\Orawres.obj" \
	"$(INTDIR)\Orebel.obj" \
	"$(INTDIR)\Oregion.obj" \
	"$(INTDIR)\Oregions.obj" \
	"$(INTDIR)\Oremote.obj" \
	"$(INTDIR)\Oremote2.obj" \
	"$(INTDIR)\Oremotem.obj" \
	"$(INTDIR)\Oremoteq.obj" \
	"$(INTDIR)\Ores.obj" \
	"$(INTDIR)\Oresdb.obj" \
	"$(INTDIR)\Orestxt.obj" \
	"$(INTDIR)\Oresx.obj" \
	"$(INTDIR)\Orock.obj" \
	"$(INTDIR)\Orockres.obj" \
	"$(INTDIR)\Oscroll.obj" \
	"$(INTDIR)\Ose.obj" \
	"$(INTDIR)\Oseres.obj" \
	"$(INTDIR)\Osfrmres.obj" \
	"$(INTDIR)\Osite.obj" \
	"$(INTDIR)\Ositedrw.obj" \
	"$(INTDIR)\Oskill.obj" \
	"$(INTDIR)\Oslidcus.obj" \
	"$(INTDIR)\Oslider.obj" \
	"$(INTDIR)\Osnow1.obj" \
	"$(INTDIR)\Osnow2.obj" \
	"$(INTDIR)\Osnowg.obj" \
	"$(INTDIR)\Osnowres.obj" \
	"$(INTDIR)\Ospath.obj" \
	"$(INTDIR)\Ospathbt.obj" \
	"$(INTDIR)\Ospinner.obj" \
	"$(INTDIR)\Ospredbg.obj" \
	"$(INTDIR)\Ospreoff.obj" \
	"$(INTDIR)\Ospresmo.obj" \
	"$(INTDIR)\Ospreuse.obj" \
	"$(INTDIR)\Osprite.obj" \
	"$(INTDIR)\Osprite2.obj" \
	"$(INTDIR)\Ospritea.obj" \
	"$(INTDIR)\Osprtres.obj" \
	"$(INTDIR)\Ospy.obj" \
	"$(INTDIR)\Ospy2.obj" \
	"$(INTDIR)\Ospya.obj" \
	"$(INTDIR)\Ostr.obj" \
	"$(INTDIR)\Osys.obj" \
	"$(INTDIR)\Osys2.obj" \
	"$(INTDIR)\Osys3.obj" \
	"$(INTDIR)\Otalkeng.obj" \
	"$(INTDIR)\Otalkfre.obj" \
	"$(INTDIR)\Otalkger.obj" \
	"$(INTDIR)\Otalkmsg.obj" \
	"$(INTDIR)\Otalkres.obj" \
	"$(INTDIR)\Otalkspa.obj" \
	"$(INTDIR)\Otechres.obj" \
	"$(INTDIR)\Oterrain.obj" \
	"$(INTDIR)\Otornado.obj" \
	"$(INTDIR)\Otown.obj" \
	"$(INTDIR)\Otowna.obj" \
	"$(INTDIR)\Otownai.obj" \
	"$(INTDIR)\Otownbld.obj" \
	"$(INTDIR)\Otowndrw.obj" \
	"$(INTDIR)\Otownif.obj" \
	"$(INTDIR)\Otownind.obj" \
	"$(INTDIR)\Otownres.obj" \
	"$(INTDIR)\Otransl.obj" \
	"$(INTDIR)\Otutor.obj" \
	"$(INTDIR)\Otutor2.obj" \
	"$(INTDIR)\Ou_cara.obj" \
	"$(INTDIR)\Ou_cara2.obj" \
	"$(INTDIR)\Ou_caras.obj" \
	"$(INTDIR)\Ou_carat.obj" \
	"$(INTDIR)\Ou_cart.obj" \
	"$(INTDIR)\Ou_god.obj" \
	"$(INTDIR)\Ou_god2.obj" \
	"$(INTDIR)\Ou_mari.obj" \
	"$(INTDIR)\Ou_mari2.obj" \
	"$(INTDIR)\Ou_marif.obj" \
	"$(INTDIR)\Ou_maris.obj" \
	"$(INTDIR)\Ou_marit.obj" \
	"$(INTDIR)\Ou_mons.obj" \
	"$(INTDIR)\Ou_vehi.obj" \
	"$(INTDIR)\Ounit.obj" \
	"$(INTDIR)\Ounit2.obj" \
	"$(INTDIR)\Ounita.obj" \
	"$(INTDIR)\Ounitaac.obj" \
	"$(INTDIR)\Ounitaat.obj" \
	"$(INTDIR)\Ounitac.obj" \
	"$(INTDIR)\Ounitai.obj" \
	"$(INTDIR)\Ounitam.obj" \
	"$(INTDIR)\Ounitamt.obj" \
	"$(INTDIR)\Ounitat.obj" \
	"$(INTDIR)\Ounitat2.obj" \
	"$(INTDIR)\Ounitat3.obj" \
	"$(INTDIR)\Ounitatb.obj" \
	"$(INTDIR)\Ounitd.obj" \
	"$(INTDIR)\Ounitdrw.obj" \
	"$(INTDIR)\Ounithb.obj" \
	"$(INTDIR)\Ouniti.obj" \
	"$(INTDIR)\Ounitif.obj" \
	"$(INTDIR)\Ounitind.obj" \
	"$(INTDIR)\Ounitm.obj" \
	"$(INTDIR)\Ounitres.obj" \
	"$(INTDIR)\Ounits.obj" \
	"$(INTDIR)\Ovbrowif.obj" \
	"$(INTDIR)\Ovbrowse.obj" \
	"$(INTDIR)\Ovga.obj" \
	"$(INTDIR)\Ovga2.obj" \
	"$(INTDIR)\Ovgabuf.obj" \
	"$(INTDIR)\Ovgabuf2.obj" \
	"$(INTDIR)\Ovgalock.obj" \
	"$(INTDIR)\Ovideo.obj" \
	"$(INTDIR)\Ovolume.obj" \
	"$(INTDIR)\Ovqueue.obj" \
	"$(INTDIR)\Ow_fire.obj" \
	"$(INTDIR)\Ow_plant.obj" \
	"$(INTDIR)\Ow_rock.obj" \
	"$(INTDIR)\Ow_sound.obj" \
	"$(INTDIR)\Ow_wall.obj" \
	"$(INTDIR)\Owallres.obj" \
	"$(INTDIR)\Owarpt.obj" \
	"$(INTDIR)\Oweather.obj" \
	"$(INTDIR)\Oworld.obj" \
	"$(INTDIR)\Oworld_m.obj" \
	"$(INTDIR)\Oworld_z.obj" \
	".\CRC.obj" \
	".\I_BAR.obj" \
	".\I_BLACK.obj" \
	".\I_CTRL.obj" \
	".\I_EMASK.obj" \
	".\I_EREMAP.obj" \
	".\I_FONT.obj" \
	".\I_FREMAP.obj" \
	".\I_LINE.obj" \
	".\I_PIXEL.obj" \
	".\I_READ.obj" \
	".\I_SNOW.obj" \
	".\I_XOR.obj" \
	".\IB.obj" \
	".\IB2.obj" \
	".\IB_32.obj" \
	".\IB_A.obj" \
	".\IB_AR.obj" \
	".\IB_AT.obj" \
	".\IB_ATD.obj" \
	".\IB_ATDM.obj" \
	".\IB_ATM.obj" \
	".\IB_ATR.obj" \
	".\IB_ATRD.obj" \
	".\IB_ATRDM.obj" \
	".\IB_DW.obj" \
	".\IB_R.obj" \
	".\IB_RD.obj" \
	".\IB_T.obj" \
	".\IB_TD.obj" \
	".\IB_TDM.obj" \
	".\IB_TM1.obj" \
	".\IB_TM2.obj" \
	".\IB_TM3.obj" \
	".\IB_TR.obj" \
	".\IB_TRD.obj" \
	".\IB_TRDM.obj" \
	".\IC.obj" \
	".\IC_R.obj" \
	".\IJ_T.obj" \
	".\IR.obj" \
	".\IR_A.obj" \
	".\IR_AM.obj" \
	".\IR_BAR.obj" \
	".\IR_M.obj"

"$(OUTDIR)\am2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "am2 - Win32 Release" || "$(CFG)" == "am2 - Win32 Debug"
SOURCE=.\Am.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_AM_CP=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\OAUDIO.H"\
	".\OBATTLE.H"\
	".\obox.h"\
	".\OBULLET.H"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\OCRC_STO.H"\
	".\odate.h"\
	".\ODB.H"\
	".\ODPLAY.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OERRCTRL.H"\
	".\OEXPMASK.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OFLAME.H"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\OGFILE.H"\
	".\ogodres.h"\
	".\OGRPSEL.H"\
	".\OHELP.H"\
	".\OHILLRES.H"\
	".\OIMGRES.H"\
	".\OIMMPLAY.H"\
	".\oinfo.h"\
	".\OINGMENU.H"\
	".\OLOCBMP.H"\
	".\OLOG.H"\
	".\OLONGLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OOPTMENU.H"\
	".\OPLANT.H"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCK.H"\
	".\OROCKRES.H"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\OSFRMRES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\OSNOWG.H"\
	".\OSNOWRES.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\OTERRAIN.H"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVIDEO.H"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\OWARPT.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	{$(INCLUDE)}"dplobby.h"\
	{$(INCLUDE)}"immlib.h"\
	{$(INCLUDE)}"strmif.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_AM_CP=\
	"c:\mpsdk\VCPP\immprivg.h"\
	

"$(INTDIR)\Am.obj" : $(SOURCE) $(DEP_CPP_AM_CP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_AM_CP=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\OAUDIO.H"\
	".\OBATTLE.H"\
	".\obox.h"\
	".\OBULLET.H"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\OCRC_STO.H"\
	".\odate.h"\
	".\ODB.H"\
	".\ODPLAY.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OERRCTRL.H"\
	".\OEXPMASK.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OFLAME.H"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\OGFILE.H"\
	".\ogodres.h"\
	".\OGRPSEL.H"\
	".\OHELP.H"\
	".\OHILLRES.H"\
	".\OIMGRES.H"\
	".\OIMMPLAY.H"\
	".\oinfo.h"\
	".\OINGMENU.H"\
	".\OLOCBMP.H"\
	".\OLOG.H"\
	".\OLONGLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OOPTMENU.H"\
	".\OPLANT.H"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCK.H"\
	".\OROCKRES.H"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\OSFRMRES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\OSNOWG.H"\
	".\OSNOWRES.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\OTERRAIN.H"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVIDEO.H"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\OWARPT.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	{$(INCLUDE)}"immlib.h"\
	{$(INCLUDE)}"strmif.h"\
	

"$(INTDIR)\Am.obj" : $(SOURCE) $(DEP_CPP_AM_CP) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_act.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_A=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_act.obj" : $(SOURCE) $(DEP_CPP_OAI_A) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_A=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_act.obj" : $(SOURCE) $(DEP_CPP_OAI_A) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_act2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_AC=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_act2.obj" : $(SOURCE) $(DEP_CPP_OAI_AC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_AC=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_act2.obj" : $(SOURCE) $(DEP_CPP_OAI_AC) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_attk.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_AT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_attk.obj" : $(SOURCE) $(DEP_CPP_OAI_AT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_AT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_attk.obj" : $(SOURCE) $(DEP_CPP_OAI_AT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_buil.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_B=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_buil.obj" : $(SOURCE) $(DEP_CPP_OAI_B) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_B=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_buil.obj" : $(SOURCE) $(DEP_CPP_OAI_B) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_cap2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_C=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_cap2.obj" : $(SOURCE) $(DEP_CPP_OAI_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_C=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_cap2.obj" : $(SOURCE) $(DEP_CPP_OAI_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_capt.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_CA=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_INN.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_capt.obj" : $(SOURCE) $(DEP_CPP_OAI_CA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_CA=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_INN.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_capt.obj" : $(SOURCE) $(DEP_CPP_OAI_CA) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_defe.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_D=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_defe.obj" : $(SOURCE) $(DEP_CPP_OAI_D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_OAI_D=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_defe.obj" : $(SOURCE) $(DEP_CPP_OAI_D) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_dipl.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_DI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_dipl.obj" : $(SOURCE) $(DEP_CPP_OAI_DI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_dipl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_eco.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_E=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_eco.obj" : $(SOURCE) $(DEP_CPP_OAI_E) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_eco.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_god.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_G=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_god.obj" : $(SOURCE) $(DEP_CPP_OAI_G) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_god.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_gran.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_GR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_gran.obj" : $(SOURCE) $(DEP_CPP_OAI_GR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_gran.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_info.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_I=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_info.obj" : $(SOURCE) $(DEP_CPP_OAI_I) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_info.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_main.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_M=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_main.obj" : $(SOURCE) $(DEP_CPP_OAI_M) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_main.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_mar2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_MA=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_mar2.obj" : $(SOURCE) $(DEP_CPP_OAI_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_mar2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_mar3.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_MAR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_mar3.obj" : $(SOURCE) $(DEP_CPP_OAI_MAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_mar3.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_mari.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_MARI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_mari.obj" : $(SOURCE) $(DEP_CPP_OAI_MARI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_mari.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_mili.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_MI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_mili.obj" : $(SOURCE) $(DEP_CPP_OAI_MI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_mili.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_mons.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_MO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_mons.obj" : $(SOURCE) $(DEP_CPP_OAI_MO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_mons.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_quer.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_Q=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_quer.obj" : $(SOURCE) $(DEP_CPP_OAI_Q) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_quer.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_seek.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_S=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_seek.obj" : $(SOURCE) $(DEP_CPP_OAI_S) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_seek.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_spy.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_SP=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_spy.obj" : $(SOURCE) $(DEP_CPP_OAI_SP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_spy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_talk.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_T=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_talk.obj" : $(SOURCE) $(DEP_CPP_OAI_T) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_talk.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_town.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_TO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_INN.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_town.obj" : $(SOURCE) $(DEP_CPP_OAI_TO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_town.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_trad.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_TR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_trad.obj" : $(SOURCE) $(DEP_CPP_OAI_TR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_trad.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oai_unit.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAI_U=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_INN.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oai_unit.obj" : $(SOURCE) $(DEP_CPP_OAI_U) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oai_unit.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oanline.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OANLI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OANLINE.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Oanline.obj" : $(SOURCE) $(DEP_CPP_OANLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oanline.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oaudio.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OAUDI=\
	".\all.h"\
	".\gamedef.h"\
	".\OAUDIO.H"\
	".\obox.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OVGALOCK.H"\
	".\OVOLUME.H"\
	

"$(INTDIR)\Oaudio.obj" : $(SOURCE) $(DEP_CPP_OAUDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oaudio.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ob_flame.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OB_FL=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OB_FLAME.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ob_flame.obj" : $(SOURCE) $(DEP_CPP_OB_FL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ob_flame.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ob_homin.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OB_HO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OB_HOMIN.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\OSFRMRES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ob_homin.obj" : $(SOURCE) $(DEP_CPP_OB_HO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ob_homin.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ob_proj.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OB_PR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OB_PROJ.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ob_proj.obj" : $(SOURCE) $(DEP_CPP_OB_PR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ob_proj.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obattle.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBATT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBATTLE.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_CART.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\OU_MONS.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\OUNITALL.H"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Obattle.obj" : $(SOURCE) $(DEP_CPP_OBATT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obattle.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\obitmap.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBITM=\
	".\all.h"\
	".\gamedef.h"\
	".\OBITMAP.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\obitmap.obj" : $(SOURCE) $(DEP_CPP_OBITM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\obitmap.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oblob.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBLOB=\
	".\all.h"\
	".\gamedef.h"\
	".\OBLOB.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Oblob.obj" : $(SOURCE) $(DEP_CPP_OBLOB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oblob.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\oblob2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBLOB2=\
	".\all.h"\
	".\COLCODE.H"\
	".\gamedef.h"\
	".\OBITMAP.H"\
	".\OBLOB2.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\oblob2.obj" : $(SOURCE) $(DEP_CPP_OBLOB2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\oblob2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obox.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBOX_=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\obox.h"\
	".\obutton.h"\
	".\ofile.h"\
	".\ofont.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\opower.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Obox.obj" : $(SOURCE) $(DEP_CPP_OBOX_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obox.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obullet.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBULL=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSERES.H"\
	".\OSFRMRES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CART.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Obullet.obj" : $(SOURCE) $(DEP_CPP_OBULL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obullet.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obulleta.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBULLE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OB_FLAME.H"\
	".\OB_HOMIN.H"\
	".\OB_PROJ.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Obulleta.obj" : $(SOURCE) $(DEP_CPP_OBULLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obulleta.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obutt3d.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBUTT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBUTT3D.H"\
	".\ofile.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Obutt3d.obj" : $(SOURCE) $(DEP_CPP_OBUTT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obutt3d.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obuttcus.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBUTTC=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBUTTCUS.H"\
	".\ofile.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Obuttcus.obj" : $(SOURCE) $(DEP_CPP_OBUTTC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obuttcus.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obutton.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OBUTTO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\obutton.h"\
	".\ofile.h"\
	".\ofont.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Obutton.obj" : $(SOURCE) $(DEP_CPP_OBUTTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Obutton.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ocoltbl.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OCOLT=\
	".\all.h"\
	".\gamedef.h"\
	".\OCOLTBL.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Ocoltbl.obj" : $(SOURCE) $(DEP_CPP_OCOLT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ocoltbl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oconfig.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OCONF=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OHELP.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oconfig.obj" : $(SOURCE) $(DEP_CPP_OCONF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oconfig.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ocrc_sto.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OCRC_=\
	".\all.h"\
	".\CRC.H"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\OCRC_STO.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ocrc_sto.obj" : $(SOURCE) $(DEP_CPP_OCRC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ocrc_sto.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odate.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ODATE=\
	".\odate.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\otransl.h"\
	

"$(INTDIR)\Odate.obj" : $(SOURCE) $(DEP_CPP_ODATE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Odate.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ODB_C=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Odb.obj" : $(SOURCE) $(DEP_CPP_ODB_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Odb.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odir.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ODIR_=\
	".\all.h"\
	".\gamedef.h"\
	".\ODIR.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Odir.obj" : $(SOURCE) $(DEP_CPP_ODIR_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Odir.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odplay.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ODPLA=\
	".\all.h"\
	".\gamedef.h"\
	".\mptypes.h"\
	".\OBLOB.H"\
	".\ODPLAY.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dplobby.h"\
	

"$(INTDIR)\Odplay.obj" : $(SOURCE) $(DEP_CPP_ODPLA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Odplay.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odynarr.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ODYNA=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Odynarr.obj" : $(SOURCE) $(DEP_CPP_ODYNA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Odynarr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odynarrb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ODYNAR=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\oworld.h"\
	

"$(INTDIR)\Odynarrb.obj" : $(SOURCE) $(DEP_CPP_ODYNAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Odynarrb.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oeffect.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OEFFE=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oeffect.obj" : $(SOURCE) $(DEP_CPP_OEFFE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oeffect.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oerrctrl.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OERRC=\
	".\all.h"\
	".\CRC.H"\
	".\gamedef.h"\
	".\mptypes.h"\
	".\ODPLAY.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OERRCTRL.H"\
	".\ofile.h"\
	".\OIMMPLAY.H"\
	".\OLOG.H"\
	".\omisc.h"\
	".\ostr.h"\
	".\ovqueue.h"\
	{$(INCLUDE)}"dplobby.h"\
	{$(INCLUDE)}"immlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OERRC=\
	"c:\mpsdk\VCPP\immprivg.h"\
	

"$(INTDIR)\Oerrctrl.obj" : $(SOURCE) $(DEP_CPP_OERRC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oerrctrl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oerror.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OERRO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Oerror.obj" : $(SOURCE) $(DEP_CPP_OERRO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oerror.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oexpmask.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OEXPM=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\OEXPMASK.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Oexpmask.obj" : $(SOURCE) $(DEP_CPP_OEXPM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oexpmask.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_base.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_BA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_base.obj" : $(SOURCE) $(DEP_CPP_OF_BA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_base.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_base2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_BAS=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_base2.obj" : $(SOURCE) $(DEP_CPP_OF_BAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_base2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_camp.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_CA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_camp.obj" : $(SOURCE) $(DEP_CPP_OF_CA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_camp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_camp2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_CAM=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_camp2.obj" : $(SOURCE) $(DEP_CPP_OF_CAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_camp2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_fact.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_FA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_fact.obj" : $(SOURCE) $(DEP_CPP_OF_FA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_fact.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_fact2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_FAC=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_fact2.obj" : $(SOURCE) $(DEP_CPP_OF_FAC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_fact2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_harb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_HA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_harb.obj" : $(SOURCE) $(DEP_CPP_OF_HA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_harb.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_harb2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_HAR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_harb2.obj" : $(SOURCE) $(DEP_CPP_OF_HAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_harb2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_inn.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_IN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_INN.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_inn.obj" : $(SOURCE) $(DEP_CPP_OF_IN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_inn.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_inn2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_INN=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_INN.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_inn2.obj" : $(SOURCE) $(DEP_CPP_OF_INN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_inn2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_mark.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_MA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Of_mark.obj" : $(SOURCE) $(DEP_CPP_OF_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_mark.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_mark2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_MAR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_mark2.obj" : $(SOURCE) $(DEP_CPP_OF_MAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_mark2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_mine.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_MI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_mine.obj" : $(SOURCE) $(DEP_CPP_OF_MI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_mine.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_mine2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_MIN=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_mine2.obj" : $(SOURCE) $(DEP_CPP_OF_MIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_mine2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_mons.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_MO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_MONS.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_mons.obj" : $(SOURCE) $(DEP_CPP_OF_MO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_mons.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_rese.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_RE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_RESE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_rese.obj" : $(SOURCE) $(DEP_CPP_OF_RE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_rese.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_rese2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_RES=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_RESE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_rese2.obj" : $(SOURCE) $(DEP_CPP_OF_RES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_rese2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_war.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_WA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Of_war.obj" : $(SOURCE) $(DEP_CPP_OF_WA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_war.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Of_war2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OF_WAR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Of_war2.obj" : $(SOURCE) $(DEP_CPP_OF_WAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Of_war2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofile.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFILE=\
	".\all.h"\
	".\gamedef.h"\
	".\obox.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Ofile.obj" : $(SOURCE) $(DEP_CPP_OFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofile.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofiletxt.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFILET=\
	".\all.h"\
	".\gamedef.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\OFILETXT.H"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Ofiletxt.obj" : $(SOURCE) $(DEP_CPP_OFILET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofiletxt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirm.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMLINK.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSERES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ofirm.obj" : $(SOURCE) $(DEP_CPP_OFIRM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirm.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirm2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRM2=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ofirm2.obj" : $(SOURCE) $(DEP_CPP_OFIRM2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirm2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirma.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ofirma.obj" : $(SOURCE) $(DEP_CPP_OFIRMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirma.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmai.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMAI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ofirmai.obj" : $(SOURCE) $(DEP_CPP_OFIRMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmai.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmdie.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMD=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ofirmdie.obj" : $(SOURCE) $(DEP_CPP_OFIRMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmdie.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmdrw.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMDR=\
	".\all.h"\
	".\COLCODE.H"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ofirmdrw.obj" : $(SOURCE) $(DEP_CPP_OFIRMDR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmdrw.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmif.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDER.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ofirmif.obj" : $(SOURCE) $(DEP_CPP_OFIRMI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmif.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmif2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMIF=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ofirmif2.obj" : $(SOURCE) $(DEP_CPP_OFIRMIF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmif2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmif3.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMIF3=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ofirmif3.obj" : $(SOURCE) $(DEP_CPP_OFIRMIF3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmif3.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFIRMR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ofirmres.obj" : $(SOURCE) $(DEP_CPP_OFIRMR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofirmres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oflame.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFLAM=\
	".\all.h"\
	".\COLCODE.H"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\OFLAME.H"\
	".\omisc.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Oflame.obj" : $(SOURCE) $(DEP_CPP_OFLAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oflame.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofont.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OFONT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ofont.obj" : $(SOURCE) $(DEP_CPP_OFONT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ofont.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamcred.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMC=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMUSIC.H"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamcred.obj" : $(SOURCE) $(DEP_CPP_OGAMC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamcred.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamdemo.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMD=\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ogame.h"\
	".\OMOUSE.H"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamdemo.obj" : $(SOURCE) $(DEP_CPP_OGAMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamdemo.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogame.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAME=\
	".\all.h"\
	".\COLCODE.H"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OAUDIO.H"\
	".\OBATTLE.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OEXPMASK.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\ogodres.h"\
	".\OHILLRES.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OLOCBMP.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OPLANT.H"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCK.H"\
	".\OROCKRES.H"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\OSFRMRES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSNOWG.H"\
	".\OSNOWRES.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\OTERRAIN.H"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_CART.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\OU_MONS.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\OUNITALL.H"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\OWALLRES.H"\
	".\OWARPT.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogame.obj" : $(SOURCE) $(DEP_CPP_OGAME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogame.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamemp.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMEM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\OBATTLE.H"\
	".\OBLOB.H"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\ODPLAY.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OERRCTRL.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGET.H"\
	".\OGETA.H"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\OIMMPLAY.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	{$(INCLUDE)}"dplobby.h"\
	{$(INCLUDE)}"immlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OGAMEM=\
	"c:\mpsdk\VCPP\immprivg.h"\
	

"$(INTDIR)\Ogamemp.obj" : $(SOURCE) $(DEP_CPP_OGAMEM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamemp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamency.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMEN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\oresdb.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamency.obj" : $(SOURCE) $(DEP_CPP_OGAMEN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamency.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamend.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMEND=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OINGMENU.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamend.obj" : $(SOURCE) $(DEP_CPP_OGAMEND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamend.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogameset.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMES=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\ODIR.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\OGAMESET.H"\
	".\omisc.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	

"$(INTDIR)\Ogameset.obj" : $(SOURCE) $(DEP_CPP_OGAMES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogameset.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamhall.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMH=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamhall.obj" : $(SOURCE) $(DEP_CPP_OGAMH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamhall.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogammain.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBATTLE.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogammain.obj" : $(SOURCE) $(DEP_CPP_OGAMM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogammain.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogammenu.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMME=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\OAUDIO.H"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogammenu.obj" : $(SOURCE) $(DEP_CPP_OGAMME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogammenu.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamsce2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMS=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\OBLOB.H"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\OFILETXT.H"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGETA.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\opower.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamsce2.obj" : $(SOURCE) $(DEP_CPP_OGAMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamsce2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamscen.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMSC=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBATTLE.H"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\ODIR.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\OFILETXT.H"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ogamscen.obj" : $(SOURCE) $(DEP_CPP_OGAMSC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamscen.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamsing.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGAMSI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBATTLE.H"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGET.H"\
	".\OGETA.H"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogamsing.obj" : $(SOURCE) $(DEP_CPP_OGAMSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogamsing.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogenhill.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGENH=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OHILLRES.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\OTERRAIN.H"\
	".\oworld.h"\
	

"$(INTDIR)\Ogenhill.obj" : $(SOURCE) $(DEP_CPP_OGENH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogenhill.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogenmap.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGENM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogame.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OPLASMA.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\OTERRAIN.H"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ogenmap.obj" : $(SOURCE) $(DEP_CPP_OGENM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogenmap.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogenmap2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGENMA=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogame.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ogenmap2.obj" : $(SOURCE) $(DEP_CPP_OGENMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogenmap2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oget.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGET_=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\ofont.h"\
	".\OGET.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oget.obj" : $(SOURCE) $(DEP_CPP_OGET_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oget.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogeta.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGETA=\
	".\all.h"\
	".\COLCODE.H"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\ofont.h"\
	".\OGETA.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogeta.obj" : $(SOURCE) $(DEP_CPP_OGETA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogeta.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogf_v1.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGF_V=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogf_v1.h"\
	".\OGFILE.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ogf_v1.obj" : $(SOURCE) $(DEP_CPP_OGF_V) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogf_v1.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogfile.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGFIL=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OAUDIO.H"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\oworld.h"\
	

"$(INTDIR)\Ogfile.obj" : $(SOURCE) $(DEP_CPP_OGFIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogfile.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogfile2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGFILE=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCK.H"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSNOWG.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ogfile2.obj" : $(SOURCE) $(DEP_CPP_OGFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogfile2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogfile3.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGFILE3=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OB_PROJ.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogf_v1.h"\
	".\OGFILE.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSNOWG.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ogfile3.obj" : $(SOURCE) $(DEP_CPP_OGFILE3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogfile3.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogfilea.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGFILEA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\OBLOB.H"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odate.h"\
	".\ODB.H"\
	".\ODIR.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ogfilea.obj" : $(SOURCE) $(DEP_CPP_OGFILEA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogfilea.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogodres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGODR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OGAMESET.H"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_GOD.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ogodres.obj" : $(SOURCE) $(DEP_CPP_OGODR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogodres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogrpsel.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OGRPS=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGRPSEL.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ogrpsel.obj" : $(SOURCE) $(DEP_CPP_OGRPS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ogrpsel.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ohelp.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OHELP=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\OFILETXT.H"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMUSIC.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ohelp.obj" : $(SOURCE) $(DEP_CPP_OHELP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ohelp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ohillres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OHILL=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\OHILLRES.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ohillres.obj" : $(SOURCE) $(DEP_CPP_OHILL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ohillres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ohsetres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OHSET=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\OHSETRES.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ohsetres.obj" : $(SOURCE) $(DEP_CPP_OHSET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ohsetres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oimgres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OIMGR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oresx.h"\
	".\ostr.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oimgres.obj" : $(SOURCE) $(DEP_CPP_OIMGR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oimgres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oimmplay.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OIMMP=\
	".\all.h"\
	".\gamedef.h"\
	".\mptypes.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\OIMMPLAY.H"\
	".\omisc.h"\
	".\ostr.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"immlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OIMMP=\
	"c:\mpsdk\VCPP\immprivg.h"\
	

"$(INTDIR)\Oimmplay.obj" : $(SOURCE) $(DEP_CPP_OIMMP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oimmplay.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oinfo.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OINFO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oinfo.obj" : $(SOURCE) $(DEP_CPP_OINFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oinfo.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oingmenu.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OINGM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OINGMENU.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oingmenu.obj" : $(SOURCE) $(DEP_CPP_OINGM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oingmenu.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oisoarea.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OISOA=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OISOAREA.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oisoarea.obj" : $(SOURCE) $(DEP_CPP_OISOA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oisoarea.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Olightn.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OLIGH=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OLIGHTN.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Olightn.obj" : $(SOURCE) $(DEP_CPP_OLIGH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Olightn.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Olightn2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OLIGHT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\OLIGHTN.H"\
	".\omisc.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Olightn2.obj" : $(SOURCE) $(DEP_CPP_OLIGHT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Olightn2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\olocbmp.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OLOCB=\
	".\all.h"\
	".\gamedef.h"\
	".\OBITMAP.H"\
	".\OBLOB2.H"\
	".\ofile.h"\
	".\OLOCBMP.H"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\olocbmp.obj" : $(SOURCE) $(DEP_CPP_OLOCB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\olocbmp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Olog.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OLOG_=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\OLOG.H"\
	".\omisc.h"\
	".\ostr.h"\
	".\ovqueue.h"\
	

"$(INTDIR)\Olog.obj" : $(SOURCE) $(DEP_CPP_OLOG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Olog.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Olonglog.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OLONG=\
	".\ofile.h"\
	".\OLONGLOG.H"\
	

"$(INTDIR)\Olonglog.obj" : $(SOURCE) $(DEP_CPP_OLONG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Olonglog.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Olzw.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OLZW_=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\OLZW.H"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Olzw.obj" : $(SOURCE) $(DEP_CPP_OLZW_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Olzw.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omatrix.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMATR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OHILLRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OPLANT.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTERRAIN.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Omatrix.obj" : $(SOURCE) $(DEP_CPP_OMATR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omatrix.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\omatrix2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMATRI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBITMAP.H"\
	".\OBLOB2.H"\
	".\OCOLTBL.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\omatrix2.obj" : $(SOURCE) $(DEP_CPP_OMATRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\omatrix2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omem.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMEM_=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Omem.obj" : $(SOURCE) $(DEP_CPP_OMEM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omisc.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMISC=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Omisc.obj" : $(SOURCE) $(DEP_CPP_OMISC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omisc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omlink.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMLIN=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMLINK.H"\
	".\ostr.h"\
	

"$(INTDIR)\Omlink.obj" : $(SOURCE) $(DEP_CPP_OMLIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omlink.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omonsres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMONS=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Omonsres.obj" : $(SOURCE) $(DEP_CPP_OMONS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omonsres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omouse.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMOUS=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSE2.H"\
	".\OMOUSECR.H"\
	".\opower.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Omouse.obj" : $(SOURCE) $(DEP_CPP_OMOUS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omouse.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omousecr.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMOUSE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Omousecr.obj" : $(SOURCE) $(DEP_CPP_OMOUSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omousecr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omousefr.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMOUSEF=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSE2.H"\
	".\OMOUSECR.H"\
	".\opower.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Omousefr.obj" : $(SOURCE) $(DEP_CPP_OMOUSEF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omousefr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omousege.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMOUSEG=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSE2.H"\
	".\OMOUSECR.H"\
	".\opower.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Omousege.obj" : $(SOURCE) $(DEP_CPP_OMOUSEG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omousege.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omousesp.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMOUSES=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSE2.H"\
	".\OMOUSECR.H"\
	".\opower.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Omousesp.obj" : $(SOURCE) $(DEP_CPP_OMOUSES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omousesp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omp_crc.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMP_C=\
	".\all.h"\
	".\CRC.H"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OB_FLAME.H"\
	".\OB_HOMIN.H"\
	".\OB_PROJ.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_CART.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\OU_MONS.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Omp_crc.obj" : $(SOURCE) $(DEP_CPP_OMP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omp_crc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omusic.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OMUSI=\
	".\all.h"\
	".\gamedef.h"\
	".\OAUDIO.H"\
	".\oconfig.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMUSIC.H"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OVOLUME.H"\
	

"$(INTDIR)\Omusic.obj" : $(SOURCE) $(DEP_CPP_OMUSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Omusic.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onationa.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONATI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Onationa.obj" : $(SOURCE) $(DEP_CPP_ONATI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onationa.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onationb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONATIO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onationb.obj" : $(SOURCE) $(DEP_CPP_ONATIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"

DEP_CPP_ONATIO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onationb.obj" : $(SOURCE) $(DEP_CPP_ONATIO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onews.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONEWS=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Onews.obj" : $(SOURCE) $(DEP_CPP_ONEWS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onews.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onews2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONEWS2=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onews2.obj" : $(SOURCE) $(DEP_CPP_ONEWS2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onews2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onewseng.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONEWSE=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onewseng.obj" : $(SOURCE) $(DEP_CPP_ONEWSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onewseng.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onewsfre.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONEWSF=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onewsfre.obj" : $(SOURCE) $(DEP_CPP_ONEWSF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onewsfre.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onewsger.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONEWSG=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onewsger.obj" : $(SOURCE) $(DEP_CPP_ONEWSG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onewsger.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onewsspa.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ONEWSS=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Onewsspa.obj" : $(SOURCE) $(DEP_CPP_ONEWSS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Onewsspa.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ooptmenu.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OOPTM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\OAUDIO.H"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\oraceres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ooptmenu.obj" : $(SOURCE) $(DEP_CPP_OOPTM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ooptmenu.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oplant.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OPLAN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OPLANT.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTERRAIN.H"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Oplant.obj" : $(SOURCE) $(DEP_CPP_OPLAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oplant.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oplasma.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OPLAS=\
	".\all.h"\
	".\gamedef.h"\
	".\oconfig.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OPLASMA.H"\
	".\ostr.h"\
	

"$(INTDIR)\Oplasma.obj" : $(SOURCE) $(DEP_CPP_OPLAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oplasma.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Opower.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OPOWE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSERES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Opower.obj" : $(SOURCE) $(DEP_CPP_OPOWE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Opower.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_ai.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_AI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_ai.obj" : $(SOURCE) $(DEP_CPP_OR_AI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_ai.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_eco.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_EC=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_eco.obj" : $(SOURCE) $(DEP_CPP_OR_EC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_eco.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_mil.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_MI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_mil.obj" : $(SOURCE) $(DEP_CPP_OR_MI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_mil.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_nat.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_NA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGETA.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Or_nat.obj" : $(SOURCE) $(DEP_CPP_OR_NA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_nat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_news.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_NE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onews.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otransl.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Or_news.obj" : $(SOURCE) $(DEP_CPP_OR_NE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_news.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_rank.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_RA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_rank.obj" : $(SOURCE) $(DEP_CPP_OR_RA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_rank.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_spy.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_SP=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_spy.obj" : $(SOURCE) $(DEP_CPP_OR_SP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_spy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_tech.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_TE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogodres.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_GOD.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_tech.obj" : $(SOURCE) $(DEP_CPP_OR_TE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_tech.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_town.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_TO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_town.obj" : $(SOURCE) $(DEP_CPP_OR_TO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_town.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Or_trade.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OR_TR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Or_trade.obj" : $(SOURCE) $(DEP_CPP_OR_TR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Or_trade.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oraceres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORACE=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\OGAMESET.H"\
	".\omisc.h"\
	".\oraceres.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ounitres.h"\
	

"$(INTDIR)\Oraceres.obj" : $(SOURCE) $(DEP_CPP_ORACE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oraceres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orain1.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORAIN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\ORAIN.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Orain1.obj" : $(SOURCE) $(DEP_CPP_ORAIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orain1.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orain2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORAIN2=\
	".\imgfun.h"\
	".\ORAIN.H"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Orain2.obj" : $(SOURCE) $(DEP_CPP_ORAIN2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orain2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orain3.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORAIN3=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\ORAIN.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Orain3.obj" : $(SOURCE) $(DEP_CPP_ORAIN3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orain3.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orawres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORAWR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\OHELP.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Orawres.obj" : $(SOURCE) $(DEP_CPP_ORAWR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orawres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orebel.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREBE=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Orebel.obj" : $(SOURCE) $(DEP_CPP_OREBE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orebel.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oregion.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREGI=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oregion.obj" : $(SOURCE) $(DEP_CPP_OREGI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oregion.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oregions.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREGIO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Oregions.obj" : $(SOURCE) $(DEP_CPP_OREGIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oregions.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oremote.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREMO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\obutton.h"\
	".\ODPLAY.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OERRCTRL.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMMPLAY.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dplobby.h"\
	{$(INCLUDE)}"immlib.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OREMO=\
	"c:\mpsdk\VCPP\immprivg.h"\
	

"$(INTDIR)\Oremote.obj" : $(SOURCE) $(DEP_CPP_OREMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oremote.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oremote2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREMOT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OERRCTRL.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Oremote2.obj" : $(SOURCE) $(DEP_CPP_OREMOT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oremote2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oremotem.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREMOTE=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\obutton.h"\
	".\OCRC_STO.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\OLONGLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_CART.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\OU_MONS.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\OUNITALL.H"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Oremotem.obj" : $(SOURCE) $(DEP_CPP_OREMOTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oremotem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oremoteq.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREMOTEQ=\
	".\all.h"\
	".\gamedef.h"\
	".\mptypes.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ostr.h"\
	".\ovqueue.h"\
	

"$(INTDIR)\Oremoteq.obj" : $(SOURCE) $(DEP_CPP_OREMOTEQ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oremoteq.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ores.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORES_=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ores.h"\
	".\ostr.h"\
	".\osys.h"\
	

"$(INTDIR)\Ores.obj" : $(SOURCE) $(DEP_CPP_ORES_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ores.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oresdb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORESD=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\omisc.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	

"$(INTDIR)\Oresdb.obj" : $(SOURCE) $(DEP_CPP_ORESD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oresdb.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orestxt.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OREST=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ORESTXT.H"\
	".\ostr.h"\
	

"$(INTDIR)\Orestxt.obj" : $(SOURCE) $(DEP_CPP_OREST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orestxt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oresx.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_ORESX=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	

"$(INTDIR)\Oresx.obj" : $(SOURCE) $(DEP_CPP_ORESX) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oresx.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orock.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OROCK=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\oresdb.h"\
	".\OROCK.H"\
	".\OROCKRES.H"\
	".\ostr.h"\
	

"$(INTDIR)\Orock.obj" : $(SOURCE) $(DEP_CPP_OROCK) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orock.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orockres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OROCKR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCKRES.H"\
	".\ostr.h"\
	".\OTERRAIN.H"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Orockres.obj" : $(SOURCE) $(DEP_CPP_OROCKR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Orockres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oscroll.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSCRO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oscroll.obj" : $(SOURCE) $(DEP_CPP_OSCRO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oscroll.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ose.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSE_C=\
	".\all.h"\
	".\gamedef.h"\
	".\OAUDIO.H"\
	".\oconfig.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\OGAMESET.H"\
	".\omisc.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\ostr.h"\
	".\OVOLUME.H"\
	

"$(INTDIR)\Ose.obj" : $(SOURCE) $(DEP_CPP_OSE_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ose.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oseres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSERE=\
	".\all.h"\
	".\gamedef.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\ostr.h"\
	".\OVOLUME.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Oseres.obj" : $(SOURCE) $(DEP_CPP_OSERE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oseres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osfrmres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSFRM=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\OGAMESET.H"\
	".\omisc.h"\
	".\oresx.h"\
	".\OSFRMRES.H"\
	".\ostr.h"\
	".\osys.h"\
	

"$(INTDIR)\Osfrmres.obj" : $(SOURCE) $(DEP_CPP_OSFRM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osfrmres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osite.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSITE=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Osite.obj" : $(SOURCE) $(DEP_CPP_OSITE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osite.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ositedrw.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSITED=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogodres.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ositedrw.obj" : $(SOURCE) $(DEP_CPP_OSITED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ositedrw.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oskill.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSKIL=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\ores.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ostr.h"\
	

"$(INTDIR)\Oskill.obj" : $(SOURCE) $(DEP_CPP_OSKIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oskill.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oslidcus.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSLID=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OSLIDCUS.H"\
	".\ostr.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oslidcus.obj" : $(SOURCE) $(DEP_CPP_OSLID) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oslidcus.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oslider.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSLIDE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\ofont.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OSLIDER.H"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oslider.obj" : $(SOURCE) $(DEP_CPP_OSLIDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oslider.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osnow1.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSNOW=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OSNOW.H"\
	".\ostr.h"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Osnow1.obj" : $(SOURCE) $(DEP_CPP_OSNOW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osnow1.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osnow2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSNOW2=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OSNOW.H"\
	".\ostr.h"\
	".\ovgabuf.h"\
	

"$(INTDIR)\Osnow2.obj" : $(SOURCE) $(DEP_CPP_OSNOW2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osnow2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osnowg.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSNOWG=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\OSNOWG.H"\
	".\OSNOWRES.H"\
	".\ostr.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Osnowg.obj" : $(SOURCE) $(DEP_CPP_OSNOWG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osnowg.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osnowres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSNOWR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSNOWRES.H"\
	".\ostr.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Osnowres.obj" : $(SOURCE) $(DEP_CPP_OSNOWR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osnowres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospath.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPAT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ospath.obj" : $(SOURCE) $(DEP_CPP_OSPAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospath.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospathbt.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPATH=\
	".\gamedef.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ospath.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospathbt.obj" : $(SOURCE) $(DEP_CPP_OSPATH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospathbt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospinner.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPIN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSPINNER.H"\
	".\ostr.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ospinner.obj" : $(SOURCE) $(DEP_CPP_OSPIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospinner.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospredbg.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPRE=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\ostr.h"\
	".\osys.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospredbg.obj" : $(SOURCE) $(DEP_CPP_OSPRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospredbg.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospreoff.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPREO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ospreoff.obj" : $(SOURCE) $(DEP_CPP_OSPREO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospreoff.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospresmo.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPRES=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospresmo.obj" : $(SOURCE) $(DEP_CPP_OSPRES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospresmo.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospreuse.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPREU=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospreuse.obj" : $(SOURCE) $(DEP_CPP_OSPREU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospreuse.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osprite.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPRI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSFRMRES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Osprite.obj" : $(SOURCE) $(DEP_CPP_OSPRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osprite.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osprite2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPRIT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\oresdb.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Osprite2.obj" : $(SOURCE) $(DEP_CPP_OSPRIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osprite2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospritea.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPRITE=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTORNADO.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospritea.obj" : $(SOURCE) $(DEP_CPP_OSPRITE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospritea.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osprtres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPRT=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Osprtres.obj" : $(SOURCE) $(DEP_CPP_OSPRT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osprtres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospy.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPY_=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospy.obj" : $(SOURCE) $(DEP_CPP_OSPY_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospy2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPY2=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospy2.obj" : $(SOURCE) $(DEP_CPP_OSPY2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospy2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospya.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSPYA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ospya.obj" : $(SOURCE) $(DEP_CPP_OSPYA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ospya.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ostr.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSTR_=\
	".\omisc.h"\
	".\ostr.h"\
	

"$(INTDIR)\Ostr.obj" : $(SOURCE) $(DEP_CPP_OSTR_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ostr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osys.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSYS_=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\OAUDIO.H"\
	".\obox.h"\
	".\OBULLET.H"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\OCRC_STO.H"\
	".\odate.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OERRCTRL.H"\
	".\of_base.h"\
	".\OF_HARB.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OFLAME.H"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\OGFILE.H"\
	".\ogodres.h"\
	".\OGRPSEL.H"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OINGMENU.H"\
	".\OLOG.H"\
	".\OLONGLOG.H"\
	".\OLZW.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSE2.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Osys.obj" : $(SOURCE) $(DEP_CPP_OSYS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osys.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osys2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSYS2=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\OBULLET.H"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OINGMENU.H"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OOPTMENU.H"\
	".\opower.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCK.H"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\OSNOWG.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\OWARPT.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Osys2.obj" : $(SOURCE) $(DEP_CPP_OSYS2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osys2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osys3.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OSYS3=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSE2.H"\
	".\ostr.h"\
	".\osys.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Osys3.obj" : $(SOURCE) $(DEP_CPP_OSYS3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Osys3.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otalkeng.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTALK=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otalkeng.obj" : $(SOURCE) $(DEP_CPP_OTALK) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otalkeng.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otalkfre.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTALKF=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otalkfre.obj" : $(SOURCE) $(DEP_CPP_OTALKF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otalkfre.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otalkger.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTALKG=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otalkger.obj" : $(SOURCE) $(DEP_CPP_OTALKG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otalkger.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otalkmsg.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTALKM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otalkmsg.obj" : $(SOURCE) $(DEP_CPP_OTALKM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otalkmsg.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otalkres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTALKR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otalkres.obj" : $(SOURCE) $(DEP_CPP_OTALKR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otalkres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otalkspa.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTALKS=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otalkres.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otalkspa.obj" : $(SOURCE) $(DEP_CPP_OTALKS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otalkspa.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otechres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTECH=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_RESE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Otechres.obj" : $(SOURCE) $(DEP_CPP_OTECH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otechres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oterrain.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTERR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\OIMGRES.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTERRAIN.H"\
	".\oworld.h"\
	

"$(INTDIR)\Oterrain.obj" : $(SOURCE) $(DEP_CPP_OTERR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oterrain.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otornado.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTORN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Otornado.obj" : $(SOURCE) $(DEP_CPP_OTORN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otornado.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otown.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWN=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OPLANT.H"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Otown.obj" : $(SOURCE) $(DEP_CPP_OTOWN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otown.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otowna.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWNA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obox.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Otowna.obj" : $(SOURCE) $(DEP_CPP_OTOWNA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otowna.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otownai.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWNAI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\OF_CAMP.H"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\OF_INN.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\of_mons.h"\
	".\OF_RESE.H"\
	".\OF_WAR.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMALL.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	

"$(INTDIR)\Otownai.obj" : $(SOURCE) $(DEP_CPP_OTOWNAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otownai.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otownbld.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWNB=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Otownbld.obj" : $(SOURCE) $(DEP_CPP_OTOWNB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otownbld.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otowndrw.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWND=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\OPLANT.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otowndrw.obj" : $(SOURCE) $(DEP_CPP_OTOWND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otowndrw.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otownif.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWNI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otownif.obj" : $(SOURCE) $(DEP_CPP_OTOWNI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otownif.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otownind.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWNIN=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otechres.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Otownind.obj" : $(SOURCE) $(DEP_CPP_OTOWNIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otownind.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otownres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTOWNR=\
	".\all.h"\
	".\gamedef.h"\
	".\ODB.H"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\OIMGRES.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oraceres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\oworld.h"\
	

"$(INTDIR)\Otownres.obj" : $(SOURCE) $(DEP_CPP_OTOWNR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otownres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otransl.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTRAN=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\OFILETXT.H"\
	".\ogame.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	

"$(INTDIR)\Otransl.obj" : $(SOURCE) $(DEP_CPP_OTRAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otransl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otutor.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTUTO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OAUDIO.H"\
	".\OBATTLE.H"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\OFILETXT.H"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OGAMESET.H"\
	".\OGFILE.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Otutor.obj" : $(SOURCE) $(DEP_CPP_OTUTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otutor.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otutor2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OTUTOR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\OBLOB.H"\
	".\obox.h"\
	".\OBUTT3D.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\OMUSIC.H"\
	".\opower.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\OSLIDCUS.H"\
	".\ostr.h"\
	".\osys.h"\
	".\otransl.h"\
	".\OTUTOR.H"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Otutor2.obj" : $(SOURCE) $(DEP_CPP_OTUTOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Otutor2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_cara.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_CA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ou_cara.obj" : $(SOURCE) $(DEP_CPP_OU_CA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_cara.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_cara2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_CAR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_cara2.obj" : $(SOURCE) $(DEP_CPP_OU_CAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_cara2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_caras.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_CARA=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_caras.obj" : $(SOURCE) $(DEP_CPP_OU_CARA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_caras.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_carat.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_CARAT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_carat.obj" : $(SOURCE) $(DEP_CPP_OU_CARAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_carat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_cart.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_CART=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\OU_CART.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_cart.obj" : $(SOURCE) $(DEP_CPP_OU_CART) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_cart.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_god.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_GO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBUTT3D.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_base.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_god.obj" : $(SOURCE) $(DEP_CPP_OU_GO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_god.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_god2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_GOD=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_GOD.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_god2.obj" : $(SOURCE) $(DEP_CPP_OU_GOD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_god2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_mari.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_MA=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ou_mari.obj" : $(SOURCE) $(DEP_CPP_OU_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_mari.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_mari2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_MAR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_mari2.obj" : $(SOURCE) $(DEP_CPP_OU_MAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_mari2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_marif.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_MARI=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ou_marif.obj" : $(SOURCE) $(DEP_CPP_OU_MARI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_marif.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_maris.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_MARIS=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_maris.obj" : $(SOURCE) $(DEP_CPP_OU_MARIS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_maris.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_marit.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_MARIT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_FACT.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\OF_MINE.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_marit.obj" : $(SOURCE) $(DEP_CPP_OU_MARIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_marit.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_mons.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_MO=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onews.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_MONS.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_mons.obj" : $(SOURCE) $(DEP_CPP_OU_MO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_mons.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ou_vehi.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OU_VE=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ou_vehi.obj" : $(SOURCE) $(DEP_CPP_OU_VE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ou_vehi.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounit.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNIT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odate.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\opower.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\OU_MONS.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounit.obj" : $(SOURCE) $(DEP_CPP_OUNIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounit.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounit2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNIT2=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounit2.obj" : $(SOURCE) $(DEP_CPP_OUNIT2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounit2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounita.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITA=\
	".\all.h"\
	".\color.h"\
	".\CRC.H"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_CART.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\OU_MONS.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\OUNITALL.H"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounita.obj" : $(SOURCE) $(DEP_CPP_OUNITA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounita.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitaac.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAA=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitaac.obj" : $(SOURCE) $(DEP_CPP_OUNITAA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitaac.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitaat.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAAT=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitaat.obj" : $(SOURCE) $(DEP_CPP_OUNITAAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitaat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitac.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAC=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\oraceres.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounitac.obj" : $(SOURCE) $(DEP_CPP_OUNITAC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitac.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitai.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitai.obj" : $(SOURCE) $(DEP_CPP_OUNITAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitai.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitam.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAM=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTERRAIN.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitam.obj" : $(SOURCE) $(DEP_CPP_OUNITAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitam.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitamt.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAMT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitamt.obj" : $(SOURCE) $(DEP_CPP_OUNITAMT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitamt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitat.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAT=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OF_CAMP.H"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\omonsres.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovgabuf.h"\
	".\OWARPT.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounitat.obj" : $(SOURCE) $(DEP_CPP_OUNITAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitat2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAT2=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OF_CAMP.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_CART.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitat2.obj" : $(SOURCE) $(DEP_CPP_OUNITAT2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitat2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitat3.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITAT3=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ogame.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounitat3.obj" : $(SOURCE) $(DEP_CPP_OUNITAT3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitat3.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitatb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITATB=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitatb.obj" : $(SOURCE) $(DEP_CPP_OUNITATB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitatb.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitd.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITD=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitd.obj" : $(SOURCE) $(DEP_CPP_OUNITD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitd.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitdrw.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITDR=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSFRMRES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ounitdrw.obj" : $(SOURCE) $(DEP_CPP_OUNITDR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitdrw.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounithb.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITH=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounithb.obj" : $(SOURCE) $(DEP_CPP_OUNITH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounithb.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ouniti.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\of_mons.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_GOD.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ouniti.obj" : $(SOURCE) $(DEP_CPP_OUNITI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ouniti.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitif.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITIF=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\KEY.H"\
	".\mptypes.h"\
	".\OBUTT3D.H"\
	".\OBUTTCUS.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\OHELP.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\ovqueue.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ounitif.obj" : $(SOURCE) $(DEP_CPP_OUNITIF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitif.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitind.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITIN=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OF_CAMP.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\orebel.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ospy.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitind.obj" : $(SOURCE) $(DEP_CPP_OUNITIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitind.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitm.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITM=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OF_HARB.H"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\OLOG.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounitm.obj" : $(SOURCE) $(DEP_CPP_OUNITM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitm.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounitres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITR=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ounitres.obj" : $(SOURCE) $(DEP_CPP_OUNITR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounitres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ounits.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OUNITS=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\of_mark.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\OU_CARA.H"\
	".\OU_MARI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ounits.obj" : $(SOURCE) $(DEP_CPP_OUNITS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ounits.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovbrowif.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVBRO=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OVBROWIF.H"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	

"$(INTDIR)\Ovbrowif.obj" : $(SOURCE) $(DEP_CPP_OVBRO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovbrowif.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovbrowse.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVBROW=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\opower.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oscroll.h"\
	".\oskill.h"\
	".\ostr.h"\
	".\otransl.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ovbrowse.obj" : $(SOURCE) $(DEP_CPP_OVBROW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovbrowse.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovga.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVGA_=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OCOLTBL.H"\
	".\ofile.h"\
	".\OLOG.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\oresdb.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	".\ovqueue.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ovga.obj" : $(SOURCE) $(DEP_CPP_OVGA_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovga.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovga2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVGA2=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ovga2.obj" : $(SOURCE) $(DEP_CPP_OVGA2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovga2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovgabuf.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVGAB=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ovgabuf.obj" : $(SOURCE) $(DEP_CPP_OVGAB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovgabuf.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovgabuf2.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVGABU=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OCOLTBL.H"\
	".\ofile.h"\
	".\ofont.h"\
	".\OIMGRES.H"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\opower.h"\
	".\oresx.h"\
	".\ostr.h"\
	".\otransl.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ovgabuf2.obj" : $(SOURCE) $(DEP_CPP_OVGABU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovgabuf2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovgalock.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVGAL=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\ostr.h"\
	".\osys.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVGALOCK.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Ovgalock.obj" : $(SOURCE) $(DEP_CPP_OVGAL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovgalock.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovideo.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVIDE=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\OVIDEO.H"\
	{$(INCLUDE)}"amvideo.h"\
	{$(INCLUDE)}"control.h"\
	{$(INCLUDE)}"evcode.h"\
	{$(INCLUDE)}"ksuuids.h"\
	{$(INCLUDE)}"strmif.h"\
	{$(INCLUDE)}"uuids.h"\
	

"$(INTDIR)\Ovideo.obj" : $(SOURCE) $(DEP_CPP_OVIDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovideo.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovolume.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVOLU=\
	".\OAUDIO.H"\
	".\oconfig.h"\
	".\ofile.h"\
	".\oresx.h"\
	".\OVOLUME.H"\
	

"$(INTDIR)\Ovolume.obj" : $(SOURCE) $(DEP_CPP_OVOLU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovolume.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovqueue.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OVQUE=\
	".\all.h"\
	".\gamedef.h"\
	".\ofile.h"\
	".\omisc.h"\
	".\ostr.h"\
	".\ovqueue.h"\
	

"$(INTDIR)\Ovqueue.obj" : $(SOURCE) $(DEP_CPP_OVQUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ovqueue.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ow_fire.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OW_FI=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\OSERES.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\OTERRAIN.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ow_fire.obj" : $(SOURCE) $(DEP_CPP_OW_FI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ow_fire.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ow_plant.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OW_PL=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OPLANT.H"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\ostr.h"\
	".\OTERRAIN.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ow_plant.obj" : $(SOURCE) $(DEP_CPP_OW_PL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ow_plant.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ow_rock.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OW_RO=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresdb.h"\
	".\OROCK.H"\
	".\OROCKRES.H"\
	".\ostr.h"\
	".\OTERRAIN.H"\
	".\oworld.h"\
	

"$(INTDIR)\Ow_rock.obj" : $(SOURCE) $(DEP_CPP_OW_RO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ow_rock.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ow_sound.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OW_SO=\
	".\all.h"\
	".\gamedef.h"\
	".\OAUDIO.H"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oresx.h"\
	".\OSE.H"\
	".\ostr.h"\
	".\osys.h"\
	".\OVOLUME.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Ow_sound.obj" : $(SOURCE) $(DEP_CPP_OW_SO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ow_sound.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ow_wall.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OW_WA=\
	".\all.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	".\WALLTILE.H"\
	

"$(INTDIR)\Ow_wall.obj" : $(SOURCE) $(DEP_CPP_OW_WA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Ow_wall.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Owallres.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OWALL=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\ODB.H"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\ofont.h"\
	".\OGAMESET.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OWALLRES.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	".\WALLTILE.H"\
	

"$(INTDIR)\Owallres.obj" : $(SOURCE) $(DEP_CPP_OWALL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Owallres.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Owarpt.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OWARP=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\ovgabuf.h"\
	".\OWARPT.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Owarpt.obj" : $(SOURCE) $(DEP_CPP_OWARP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Owarpt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oweather.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OWEAT=\
	".\all.h"\
	".\gamedef.h"\
	".\odynarr.h"\
	".\ofile.h"\
	".\ofirmid.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ostr.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	

"$(INTDIR)\Oweather.obj" : $(SOURCE) $(DEP_CPP_OWEAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oweather.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oworld.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OWORL=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\mptypes.h"\
	".\OANLINE.H"\
	".\obutton.h"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ofont.h"\
	".\ogame.h"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\OMOUSECR.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\onews.h"\
	".\OPLANT.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\oremote.h"\
	".\oremoteq.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oscroll.h"\
	".\OSERES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_VEHI.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovbrowse.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\ovqueue.h"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oworld.obj" : $(SOURCE) $(DEP_CPP_OWORL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oworld.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oworld_m.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OWORLD=\
	".\all.h"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\obutton.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\ogame.h"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\OPLANT.H"\
	".\opower.h"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\oskill.h"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oworld_m.obj" : $(SOURCE) $(DEP_CPP_OWORLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oworld_m.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oworld_z.cpp

!IF  "$(CFG)" == "am2 - Win32 Release"

DEP_CPP_OWORLD_=\
	".\all.h"\
	".\COLCODE.H"\
	".\color.h"\
	".\gamedef.h"\
	".\imgfun.h"\
	".\OANLINE.H"\
	".\OAUDIO.H"\
	".\OBITMAP.H"\
	".\OBLOB2.H"\
	".\OBULLET.H"\
	".\obutton.h"\
	".\OCOLTBL.H"\
	".\oconfig.h"\
	".\odynarr.h"\
	".\odynarrb.h"\
	".\OEFFECT.H"\
	".\OEXPMASK.H"\
	".\ofile.h"\
	".\ofirm.h"\
	".\ofirma.h"\
	".\OFIRMDIE.H"\
	".\ofirmid.h"\
	".\ofirmres.h"\
	".\OFLAME.H"\
	".\ofont.h"\
	".\ogame.h"\
	".\ogodres.h"\
	".\OHILLRES.H"\
	".\OIMGRES.H"\
	".\oinfo.h"\
	".\OLIGHTN.H"\
	".\OLOCBMP.H"\
	".\omatrix.h"\
	".\omisc.h"\
	".\OMOUSE.H"\
	".\onation.h"\
	".\onationa.h"\
	".\onationb.h"\
	".\OPLANT.H"\
	".\opower.h"\
	".\ORAIN.H"\
	".\orawres.h"\
	".\oregion.h"\
	".\oregions.h"\
	".\ores.h"\
	".\oresdb.h"\
	".\oresx.h"\
	".\OROCK.H"\
	".\OROCKRES.H"\
	".\OSITE.H"\
	".\oskill.h"\
	".\OSNOW.H"\
	".\OSNOWG.H"\
	".\OSNOWRES.H"\
	".\ospath.h"\
	".\ospreuse.h"\
	".\osprite.h"\
	".\osprtres.h"\
	".\ostr.h"\
	".\osys.h"\
	".\otalkmsg.h"\
	".\OTERRAIN.H"\
	".\OTORNADO.H"\
	".\otown.h"\
	".\otownrec.h"\
	".\otownres.h"\
	".\otransl.h"\
	".\OU_GOD.H"\
	".\ounit.h"\
	".\ounitres.h"\
	".\ovga.h"\
	".\ovgabuf.h"\
	".\OVOLUME.H"\
	".\OWALLRES.H"\
	".\OWEATHER.H"\
	".\oworld.h"\
	".\OWORLDMT.H"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\Oworld_z.obj" : $(SOURCE) $(DEP_CPP_OWORLD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "am2 - Win32 Debug"


"$(INTDIR)\Oworld_z.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

