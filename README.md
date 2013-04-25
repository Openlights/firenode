FireNode
========

Firelight UDP->USB bridge.  Connects to strand controllers via VCP (or libusb in the future).
Listens on UDP 3020 for broadcasts from [FireMix](https://github.com/craftyjon/firemix)


Dependencies
------------

* Qt4
* QtSerialPort (http://qt-project.org/wiki/QtSerialPort)


Building on Linux
-----------------

    qmake
    make


Building on Windows
-------------------

I have tested using VS2008 and the Qt4 VS plugin.  With this setup, you can just open the vcproj file and build.
Other configurations are usable at your own risk.
