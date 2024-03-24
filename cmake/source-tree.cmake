MACRO(DoSource)
	SET(LocalFixed)
	
	FOREACH(L ${Local}) 
		IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${wd}${L}")
			LIST(APPEND LocalFixed "${CMAKE_SOURCE_DIR}/${wd}${L}")
		ENDIF()
	ENDFOREACH()
	
	LIST(APPEND All ${LocalFixed})
	
	FOREACH(S ${Local})
		IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${wd}${S}")			
			SET(wd "${wd}${S}/")
			
			LIST(APPEND pwd "${wd}")
			
			INCLUDE("${CMAKE_SOURCE_DIR}/${wd}dir.cmake")
			DoSource()
			
			LIST(REMOVE_AT pwd -1)
			LIST(GET pwd -1 wd)
		ENDIF()
	ENDFOREACH()
ENDMACRO()

MACRO(StartSource src)
	SET(wd)
	SET(pwd "/")
	SET(testid 0)
	SET(Local ${src})
	SET(ExcludeDoc)
	SET(deps)
	DoSource()
ENDMACRO()
