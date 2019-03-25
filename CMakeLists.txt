# Nebula demos

# 3.7.2 required for using FindVulkan
cmake_minimum_required(VERSION 3.7.2)

get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include(${FIPS_ROOT_DIR}/cmake/fips.cmake)

fips_setup(PROJECT demos)

SET(N_PROJECT_NAME scripts)

fips_project(${N_PROJECT_NAME})

    fips_begin_sharedlib(${N_PROJECT_NAME})
        enable_language(CSharp)
        fips_dir(data/scripts)
            fips_files(
                main.cs
            )
        fips_dir(data/scripts/engine)
            fips_files(
                api.cs
                consolehook.cs
            )
        fips_dir(data/scripts/util)
            fips_files(
                string.cs
            )
        fips_deps(xna_math)
    fips_end_sharedlib()
    target_compile_options(${N_PROJECT_NAME}
        PRIVATE "/langversion:6"
        PRIVATE "/unsafe"
        )
    # we need to convert .pdb to .mdb files to load debugging symbols correctly
    # add_custom_command(TARGET ${N_PROJECT_NAME}
    #     COMMAND pdb2mdb ${FIPS_PROJECT_DEPLOY_DIR}/${N_PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    #     VERBATIM
    #     POST_BUILD
    #     )   

fips_include_directories(.)
# fips_ide_group(demos)
fips_add_subdirectory(code)
fips_finish()

