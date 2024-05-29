.. _requirementsinstallation:
.. include:: <isonum.txt>

Requirements and Installation from Binaries
===========================================

Requirements
------------

TREvoSim has no minimum requirements as such, and will run on most standard systems (Windows/Linux/Mac); however, it has not been tested on versions of Windows older than Windows 10, versions of Ubuntu older than 18.04 (v1,2) or 22.04 (v3), and below macOS High Sierra. Performance will benefit from high processor speed and increased number of processor cores, with large amounts (|gt|4GB) of available RAM recommended for large simulations. Graphics card performance is not relevant as GPUs are not currently used in the program's calculation pipeline. A fast hard drive (e.g. SSD) is recommend when intensive logging is enabled; as slow I/O response time can affect the iteration cycle speed.

Installation from Binaries
--------------------------

Binary download locations
^^^^^^^^^^^^^^^^^^^^^^^^^

You can download pre-compiled binary releases and packaged installers from the TREvoSim GitHub repository:

  - https://github.com/palaeoware/trevosim/releases/tag/v3.0.0
  - https://github.com/palaeoware/trevosim/releases/tag/v2.0.0
  - https://github.com/palaeoware/trevosim/releases/tag/v1.0.0


Windows 64-bit
^^^^^^^^^^^^^^

For Windows users we provide two means of installation:

1. A portable binary release within a .ZIP archive. To run this, all you need to do is extract the zip to a convenient location, and double click on the .exe file.
2. A self contained installer - download and double click on the file, which will guide you though the installation process.

MacOS
^^^^^
For TREvoSim v1.X and v2.X we provide a .ZIP archive containing the TREvoSim program that can be downloaded from the TREvoSim GitHub repository.

For TREvoSim v3.X we provide a .DMG image to install the software. Download and double click on the .dmg and follow the on screen instructions. You may be required to the approve the software in security and privacy settings before it will launch.

Note: this binary was build and tested on an Apple M1 processor as such users on other chipsets might need to compile and build their own TREvoSim binaries.

Ubuntu 64-bit / Linux
^^^^^^^^^^^^^^^^^^^^^

For Linux users, please see the 'Compiling from Source and Testing' instructions on the next page. This uses a limited number of lines of bash to compile TREvoSim on your computer. Please contact palaeoware@gmail.com if you encounter any issues.