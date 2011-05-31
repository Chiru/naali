#!/bin/bash
set -e
set -x

# A script to make sure QT+QT3D+QTMobility builds and installs with correct parameters
# -- for linux environment
# -- At the moment uses QT 4.7.3, QT3D HEAD, QTMobility 1.2.0
# -- and qtscriptgenerator

#---- Set variables for the build
nprocs=`grep -c "^processor" /proc/cpuinfo` 
export QTBUILDDIR=$HOME/qt
export QTTARGETDIR=/usr/local/qt-releases/v4.7.3
#export QTINSTALLDIR=/usr/local/qt-releases/qt
mkdir -p $QTBUILDDIR
cd $QTBUILDDIR

export QTDIR=$QTINSTALLDIR
export PATH=$QTDIR/bin:$PATH

#---- Make sure some extra dependencies are present for builds
sudo aptitude install libxv-dev libbluetooth-dev

#---- Build QT
git clone git://gitorious.org/qt/qt.git
cd qt
git checkout v4.7.3
./configure --prefix=$QTTARGETDIR --disable-qt3support -dbus -phonon-backend | tee qt.configure.log
make -j $nprocs | tee qt.build.log
sudo make install | tee qt.install.log

#---- Build QT3D
git clone git://gitorious.org/qt-labs/qt3d.git
cd qt3d
sudo ln -s $QTBUILDDIR/qt/include/QtGui/private $QTTARGETDIR/include/QtGui/private
sudo ln -s $QTBUILDDIR/qt/include/QtCore/private $QTTARGETDIR/include/QtCore/private
sudo ln -s $QTBUILDDIR/qt/include/QtDeclarative/private $QTTARGETDIR/include/QtDeclarative/private
sudo ln -s $QTBUILDDIR/qt/include/QtOpenGL/private $QTTARGETDIR/include/QtOpenGL/private
#Overcome qt3d bugs in integration:
sed -i "s/0x040703/0x040704/" ./src/quick3d/qdeclarativeeffect.cpp
sed -i "s/0x040703/0x040704/" ./src/imports/threed/shaderprogram.cpp
$QTTARGETDIR/bin/qmake opengl.pro -r
make -j $nprocs | tee qt3d.build.log
sudo make install | tee qt3d.install.log
cd ..

#----- Building QT mobility
git clone git://gitorious.org/qt-mobility/qt-mobility.git
cd qt-mobility
git checkout v1.2.0
./configure --prefix $QTTARGETDIR -qmake-exec $QTARGETDIR/bin/qmake | tee qtmobility.configure.log
make -j $nprocs | tee qtmobility.build.log
sudo make install | tee qtmobility.install.log
cd ..

#----- QT Scripting
git clone git://gitorious.org/qt-labs/qtscriptgenerator.git
cd qtscriptgenerator/generator
$QTDIR/bin/qmake && make -j $nprocs
./generator
cd ../qtbindings
qmake && make -j $nprocs
sudo cp -Rv ../plugins/script/* $QTDIR/plugins/script

#----- Done!
echo "***"
echo "Builddir in $QTBUILDDIR not removed. Delete the directory manually if not needed"
echo "QT Installed in $QTTARGETDIR"
echo "Done."

