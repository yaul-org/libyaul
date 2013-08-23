SET(CMAKE_SYSTEM_NAME Sega-Saturn)
SET(CMAKE_SYSTEM_PROCESSOR SH7604)

INCLUDE(CMakeForceCompiler)

# Specify the cross compiler.
CMAKE_FORCE_C_COMPILER(sh-elf-gcc GNU)
