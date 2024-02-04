# myVIRTUOSE
Towards building haptic datasets with the Haption Virtuose 6D TAO

# Installation
Clone this repo
Copy paste the virtuoseAPI.dll into C:/Windows/System32/ - to ensure the built executable can find it in the path when running.
That's it!

# Building the project
Generate CMake Cache
Build all executables
Preferred naming convention
  "[0-9]*-*/[0-9]*.cpp"

# Notes of caution
- In any loop of sampling feedback from the Virtuose, ensure to include a Sleep() statement.

fin
