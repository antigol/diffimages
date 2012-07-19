diffimages
==========

To make a deb file.

* rename sources folder in `diffimages-0.1.0`
* compress the sources in `diffimages_0.1.0.orig.tar.gz`
* open terminal inside `diffimages-0.1.0 and run debuild -S -sa --lintian-opts -i`
* `cd ..`
* `sudo pbuilder build *.dsc`
