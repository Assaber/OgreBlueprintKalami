function(rod tag dst_dir store git_addr)
if(NOT EXISTS ${dst_dir})
	if(EXISTS ${store})
		message("[${tag}]Restore from an existing archive...")
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar xzf ${store}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			RESULT_VARIABLE TAR_RESULT
		)
		
		if(TAR_RESULT EQUAL "0")
			message(STATUS "[${tag}]Extraction successful")
		else()
			message(FATAL_ERROR "[${tag}]Failed to extract ${store}")
		endif()
	
	else()
		message(STATUS "Folder does not exist. Cloning from git...")

		execute_process(
			COMMAND git clone ${git_addr} ${dst_dir}
			RESULT_VARIABLE GIT_RESULT
		)

		# 检查git命令是否执行成功
		if(GIT_RESULT EQUAL "0")
			message(STATUS "[${tag}]Git clone successful")
		else()
			message(FATAL_ERROR "[${tag}]Failed to clone from git")
		endif()
	endif()
endif()
endfunction()