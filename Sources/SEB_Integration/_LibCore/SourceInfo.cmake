CollectSources(${PROJECT_MODULE_ROOT} LibCoreSources)

add_library(SEB SHARED ${LibCoreSources})
SetTargetOutputDir(SEB "${OUTPUT_DIRECTORY_ROOT}/modules/SEB/")