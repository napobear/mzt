#
#++
#
#
# 	#     #
# 	##   ## ###### #####
# 	# # # #     #    #
# 	#  #  #    #     #
# 	#     #   #      #
# 	#     #  #       #
# 	#     # ######   #
#	------------------
#	   My Z80 Tools
#
##
#  AUTHOR:
#  Piergiorgio Betti <pbetti@lpconsul.net>
#
#  LICENSE: See "License.txt" under MZT top-level source directory
#
#  HISTORY:
#  -[Date]- -[Who]------------- -[What]---------------------------------------
#  20141027 P. Betti            Creation date
#--
#

# Set this to desired installation path
INSTALL_PREFIX=/usr/local
#


DATE=20141027
VERSION=1.0.0
RELFILES=CMakeLists.txt \
	License.txt \
	Makefile \
	mzt.kdev4 \
	mdz80 \
	mld80 \
	mzmac

all: mzt


mzt:
	if [ ! -f build/Makefile ]; then \
		 mkdir -p build; \
		 cd build; \
		 cmake -DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX) ../;\
		 cd ..; \
	fi; \
	cd build; make; \
	cd ..

install: all
	cd build; \
	make install; \
	cd ..

clean:
	rm -rf build
	rm -f *.tar.bz2 *.tar.gz

release:
	tar jcf mzt-$(VERSION).tar.bz2 $(RELFILES)
	tar zcf mzt-$(VERSION).tar.gz $(RELFILES)

depend:
	@echo "Depencies are generated at compile time."
