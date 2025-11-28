
Hoe je de LSP configuratie voor een cmake prject kan vernieuwen.

https://www.reddit.com/r/neovim/comments/17rhvtl/guide_how_to_use_clangd_cc_lsp_in_any_project/



 Guide how to use clangd C/C++ LSP in any project regardless of its build system

I see that question often asked in various places, so I wrote a guide how to generate compile_commands.json for your existing project that does not use CMake, regardless of it's structure or build system. The up-to-date version can be found in here, but I will also copy-paste it into this reddit post.

(you can also use this method in other IDEs that support CMake natively, like CLion)
How to use clangd C/C++ LSP in any project

tl;dr: If you want to just know the method, skip to How to section

Clangd is a state-of-the-art C/C++ LSP that can be used in every popular text editors like Neovim, Emacs or VS Code. Even CLion uses clangd under the hood. Unfortunately, clangd requires compile_commands.json to work, and the only way to painlessly generate it is to use CMake.

But what if I tell you you can quickly hack your way around that, and generate compile_commands.json for any project, no matter how compilcated? I have used that way at work for years, originaly because I used CLion which supported only CMake projects - but now I use that method succesfully with clangd and Neovim.
Method summary

Basically what we need to achieve is to create a CMake file that will generate a compile_commands.json file with information about:

    All source files

    All include directories

    External libraries

    Precompiler definitions

We can do that easily without really caring about if the CMake-generate result will compile at all - we don't need to rewrite our existing build system, just hack a CMake file that will generate enough information for Clangd to work.
Prerequisities

    CMake

    clangd

How to

First, create a CMakeLists.txt file in the root folder of your projects, with content similar to this:

cmake_minimum_required(VERSION 3.8)
project(my_project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Change path from /src if needed, or add more directories
file(GLOB_RECURSE sources
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        )
# Add precompiler definitions like that:
add_definitions(-DSOME_DEFINITION)

add_executable(my_app ${sources})

# Add more include directories if needed
target_include_directories(my_app PUBLIC "{CMAKE_SOURCE_DIR}/include")

# If you have precompiled headers you can add them like this
target_precompiled_headers(my_app PRIVATE "${CMAKE_SOURCE_DIR}/src/pch.h")

Modify it according to your project structure, and run:

cmake -S . -G "Unix Makefiles" -B cmake

which will generate the CMake output inside cmake directory. Check if compile_commands.json is there.

NOTE: You need to run that command every time you add/remove a source file in your project.

If you need more (ex. include external libraries like Boost), check out CMake documentation

Now you have two options:

    Symlink compile_commands.json to your root project folder:

      ln -s cmake/compile_commands.json .

OR

    Create .clangd file in your root project folder, with the following contents:

      CompileFlags:
        CompilationDatabase: "cmake"

Now open the project in you editor and everything should work (assuming clangd LSP is started).


