# DEBIAN NOTE: This file is (obviously) just a simple stub.  It did not
#   exist in any form in the original.

# The below can be customized for taste.
builddir = build
make = make

# This target forwards the make to the build directory (unless it is one of
# those below, of course)
.DEFAULT moria:
	@if ( test \! -d $(builddir) ) then $(make) builddir; fi
	@echo "*** Entering $(builddir)/ to actually make the package."
	( cd $(builddir) && $(make) $@ || exit 1 )

# This target is used to avoid complaints from dpkg-source, use with caution!
#   See README.debian-build for details.
clean:
	if ( test -d $(builddir) ) then rm -r $(builddir); fi

# To forward clean requests to the build directory (not actually used by my
#   package at this point.
buildclean:
	@if ( test -d $(builddir) ) then                                      \
	  ( cd $(builddir) && $(make) clean || exit 1 )                       \
	fi

# This target tests if the build directory exists, and complains if it does.
#   It is called from debian/rules, and is placed in this makefile simply so
#   it can inherit the builddir variable.
isclean:
	@if ( test -d $(builddir) ) then                                      \
	  echo -e "\nERROR: You must remove the $(builddir) directory before using dpkg-buildpackage";\
	  echo "       due to limitations in dpkg-source.  "\""make clean"\"" from the base dir"; \
	  echo -e "       will do this for you.  See README.debian-build for details.\n";\
	  exit 1;                                                             \
	fi

# This target actually makes the build directory and populates it with the
#   required symlinks.  It also makes WARNING files so that people who haven't
#   read README.debian-build hopefully will not modify files in this directory.
builddir:
	@ echo "*** Making $(builddir)/ which will contain symlinks to the sources..."
	mkdir $(builddir) || exit 1
	chmod 700 $(builddir) || exit 1
	( cd $(builddir) && ln -si ../files ../unix/* ../source/* . || exit 1 )
	( cd $(builddir) && echo -e "WARNING: Do not modify anything in this directory; it may be purged without\nwarning.  If you want to change the source do it in the source/ or unix/ dirs\nto which these symlinks point.  See README.debian-build for details." >> zz-WARNING && cp -ip zz-WARNING 00-WARNING && cp -ip ../debian/README.debian-build . || exit 1 )
	@ echo "*** Done making $(builddir)/"
	@ echo
