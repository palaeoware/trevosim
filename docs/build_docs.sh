#This command will build the documentation on a system that has python3, sphinx, and the correct theme installed. 
#On Linux that erquires these commands (assuming Python3 is already installed):
#sudo apt-get install python3-sphinx
#sudo apt install python3-pip
#Before 24.04 - sudo pip3 install sphinx_rtd_theme
#24.04 and after: sudo apt-get -y install python3-sphinx-rtd-theme
echo "Now building trevosim doc"
sphinx-build -E ./ ../docs_build
#2025 - added -E switch as this forces updates - cache/environment was causing issues adding new files
