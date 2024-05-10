.. _requirements:

Compiling, Testing, Installation, and Requirements
==================================================

Compiling from Source
----------------------

Windows 64-bit
^^^^^^^^^^^^^^

For v1.X,2.X - QT Creator + QT v5.x using MSYS2 (64-bit) and MinGW (64-bit)
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of QT v5.x 64-bit.

#. Download and run the latest version of `MSYS2 <https://www.msys2.org/>`_ for 64-bit Windows. This will be name "MSYS2-x86_64-..." for the 64-bit installer.
#. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
#. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MSYS2 shell before re-running the command to finish.
#. Once MSYS2 and pacman are fully updated run the following command to install QT 5.x and its dependencies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
#. Optional - if you intend on debugging the software in QT and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
#. At this stage you should have the following under the MSYS2 install location:
    * {install location}/mingw64 (Main ming64 folder)
    * {install location}/mingw64/bin/qmake.exe (QMake for QT version)
    * {install location}/mingw64/bin/g++.exe (C++ complier)
    * {install location}/mingw64/bin/gcc.exe (C complier)
    * {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
#. You should now be able to find the required libraries under "{install location}/mingw64/bin" and the required header (.h) files for QT v5.x.
#. Open the .pro file in QT Creator, and then use the information above to setup a new 64-bit ming64 kit. Follow standard QT Creator debug/release procedure.

For v3.X - QT Creator + QT v6.x using Qt installer and MSVC2019 (64-bit)
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

1. Install Qt 6.X on your system by running the installer from Qt: https://www.qt.io/download
2. Install Microsoft Visual Studio 2019 installed. See https://visualstudio.microsoft.com/vs/older-downloads/. Note Visual Studio 2022 should also work but has not yet been tested with TREvoSim.
3. Download source code.
4. In QT Creator open a new project by navigating to and openning the TREvoSim CMakeLists.txt file.
5. Configure build and follow standard debug/release procedure.
   
  - See the Qt Windows Deployment tool: https://doc.qt.io/qt-6/windows-deployment.html

6. To package the application as Windows Installation Binary for distribution we recommend using Inno Setup: https://jrsoftware.org/isinfo.php

  - Inno Setup is a free installer for Windows programs by Jordan Russell and Martijn Laan.
  - The file TREvoSim_Installer_v3.x.x_dev.iss contained under the ./inno_setup folder is provided as a template. This file should be modified and used to create the Windows Installation Binary. It expects all support .DLLs and the .EXE to be placed in a ./bin folder. The Inno Setup will then create the new install binary in a folder called ./build


Ubuntu 22.04 64-bit - QT Creator + QT v6.x using GCC (64-bit)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To compile from command line
""""""""""""""""""""""""""""

1. Install GCC and Qt using system packages:

.. code-block:: console

  sudo apt-get install build-essential libgl1-mesa-dev
  sudo apt install qt6-base-dev libqt6core5compat6-dev


2. Download source code and navigate to folder, or alternatively clone using Git:

.. code-block:: console

  git clone https://github.com/palaeoware/trevosim.git
  cd trevosim

3. Within TREvoSim folder, run the following command to build the software:

.. code-block:: console

 cmake --build .

4. Launch the software by double clicking on the TREvoSim binary that has been created in this folder.

Using Qt creator
""""""""""""""""

1. Install Qt 6.X on your system by running the installer from Qt: https://www.qt.io/download
2. Download source code, launch Qt Creator, and open the CMakeLists.txt file. Configure build and follow standard debug/release procedure.

MacOS - QT Creator + QT v6.x using Clang and xcode
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Xcode can be downloaded from the Apple Store or Apple's developer website (including older versions of Xcode): https://developer.apple.com/download/. We recommend always using the latest Xcode available from Apple that has been tested with the Qt version you are using.
2. Install Qt 6.X and Qt Creator on your system by running the installer from Qt: https://www.qt.io/download. An alternative may be to install via homebrew: 

.. code-block:: console

  $ brew install qt

3. To build the software and test suite, download source code, launch Qt Creator, and open the CMakeLists.txt file.
4. Configure build and follow standard debug/release procedure.
   
  - See the Qt Mac Deployment tool: https://doc.qt.io/qt-6/macos-deployment.html#the-mac-deployment-tool 
  - Also see the volume package information to make a .dmg: https://doc.qt.io/qt-6/macos-deployment.html#volume-name 

Testing
-------

TREvoSim >= v3 come with a suite of tests that verify the core simulator/generator code. These can be called via one of two methods:

1. As a standalone program - for this we leverage the GoogleTest Mocking anf Testing Framework (https://github.com/google/googletest) which is pulled in automatically by CMake and built alongside the TREvoSim main program - creating a separate standalone test program called TREvoSimTest. This program can be called from the command line. Note that for Windows there is a requirement for the support .DLLs and the .EXE to be placed in the same folder, alternatively this program can be added into the Inno Setup during the Installer creation stage.
2. From the QT Creator application. Once the test program has been build (see point 1 above) you can run the test suite with the QT Cretor by going to the 'Tools > Tests > Run All Tests' from the main menu. This will build the application if not already done so and display the results in the 'Test Result' tab which normally appears at the bottom of the worksapce.

Installation
------------

From the TREvoSim GitHub repository pre-compiled binary releases and packaged installers can be downloaded. 

Windows 64-bit
^^^^^^^^^^^^^^

For Windows users we provide both a portable binary release (.zip; v1-3) - which just needs extracting to a convenient location as per the instructions on the release - and a self contained installer (currently v1-2 only).

MacOS
^^^^^
For Mac we provide a zip containing the TREvoSim program that can be downloaded from the TREvoSim GitHub repository. To install the software, double click on the .dmg and follow the on screen instructions. You may be required to the approve the software in security and privacy settings before it will launch.

Note: this binary was build and tested on an Apple M1 processor as such users on other chipsets might need to complie and build their own TREvoSim binaries.

Ubuntu 64-bit / Linux
^^^^^^^^^^^^^^^^^^^^^

For Linux users, the above instructions will allow the software to be built using a limited number of lines of bash. Please contact palaeoware@gmail.com if you encounter any issues.

Requirements
------------

TREvoSim has no minimum requirements as such, and will run on most standard systems (Windows/Linux/Mac); it however has not been tested on versions of Windows older than Windows 10, before Ubuntu 22.04, and below macOS High Sierra. Performance will benefit from high processor speed and increased number of processor cores, with large amounts (>4GB) of available RAM recommended for large simulations. Graphics card performance is not relevant as GPUs are not currently used in the program's calculation pipeline. A fast hard drive (e.g. SSD) is recommend when intensive logging is enabled; as slow I/O response time can affect the iteration cycle speed.