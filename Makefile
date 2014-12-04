# Makefile

# Cette commande fonctionne sur unix, et renvoie le nom de le l'OS,
# Mais elle ne fonctione pas sous windows, donc elle renvoie une chaine vide.
OS = $(shell uname)
ifeq ($(OS),Linux)
	MAKE=make
	CMAKE=cmake
else ifeq ($(OS),Darwin)
	MAKE=make
	CMAKE=cmake
else
	MAKE=mingw32-make
	CMAKE="C:\Program Files\CMake 2.8\bin\cmake.exe"
	UNIX= -G "MinGW Makefiles"
	OS=Windows
endif

default: release

################################################################################

before:
	$(CMAKE) -E make_directory build/Debug
	$(CMAKE) -E make_directory build/Release
	$(CMAKE) -E make_directory build/MinSizeRel

################################################################################

before_debug: 
	$(CMAKE) -E make_directory build/Debug
	$(CMAKE) -E make_directory build/Release
	$(CMAKE) -E make_directory build/MinSizeRel

################################################################################

debug: before_debug
	(cd build/Debug && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=Debug && $(MAKE) -j 4)

release: before
	(cd build/Release && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)

minrelease: before
	(cd build/MinSizeRel && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=MinSizeRel && $(MAKE) -j 4)

################################################################################

clean:
	$(CMAKE) -E remove_directory bin
	$(CMAKE) -E remove_directory build
	$(CMAKE) -E remove_directory usr

installer: release
	"C:\Program Files\NSIS\makensis.exe" installer_script.nsi

dpkg: release
	(mkdir -p usr/bin)
	(mkdir -p usr/share/doc)
	(cp CHANGELOG.txt usr/share/doc/)
	(cp LICENSE.txt usr/share/doc/)
	(cp README.txt usr/share/doc/)
	(cp bin/Release/o3prmeditor usr/bin/)
	(cd .. && dpkg-deb --build o3prmeditor)


