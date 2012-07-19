diffimages
==========

To make a deb file.

* rename sources folder in _diffimages-0.1.0_
* compress the sources in _diffimages_0.1.0.orig.tar.gz_
* open terminal inside _diffimages-0.1.0_ and run `debuild -S -sa --lintian-opts -i`
* `cd ..`
* `sudo pbuilder build *.dsc`
