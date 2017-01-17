# Microsoft Developer Studio Project File - Name="PBCVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PBCVC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PBCVC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PBCVC.mak" CFG="PBCVC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PBCVC - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PBCVC - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PBCVC - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /Ot /I "../include" /I "." /I "./self-compiled" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ./self-compiled/gmp.lib ./libeay32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"lib"

!ELSEIF  "$(CFG)" == "PBCVC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ./gmp.lib ./libeay32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PBCVC - Win32 Release"
# Name "PBCVC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\ecc\a_param.c
# End Source File
# Begin Source File

SOURCE=..\ecc\curve.c
# End Source File
# Begin Source File

SOURCE=..\ecc\d_param.c
# End Source File
# Begin Source File

SOURCE=..\misc\darray.c
# End Source File
# Begin Source File

SOURCE=..\misc\extend_printf.c
# End Source File
# Begin Source File

SOURCE=..\ecc\f_param.c
# End Source File
# Begin Source File

SOURCE=..\arith\fasterfp.c
# End Source File
# Begin Source File

SOURCE=..\arith\fastfp.c
# End Source File
# Begin Source File

SOURCE=..\arith\field.c
# End Source File
# Begin Source File

SOURCE=..\arith\fieldquadratic.c
# End Source File
# Begin Source File

SOURCE=..\misc\fops.c
# End Source File
# Begin Source File

SOURCE=..\arith\fp.c
# End Source File
# Begin Source File

SOURCE=.\getime.cpp
# End Source File
# Begin Source File

SOURCE=..\test\IBCES.c
# End Source File
# Begin Source File

SOURCE=..\misc\memory.c
# End Source File
# Begin Source File

SOURCE=..\arith\montfp.c
# End Source File
# Begin Source File

SOURCE=..\arith\naivefp.c
# End Source File
# Begin Source File

SOURCE=..\ecc\pairing.c
# End Source File
# Begin Source File

SOURCE=..\ecc\param.c
# End Source File
# Begin Source File

SOURCE=..\misc\parse.c
# End Source File
# Begin Source File

SOURCE=..\arith\poly.c
# End Source File
# Begin Source File

SOURCE=..\arith\random.c
# End Source File
# Begin Source File

SOURCE=..\misc\symtab.c
# End Source File
# Begin Source File

SOURCE=..\misc\tracker.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\pbc.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_a1_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_a_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_assert.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_curve.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_d_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_darray.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_e_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_f_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_field.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_fieldmpz.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_fieldquadratic.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_fops.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_fp.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_g_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_hilbert.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_memory.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_mnt.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_mpc.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_pairing.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_param.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_parse.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_poly.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_random.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_singular.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_symtab.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_time.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_tracker.h
# End Source File
# Begin Source File

SOURCE=..\include\pbc_utils.h
# End Source File
# Begin Source File

SOURCE=..\include\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
