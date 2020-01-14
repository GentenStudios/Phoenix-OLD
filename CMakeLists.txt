cmake_minimum_required(VERSION 3.8)
project(Phoenix)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

add_subdirectory(Phoenix)
