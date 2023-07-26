# Derf's Simple Build System (dsbs)

DSBS is a simple build system that I'm aiming to replace the internal build system that Visual Studio 2017 uses. Just like the the internal build system from VS2017 dsbs uses the concept of Solution and Projects. You can think of a solution as a contain of projects and projects were your source code and other data that a project contains resides. The reason why I want a custom builder over simply using other like gnu make, cmake, or premake is that I'm not a big fan of makefiles because they use a proprietary syntax I don't like. Thats why dsbs uses json because its widely used and very to the general developer populus. Plus makefile format is rather verbose. A build system to me should for the most part be almost automatic when it comes to the build/cleaning process. The developer shouldn't have to worry about editing the build file at all and just focus on their project or projects. How dsbs does this is through the use of the c++ filesystem which is a rather useful api that was added during c++17. I love it. It very useful.

If you want to understand how dsbs works check either the test in the bin folder which contains an example of compiling both shared (note not supported yet) and static library types and exectuable. Just open the solution.json file to see how to implement all project types or simply look at the main directories solution.json file.

As for the "dsbs" command just type "./dsbs -h" in the terminal to view the commands help menu. It will describe how to use the dsbs command.

I will be adding in a wiki once I finalize version 1.0.0 for dsbs. I'll post a link here.

This project is currently under a dual license depending on which region your from. If you from Europe your now under the GPLv3 license see LICENSE_EU for details. If your from other regions your still covered under the MIT license see LICENSE for details. 