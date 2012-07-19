diffimages
==========

Compile
-------

You need the Qt library
`sudo apt-get install libqt4-dev`

If you want to edit sources you should install also QtCreator
`sudo apt-get install qtcreator`

If you dont have QtCreator open a terminal in the sources directory and run
`qmake`
`make` 

If you have QtCreator open _diffimages.pro_ with
`qtcreator diffimages.pro`

If you want to install diffimages in /usr/bin
`make install`


Create the deb
--------------

* rename sources folder in _diffimages-0.1.0_
* compress the sources in _diffimages_0.1.0.orig.tar.gz_
* open terminal inside _diffimages-0.1.0_ and run `debuild -S -sa --lintian-opts -i`
* `cd ..`
* `sudo pbuilder build *.dsc`
