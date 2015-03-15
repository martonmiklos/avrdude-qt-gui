This program is a cross platform graphical interface to the Avrdude programmer. I wanted to make it very similar to the AVRStudio's programming dialog. It only uses Qt so It should run fine on Linux, Windows, Mac Os, and may other OS's which supported by Qt.  The program uses the xml files provided with the AVR Studio, to provide easy interface to modify the fuse bits for example. In the revisions > 27 there is a support for using an SQLite database extracted from the AVR studio XML files. 

The project is very early status, so I do not released anything to the downloads yet. 
As soon as it achieve the release status I will make Debian packages and static Windows builds to it. Until then you could checkout the latest code from the svn repo and compile it for yourself. 

