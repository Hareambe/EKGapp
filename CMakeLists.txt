cmake_minimum_required(VERSION 3.16)
project(calqlatr LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Charts  )

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(calqlatrexample WIN32 MACOSX_BUNDLE
    main.cpp
    src/handlers/hdf5_handler.cpp
    src/handlers/hdf5_graph.cpp
    src/handlers/hdf5_handler.h
    src/handlers/hdf5_graph.h
    src/handlers/datasetmodel.cpp
    src/handlers/datasetmodel.h
)

# HDF5 Library and Header Integration
set(HDF5_HEADER_PATH "${CMAKE_SOURCE_DIR}/android/include/hdf5")  # Path to HDF5 headers
set(HIGHFIVE_HEADER_PATH "${CMAKE_SOURCE_DIR}/android/include/highfive")  # Path to HDF5 headers
set(HDF5_LIB_PATH "${CMAKE_SOURCE_DIR}/android/libs/x86_64") # Path to HDF5 shared libraries
set(CMAKE_ANDROID_EMULATOR_SOFTWARE_RENDERING ON)

include_directories(${HDF5_HEADER_PATH} ${HIGHFIVE_HEADER_PATH})

# Link HDF5 Libraries
target_link_libraries(calqlatrexample PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Charts

    ${HDF5_LIB_PATH}/libhdf5.so
    ${HDF5_LIB_PATH}/libhdf5_hl.so
)

qt_add_qml_module(calqlatrexample
    URI demos.calqlatr
    VERSION 1.0
    QML_FILES
        "Main.qml"
        "content/Display.qml"
        "content/NumberPad.qml"
        "content/CalculatorButton.qml"
        "content/BackspaceButton.qml"
        "content/calculator.js"
    RESOURCES
        "content/images/backspace.svg"
        "content/images/backspace_fill.svg"
)

if(ANDROID)
    # Install the HDF5 shared libraries to the libs/x86_64 directory within the APK
    install(FILES
        "${HDF5_LIB_PATH}/libhdf5.so"
        "${HDF5_LIB_PATH}/libhdf5_hl.so"
        DESTINATION libs/x86_64
    )

    # Specify additional Android properties
    set_target_properties(calqlatrexample
        PROPERTIES
           QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android"
            QT_ANDROID_TARGET_SDK_VERSION "33"

    )

    # Ensure that the HDF5 shared libraries are copied to the APK build directory after building
    add_custom_command(TARGET calqlatrexample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:calqlatrexample>/libs/x86_64"
    )
    add_custom_command(TARGET calqlatrexample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HDF5_LIB_PATH}/libhdf5.so"
            "$<TARGET_FILE_DIR:calqlatrexample>/libs/x86_64/libhdf5.so"
    )
    add_custom_command(TARGET calqlatrexample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HDF5_LIB_PATH}/libhdf5_hl.so"
            "$<TARGET_FILE_DIR:calqlatrexample>/libs/x86_64/libhdf5_hl.so"
    )
endif()

if(IOS)
    set(asset_catalog_path "ios/Assets.xcassets")
    target_sources(calqlatrexample PRIVATE "${asset_catalog_path}")
    set_source_files_properties(${asset_catalog_path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(calqlatrexample
        PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
endif()

install(TARGETS calqlatrexample
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET calqlatrexample
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})
