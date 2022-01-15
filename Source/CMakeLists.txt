
haiku_add_addon(FlipperInputFilter WorkspaceFlipFilter.cpp)

target_link_libraries(FlipperInputFilter be)

#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

execute_process(
	COMMAND finddir B_USER_NONPACKAGED_ADDONS_DIRECTORY
	OUTPUT_VARIABLE B_ADDONS_PREFIX
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

install(TARGETS FlipperInputFilter LIBRARY DESTINATION "${B_ADDONS_PREFIX}/input_server/filters")


haiku_add_executable(WorkspaceFlipper
	SettingsApp.cpp
	SettingsWindow.cpp
	ZonePreview.cpp
	SettingsApp.rdef
)

target_link_libraries(WorkspaceFlipper be)

install(TARGETS WorkspaceFlipper RUNTIME DESTINATION apps)

#TODO switch to preferences when building a package?
#install(TARGETS WorkspaceFlipper RUNTIME DESTINATION preferences)

#set_property(TARGET WorkspaceFlipper PROPERTY RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
