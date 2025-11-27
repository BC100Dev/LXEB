# This part over here will house the entire source code for SEB (Safe Exam Browser) itself.
# LXEB went for the redesign of being able to dynamically switch between multiple lockdown browser protocols.

# in order to not suffer too much, I decided to let this entire thing get automated.
# nice and easy

file(GLOB SEB_Submodules "${PROJECT_MODULE_ROOT}/*")
foreach (SEB_Submodule ${SEB_Submodules})
    if (EXISTS "${SEB_Submodule}/SourceInfo.cmake")
        message(STATUS "SEB - Loading submodule at ${SEB_Submodule}")
        include("${SEB_Submodule}/SourceInfo.cmake")
    endif ()
endforeach ()