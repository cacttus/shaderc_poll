
cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Utils
function(GET_ARCH_CONFIG)
  set(ARCH_CONFIG "_")  

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SIZE Lib)
    string(APPEND ARCH_CONFIG x86)
  else()
    string(APPEND ARCH_CONFIG x64) 
  endif()
   
  # Target
  if(GENERATOR_IS_MULTI_CONFIG)
    message("Error: Generator is MULTI CONFIG")
    return()
    # "Multi-config generators use CMAKE_CONFIGURATION_TYPES as the set of configurations and ignore CMAKE_BUILD_TYPE."
  else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(ARCH_CONFIG ${ARCH_CONFIG}d PARENT_SCOPE)
    else ()
      set(ARCH_CONFIG ${ARCH_CONFIG}r PARENT_SCOPE)
    endif()
  endif() 
endfunction()  
  
message("CMAKE_MODULE_PATH='" ${CMAKE_MODULE_PATH} "' CMAKE_PREFIX_PATH='" ${CMAKE_PREFIX_PATH} "' CMAKE_TOOLCHAIN_PATH='" ${CMAKE_TOOLCHAIN_PATH} "'")
    
# ** Global Directories **  
set(SCP_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
set(SCP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
set(SCP_BIN_DEBUG_DIR ${SCP_BIN_DIR})
set(SCP_BIN_RELEASE_DIR ${SCP_BIN_DIR})


# VG 
set(SCP_PROJECT_VERSION 1.0.1)
set(SCP_TARGET_NAME shaderc_poll)
project(${SCP_TARGET_NAME} VERSION ${SCP_PROJECT_VERSION} LANGUAGES CXX C)

if(WIN32) 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/ext/") 
message("Set Module Path to " ${CMAKE_MODULE_PATH} "Win32")
endif() 

# VG
add_executable(${SCP_TARGET_NAME} 
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderCPoll.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/src/Utils.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/src/ColoredConsole.cpp
)
  
get_arch_config()  
 
set(SCP_OUTPUT_NAME ${SCP_TARGET_NAME}-${PROJECT_VERSION}${ARCH_CONFIG})
message("Output: " ${SCP_OUTPUT_NAME})
 
set_target_properties(${PROJECT_NAME} PROPERTIES 
  OUTPUT_NAME ${SCP_OUTPUT_NAME}
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SCP_BIN_DEBUG_DIR}
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SCP_BIN_RELEASE_DIR}
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SCP_BIN_DEBUG_DIR}
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${SCP_BIN_DEBUG_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${SCP_LIB_DIR}
)
  
# Packages
find_package(Threads REQUIRED) #This is better than -pthread

if(UNIX)
find_package(X11 REQUIRED)
endif()
 
# Includes
# Libraries
set(ADDL_LIBS "")
if(WIN32)
  set(SCP_WIN32_COMPILE_OPTS "")

  #Unicode required for building with Unicode Character Set (vs MBCS)
  string(APPEND CMAKE_CXX_FLAGS " -DUNICODE")
  string(APPEND CMAKE_CXX_FLAGS " -D_UNICODE")

  #Suppress error
  string(APPEND CMAKE_CXX_FLAGS " -D_CRT_NO_VA_START_VALIDATION")

  #Multicore build
  string(APPEND CMAKE_CXX_FLAGS " /MP") #NOte: originally CMAKE_CXX_FLAGS

  target_compile_options(${SCP_TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:" ${SCP_WIN32_COMPILE_OPTS})
  target_compile_options(${SCP_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:" ${SCP_WIN32_COMPILE_OPTS} -D_DEBUG -D_GLIBCXX_DEBUG)
 
  #TODO: test/ remove these
  set(SCP_ADDL_LIBS "ws2_32.lib" "winmm.lib" "imm32.lib" "version.lib" "dbghelp.lib" "setupapi.lib" "iphlpapi.lib")
elseif(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
set(SCP_GCC_LINK_GLOBAL -pthread) 

  target_compile_options(${SCP_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:" -D_DEBUG -D_GLIBCXX_DEBUG ">")
  target_compile_options(${SCP_TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:" -Ofast ">")

  target_link_options(${SCP_TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:" ${SCP_GCC_LINK_GLOBAL} ">")
  target_link_options(${SCP_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:" ${SCP_GCC_LINK_GLOBAL} ">")
   
  set(SCP_ADDL_LIBS ncurses)
endif() 
target_link_directories(${PROJECT_NAME} PRIVATE ${SCP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SCP_ADDL_LIBS})
