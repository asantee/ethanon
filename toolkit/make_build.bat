@call :PrintHeader

@rem Check input arguments

@IF "%1" == "" call :PrintHelp
@IF "%1" == "" exit

@rem Main actions

SET BuildDirBase=#build#

@IF /i %1 == R call :MakeReleaseBuild_AndExit
@IF /i %1 == D call :MakeDebugBuild_AndExit
@IF /i %1 == M call :MakeMixedBuild_AndExit

@echo Unknown parametr %1
@exit

@rem HighLevel Functions Implementation

:PrintHeader
  @echo \
  @echo  ----------------------------
  @echo  ETHANON TOOLKIT BUILD SCRIPT
  @echo  ----------------------------
  @echo /
@exit /b

:PrintHelp
  @echo Usage: make_build.bat R/D/M
  @echo     where R - Release binaries + Resources
  @echo           D - Debug binaries + Resources
  @echo           M - Mixed (Release binaries + Debug executables + Resources).
  @echo Examples: make_build.bat R
  @echo           make_build.bat D
  @echo           make_build.bat M
@exit /b

:MakeReleaseBuild_AndExit
  @echo Making Release Build...

  @call :CopyBuildFiles Release Release
@exit

:MakeDebugBuild_AndExit
  @echo Making Debug Build...

  @call :CopyBuildFiles Debug Debug
@exit

:MakeMixedBuild_AndExit
  @echo Making Mixed Build...

  @call :CopyBuildFiles Mixed Release 
  @call :AppendBuildExecs Mixed Debug
@exit



@rem Utility Functions Implementation



:CopyBuildFiles
  @call :SetBuildDirAndConfiguration %1 %2
  @call :ResetFilePostfix
  @call :CleanupDir %BuildDir%

  SET CopyDestinationDir=%BuildDir%
    SET CopySourceDir=Source\#bin#\%Configuration%
    @call :CopyFilesByExtensions .exe .dll

    SET CopySourceDir=Ethanon Toolkit
    @call :CopyFilesByExtensions .dll .enml .as

  SET CopyDestinationDir=%BuildDir%\data
    SET CopySourceDir=Ethanon Toolkit\data
    @call :CopyFilesByExtensions .bmp .png .dds .fnt .cg .enml

  SET CopyDestinationDir=%BuildDir%\effects
    SET CopySourceDir=Ethanon Toolkit\effects
    @call :CopyFilesByExtensions .par

  SET CopyDestinationDir=%BuildDir%\entities
    SET CopySourceDir=Ethanon Toolkit\entities
    @call :CopyFilesByExtensions .ent .bmp .png .dds .jpg .tga

  SET CopyDestinationDir=%BuildDir%\entities\normalmaps
    SET CopySourceDir=Ethanon Toolkit\entities\normalmaps
    @call :CopyFilesByExtensions .bmp .png .tga

  SET CopyDestinationDir=%BuildDir%\scenes
    SET CopySourceDir=Ethanon Toolkit\scenes
    @call :CopyFilesByExtensions .esc

  SET CopyDestinationDir=%BuildDir%\soundfx
    SET CopySourceDir=Ethanon Toolkit\soundfx
    @call :CopyFilesByExtensions .ogg

  SET CopyDestinationDir=%BuildDir%\particles
    SET CopySourceDir=Ethanon Toolkit\particles
    @call :CopyFilesByExtensions .png

  SET CopyDestinationDir=%BuildDir%\tools
    SET CopySourceDir=Ethanon Toolkit\tools
    @call :CopyDirectories BMFont SciTE
@exit /b



:AppendBuildExecs
  @call :SetBuildDirAndConfiguration %1 %2
  @call :SetFilePostfix

  SET CopyDestinationDir=%BuildDir%
    SET CopySourceDir=Source\#bin#\%Configuration%
    @call :CopyFilesByExtensions .exe
@exit /b



:SetBuildDirAndConfiguration
  SET BuildDir=%BuildDirBase%%1
  SET Configuration=%2
@exit /b

:ResetFilePostfix
  SET FilePostfix=
@exit /b

:SetFilePostfix
  SET FilePostfix=.%Configuration%
@exit /b

:CopyFilesByExtensions
  @IF NOT EXIST "%CopyDestinationDir%" MD "%CopyDestinationDir%"
  @FOR %%f IN (%*) DO copy "%CopySourceDir%\*%%f" %CopyDestinationDir%\*%FilePostfix%%%f"
@exit /b

:CopyDirectories
  @IF NOT EXIST "%CopyDestinationDir%" MD "%CopyDestinationDir%"
  @FOR %%d IN (%*) DO IF NOT EXIST "%CopyDestinationDir%\%%d" MD "%CopyDestinationDir%\%%d"
  @FOR %%d IN (%*) DO copy "%CopySourceDir%\%%d\*" "%CopyDestinationDir%\%%d\"
@exit /b

:CleanupDir
  @IF EXIST %1 RD %1 /Q/S
  @MD %1 
@exit /b

