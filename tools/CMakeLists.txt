
include (ValaPrecompile)
find_package(PkgConfig REQUIRED)

pkg_check_modules (GRX_CALIBRATE_DEPS REQUIRED
    glib-2.0
    gobject-2.0
    gio-2.0
    gudev-1.0
)

vala_precompile (GRX_CALIBRATE_C GRX_CALIBRATE_VALA calibrate.vala
    PACKAGES
        grx-3.0
        gio-2.0
        gudev-1.0
    OPTIONS
        --vapidir=${CMAKE_SOURCE_DIR}/bindings/vala/
    DEPENDS
        ${CMAKE_SOURCE_DIR}/bindings/vala/grx-3.0.vapi
)

add_executable (grx-calibrate ${GRX_CALIBRATE_C})
target_compile_options (grx-calibrate PUBLIC "-w")
target_include_directories (grx-calibrate PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${GRX_CALIBRATE_DEPS_INCLUDE_DIRS}
)
target_link_libraries (grx-calibrate ${GRX_CALIBRATE_DEPS_LIBRARIES} ${SHARED_LIBRARY_NAME})

install (TARGETS grx-calibrate RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})