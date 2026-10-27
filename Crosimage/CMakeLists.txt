add_executable(Crosimage WIN32)

# Keep the logical groups from the former Visual Studio project.
# List sources explicitly: unused Qqt helpers are intentionally not compiled.
function(crosimage_sources group)
    target_sources(Crosimage PRIVATE ${ARGN})
    source_group("${group}" FILES ${ARGN})
endfunction()

crosimage_sources("AltTab"
    AltTabLabel.cpp
    AltTabLabel.h
    AltTabView.cpp
    AltTabView.h
)

crosimage_sources("ImageView"
    ImageView.cpp
    ImageView.h
)

crosimage_sources("Qqt"
    LogFile.cpp
    LogFile.h
    Qqt/AbstractButtonPropertyRedirects.cpp
    Qqt/AbstractButtonPropertyRedirects.h
    Qqt/AbstractSpinBoxPropertyRedirects.cpp
    Qqt/AbstractSpinBoxPropertyRedirects.h
    Qqt/Action.cpp
    Qqt/Action.h
    Qqt/BoxLayout.cpp
    Qqt/BoxLayout.h
    Qqt/ButtonGroup.cpp
    Qqt/ButtonGroup.h
    Qqt/ByteArrayWithDataStream.h
    Qqt/CheckBox.cpp
    Qqt/CheckBox.h
    Qqt/ComboBox.cpp
    Qqt/ComboBox.h
    Qqt/CompactHBoxLayout.cpp
    Qqt/CompactHBoxLayout.h
    Qqt/CompactVBoxLayout.cpp
    Qqt/CompactVBoxLayout.h
    Qqt/CtorProcessorT.h
    Qqt/DialogButtonBox.cpp
    Qqt/DialogButtonBox.h
    Qqt/DirString.cpp
    Qqt/DirString.h
    Qqt/FileFacility.cpp
    Qqt/FileFacility.h
    Qqt/FileReader.cpp
    Qqt/FileReader.h
    Qqt/FileWriter.cpp
    Qqt/FileWriter.h
    Qqt/FormLayout.cpp
    Qqt/FormLayout.h
    Qqt/GridLayout.cpp
    Qqt/GridLayout.h
    Qqt/GroupBox.cpp
    Qqt/GroupBox.h
    Qqt/HBoxLayout.cpp
    Qqt/HBoxLayout.h
    Qqt/Label.cpp
    Qqt/Label.h
    Qqt/Layout.cpp
    Qqt/Layout.h
    Qqt/LineEdit.cpp
    Qqt/LineEdit.h
    Qqt/Menu.cpp
    Qqt/Menu.h
    Qqt/MenuButton.cpp
    Qqt/MenuButton.h
    Qqt/MsgBox.cpp
    Qqt/MsgBox.h
    Qqt/New.h
    Qqt/Object.cpp
    Qqt/Object.h
    Qqt/Parser.cpp
    Qqt/Parser.h
    Qqt/ProgressBar.cpp
    Qqt/ProgressBar.h
    Qqt/PropertyRedirect.h
    Qqt/PushButton.cpp
    Qqt/PushButton.h
    Qqt/RadioButton.cpp
    Qqt/RadioButton.h
    Qqt/RotatableToolBar.cpp
    Qqt/RotatableToolBar.h
    Qqt/Settings.cpp
    Qqt/Settings.h
    Qqt/SignalEmitter.cpp
    Qqt/SignalEmitter.h
    Qqt/SpinBox.h
    Qqt/Splitter.cpp
    Qqt/Splitter.h
    Qqt/StringStatus.cpp
    Qqt/StringStatus.h
    Qqt/TabWidget.cpp
    Qqt/TabWidget.h
    Qqt/ToString.cpp
    Qqt/ToString.h
    Qqt/ToolBar.cpp
    Qqt/ToolBar.h
    Qqt/ToolButton.cpp
    Qqt/ToolButton.h
    Qqt/TopResult.h
    Qqt/VBoxLayout.cpp
    Qqt/VBoxLayout.h
    Qqt/Widget.cpp
    Qqt/Widget.h
    Qqt/WidgetPropertyRedirects.cpp
    Qqt/WidgetPropertyRedirects.h
    Qqt/includeMe.h
    Qqt/preprocessor.h
)

crosimage_sources("Qqt\\DB"
    Qqt/DB/DbWorker.cpp
    Qqt/DB/DbWorker.h
    Qqt/DB/PreparedSqlQuery.cpp
    Qqt/DB/PreparedSqlQuery.h
    Qqt/DB/SqliteDb.cpp
    Qqt/DB/SqliteDb.h
)

crosimage_sources("Resource Files"
    crosimage.qrc
    images/folder-down-24.png
    images/folder-up-24.png
)

crosimage_sources("Thumb"
    FileSystemThread.cpp
    FileSystemThread.h
    MicrosoftLnkReader.cpp
    MicrosoftLnkReader.h
    ThumbCache.cpp
    ThumbCache.h
    ThumbDelegate.cpp
    ThumbDelegate.h
    ThumbDirPainter.cpp
    ThumbDirPainter.h
    ThumbModel.cpp
    ThumbModel.h
    ThumbVideoWorker.cpp
    ThumbVideoWorker.h
    ThumbView.cpp
    ThumbView.h
    ThumbWorker.cpp
    ThumbWorker.h
    TxtLnkProcessor.cpp
    TxtLnkProcessor.h
)

crosimage_sources("src"
    CApplication.cpp
    CApplication.h
    CMainWindow.cpp
    CMainWindow.h
    DirCache.cpp
    DirCache.h
    DirCacheItem.cpp
    DirCacheItem.h
    DirDb.cpp
    DirDb.h
    main.cpp
    Screenshot.cpp
    Screenshot.h
    pch.cpp
    pch.h
)

crosimage_sources("src\\settings"
    CrSettings.cpp
    CrSettings.h
    SettingsDialog.cpp
    SettingsDialog.h
)

target_include_directories(Crosimage PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(Crosimage PRIVATE CROSIMAGE_PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
target_link_libraries(Crosimage PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Svg Qt6::Xml
)
target_precompile_headers(Crosimage PRIVATE pch.h)
set_target_properties(Crosimage PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Crosimage>"
)

if(MSVC)
    target_compile_options(Crosimage PRIVATE /utf-8 /MP)
    set_property(TARGET Crosimage PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(WIN32 AND CROSIMAGE_DEPLOY_QT)
    if(NOT TARGET Qt6::windeployqt)
        message(FATAL_ERROR "Qt windeployqt is required; install it or configure with -DCROSIMAGE_DEPLOY_QT=OFF")
    endif()
    add_custom_command(TARGET Crosimage POST_BUILD
        COMMAND Qt6::windeployqt
            --no-translations --no-compiler-runtime
            "$<IF:$<CONFIG:Debug>,--debug,--release>"
            "$<TARGET_FILE:Crosimage>"
        COMMENT "Copying Qt runtime libraries and plugins"
        VERBATIM
    )
endif()
