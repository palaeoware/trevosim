.. _requirements:

Compiling, Installation, and Requirements
==========================================

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

For v3.X - QT Creator + QT v6.x using Qt installer and MinGW (64-bit)
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

For the TREvoSim v3.0.0 release a build was created using an stand install of Qt6 using the QT online installer available from the `Qt website <https://doc.qt.io/qt-6/get-and-install-qt.html>`_. If following this approach, ensuring the installation includes the latest Qt v6 release, and Qt Creator will allow build of the software. To achieve this, open CMakeLists.txt in Qt creator following installation, select to build a release following the `Qt creator documentation <https://doc.qt.io/qtcreator/creator-building-targets.html>`_, and then initiate a build (ctrl + B / Build, then Build Project). This will create an executable that can be launched by double clicking. To create a build that includes the tests, it will be necessary to modify the cmake file as per the instructions provided with the `Google Test framework  <https://google.github.io/googletest/quickstart-cmake.html>`_. 

*Note:* At the time of release of v3.0.0 the authors temporarily have limited access to windows machines to allow us to make the above changes to cmake, and provide more explicit instructions. We expect to make a patch release with those changes, and featuring improved documentation for Qt v6.x builds, in May 2024. 

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

3. To build the software and test suite, download source code, launch Qt Creator, and open the .cmake file.
4. Configure build and follow standard debug/release procedure.
   
  - See the Qt Mac Deployment tool: https://doc.qt.io/qt-6/macos-deployment.html#the-mac-deployment-tool 
  - Also see the volume package information to make a .dmg: https://doc.qt.io/qt-6/macos-deployment.html#volume-name 


Installation
------------

From the TREvoSim GitHub repository pre-compiled binary releases and packaged installers can be downloaded. For Windows users we provide both a portable binary release (.zip; v1-3) - which just needs extracting to a convenient location as per the instructions on the release - and a self contained installer (currently v1-2 only). For Mac we provide a zip containing the TREvoSim program that can be downloaded from the TREvoSim GitHub repository. To install the software, double click on the .dmg and follow the on screen instructions. You may be required to the approve the software in security and privacy settings before it will launch. For Linux users, the above instructions will allow the software to be built using a limited number of lines of bash. Please contact palaeoware@gmail.com if you encounter any issues.

Requirements
------------

TREvoSim has no minimum requirements as such, and will run on most standard systems (Windows/Linux/Mac); it however has not been tested on versions of Windows older than Windows 10, before Ubuntu 22.04, and below macOS High Sierra. Performance will benefit from high processor speed and increased number of processor cores, with large amounts (>4GB) of available RAM recommended for large simulations. Graphics card performance is not relevant as GPUs are not currently used in the program's calculation pipeline. A fast hard drive (e.g. SSD) is recommend when intensive logging is enabled; as slow I/O response time can affect the iteration cycle speed.