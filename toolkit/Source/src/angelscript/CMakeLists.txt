#Codigo verificadro de plataforma
if(WIN32)	
	set(angelHeaders
		include/angelscript.h
		source/as_array.h
		source/as_atomic.h
		source/as_builder.h
		source/as_bytecode.h
		source/as_callfunc.h		
		source/as_compiler.h
		source/as_config.h
		source/as_configgroup.h
		source/as_context.h
		source/as_criticalsection.h
		source/as_datatype.h
		source/as_debug.h
		source/as_gc.h
		source/as_generic.h
		source/as_map.h
		source/as_memory.h
		source/as_module.h
		source/as_objecttype.h
		source/as_outputbuffer.h
		source/as_parser.h
		source/as_property.h
		source/as_restore.h
		source/as_scriptcode.h
		source/as_scriptengine.h
		source/as_scriptfunction.h
		source/as_scriptnode.h
		source/as_scriptobject.h
		source/as_string.h
		source/as_string_util.h
		source/as_symboltable.h
		source/as_texts.h
		source/as_thread.h
		source/as_tokendef.h
		source/as_tokenizer.h
		source/as_typeinfo.h
		source/as_variablescope.h
	)
	
	set(angelSource
		source/as_atomic.cpp
		source/as_builder.cpp
		source/as_bytecode.cpp
		source/as_callfunc.cpp
		source/as_callfunc_arm.cpp
		source/as_callfunc_mips.cpp
		source/as_callfunc_ppc.cpp
		source/as_callfunc_ppc_64.cpp
		source/as_callfunc_sh4.cpp
		source/as_callfunc_x64_gcc.cpp
		source/as_callfunc_x64_mingw.cpp
		source/as_callfunc_x64_msvc.cpp
		source/as_callfunc_x64_msvc_asm.asm		
		source/as_callfunc_x86.cpp
		source/as_callfunc_xenon.cpp
		source/as_compiler.cpp
		source/as_configgroup.cpp
		source/as_context.cpp
		source/as_datatype.cpp
		source/as_gc.cpp
		source/as_generic.cpp
		source/as_globalproperty.cpp
		source/as_memory.cpp
		source/as_module.cpp
		source/as_objecttype.cpp
		source/as_outputbuffer.cpp
		source/as_parser.cpp
		source/as_restore.cpp
		source/as_scriptcode.cpp
		source/as_scriptengine.cpp
		source/as_scriptfunction.cpp
		source/as_scriptnode.cpp
		source/as_scriptobject.cpp
		source/as_string.cpp
		source/as_string_util.cpp
		source/as_thread.cpp
		source/as_tokenizer.cpp
		source/as_typeinfo.cpp
		source/as_variablescope.cpp
	)

	#Gera bibioea estatica angelcript pra wWindows
	add_library(angelscript STATIC ${angelHeaders} ${angelSource} )

	#Cria o sistema de pastas no VS
	source_group(Header_Files FILES ${angelHeaders})
	source_group(Source_Files FILES ${angelSource})

elseif(UNIX)

	set(angelSource
		source/as_atomic.cpp
		source/as_builder.cpp
		source/as_bytecode.cpp
		source/as_callfunc.cpp
		source/as_callfunc_arm.cpp
		source/as_callfunc_mips.cpp
		source/as_callfunc_ppc.cpp
		source/as_callfunc_ppc_64.cpp
		source/as_callfunc_sh4.cpp
		source/as_callfunc_x64_gcc.cpp
		source/as_callfunc_x64_mingw.cpp
		source/as_callfunc_x64_msvc.cpp
		source/as_callfunc_x64_msvc_asm.asm		
		source/as_callfunc_x86.cpp
		source/as_callfunc_xenon.cpp
		source/as_compiler.cpp
		source/as_configgroup.cpp
		source/as_context.cpp
		source/as_datatype.cpp
		source/as_gc.cpp
		source/as_generic.cpp
		source/as_globalproperty.cpp
		source/as_memory.cpp
		source/as_module.cpp
		source/as_objecttype.cpp
		source/as_outputbuffer.cpp
		source/as_parser.cpp
		source/as_restore.cpp
		source/as_scriptcode.cpp
		source/as_scriptengine.cpp
		source/as_scriptfunction.cpp
		source/as_scriptnode.cpp
		source/as_scriptobject.cpp
		source/as_string.cpp
		source/as_string_util.cpp
		source/as_thread.cpp
		source/as_tokenizer.cpp
		source/as_typeinfo.cpp
		source/as_variablescope.cpp
	)

	#Gera biblioteca estatica angelcript pra MacOsX
	add_library(angelscript STATIC ${angelSource} )

#Fim da verificacao de plataformas
endif()
	
	


