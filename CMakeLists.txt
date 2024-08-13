cmake_minimum_required(VERSION 3.16)
# Sets ${PROJECT_NAME}
project(extract-source-files)

# ---- Compiler ----
# C++17
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)
# C++ Compiler
message(STATUS "C++ Compiler:" ${CMAKE_CXX_COMPILER})
message(STATUS "C Compiler:" ${CMAKE_C_COMPILER})
set(CLANGXX ${CMAKE_CXX_COMPILER})

# Define executable to be built
# When you add_executable() before set(CMAKE_CXX_STANDARD)
add_executable(${PROJECT_NAME} ExtractSourceFiles.cpp)

# ---- LLVM, Clang ----
# Loads settings from external project LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# Show where LLVM includes are
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "Found LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

# Find llvm-config
find_program(llvm-config "llvm-config" PATHS ${LLVM_TOOLS_BINARY_DIR} NO_DEFAULT_PATH REQUIRED)
# Get llvm_libs
execute_process(COMMAND ${llvm-config} --libs # --system-libs--ignore-libllvm
    OUTPUT_VARIABLE llvm_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


include("${CLANG_DIR}/ClangConfig.cmake")
# find_package(clang REQUIRED CONFIG)
message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")
include_directories(${CLANG_INCLUDE_DIRS})

message(STATUS "LLVM libs: ${llvm_libs}")
string(REPLACE "-l" "" llvm_libs ${llvm_libs})
string(REPLACE " " ";" llvm_libs ${llvm_libs})
message(STATUS "LLVM libs: ${llvm_libs}")

# Set clang_libs
# https://jonasdevlieghere.com/understanding-the-clang-ast/
set(clang_libs
    clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangTooling
  clangToolingRefactoring
)
# message(STATUS "Clang libs: $${clang_libs}")

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${llvm_ldflags}")

# ---- Link everything together ----
# Order matters! Rule of thumb: the lower something is in this list, the more
# code relies on its symbols and thus the more important it is.
# The linker will learn about unresolved symbols from higher file first, and thus
# have then the most opportunities to resolve them.
# On macOS, these rules don't seem to be that important.
target_link_libraries(${PROJECT_NAME}
    ${clang_libs} ${llvm_libs} 
)