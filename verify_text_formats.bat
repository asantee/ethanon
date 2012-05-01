@echo \
@echo  -----------------------------------
@echo  ETHANON TOOLKIT VERIFICATION SCRIPT
@echo  -----------------------------------
@echo /

@IF NOT EXIST .\toolkit\Utilites\#bin#\Release\TextFormatVerifier.exe goto :errorMessage


@echo Source code and shaders...
@.\toolkit\Utilites\#bin#\Release\TextFormatVerifier.exe ASCII .\*.c;*.cpp;*.h;*.hpp;*.inl;*.cg angelscript;BoostSDK;CgSDK;DX9SDK

@echo Scripts...
@.\toolkit\Utilites\#bin#\Release\TextFormatVerifier.exe UTF-8 .\*.as;*.json angelscript;BoostSDK;CgSDK;DX9SDK

@echo Text resources (Unicode)...
@.\toolkit\Utilites\#bin#\Release\TextFormatVerifier.exe UTF-16 .\*.par;*.ent;*.esc Source;src

@echo Text resources (ASCII)...
@.\toolkit\Utilites\#bin#\Release\TextFormatVerifier.exe ASCII .\*.fnt;*.enml Source;src

@echo _
@echo [ Ethanon Toolkit verification complete. ]

@goto :theEnd

:errorMessage

@echo Verification tool not found.
@echo Please, build release of this solution first: .\toolkit\Utilites\Utilites.sln
@echo _
@echo [ Ethanon Toolkit verification failed. ]

:theEnd