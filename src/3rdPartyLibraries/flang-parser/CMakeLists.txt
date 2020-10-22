
if (ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION)
    add_library(FlangParser
        flang-external-builder-main.cc
    )
endif()
