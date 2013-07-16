#!/bin/bash

parse_args()
{
  build=0; install=0; rebuild=0; clean=0; has_opt=0;
  until [ -z "$1" ]
  do
    arg=$1
    echo "Processing arg: [$arg]"
    n=`expr index "$arg" -`
    if [ $n -eq '1' ]; then
      #n=1
      len=`expr length "$arg"`
      #echo "n=$n, len=$len"
      while [ $n -lt $len ] 
      do
        n=`expr $n + 1` 
        opt=`expr substr "$arg" $n 1` 
        case "$opt" in
          b) build=1; has_opt=1;;
          i) install=1; has_opt=1;;
          c) clean=1; has_opt=1;;
          r) rebuild=1; has_opt=1;;
          a) build=1; install=1; rebuild=1; has_opt=1;;
          *) echo $opt=unknown;;
        esac
      done
    else
      case "$arg" in
        pc) BUILD_OPT=PC_Linux;;
        Linux) BUILD_OPT=Berlin_Linux;;
        Android) BUILD_OPT=Berlin_Android;;
        anchovy) BUILD_OPT=Berlin_anchovy;;
        *) echo -n "Unknown arg [$arg]"; echo; exit;;
      esac
    fi
    shift
  done
}

parse_args $*

# set default options if they are not specified in arguments
if [ "$BUILD_OPT" == "" ]; then BUILD_OPT=Berlin_anchovy; fi
if [ $has_opt -eq 0 ]; then build=1; install=1; has_opt=1; fi

set -o pipefail

echo build target \"${BUILD_OPT}\"
echo build=$build install=$install rebuild=$rebuild clean=$clean

QT_TOP="$( cd "$( dirname "$0" )" && pwd )"

if [ "$BUILD_OPT" == PC_Linux ]; then
echo \"PC_Linux\" is not supported now
exit	# disable this option temporarily

	export QT_SYS_ROOT=${GALOIS_LINUX_ROOT}
	export QT_TOOLCHAIN=${GALOIS_TOOLS_PREFIX}
	export xplatform_opt="-xplatform qws/linux-arm-marvell-gnueabi-g++"
	export OUTPUT_PREFIX=/home/galois/netflix/qt
	export keyboard_opt=""

#${QT_TOP}/configure -release -prefix /usr/local/qt/qws -embedded -qt-gfx-directfb -confirm-license -opensource -reduce-relocations -reduce-exports -no-opengl -no-svg -no-phonon -no-dbus -no-iconv -no-libtiff -no-qt3support -no-declarative -no-scripttools -no-xmlpatterns -no-multimedia -no-accessibility -nomake examples -nomake demos -nomake tools -qconfig netflix -depths 32 -openssl-linked -no-exceptions -no-script -qt-gif -no-libmng -no-decoration-windows -no-decoration-styled -no-gfx-linuxfb -no-gfx-multiscreen -no-mouse-pc -no-mouse-linuxtp -no-glib -no-sql-mysql -webkit -v 2>&1 | tee conf.output
#make 2>&1 | tee conf.output; sudo make install 2>&1 | tee -a make.output
#sudo cp ../fonts/*.ttf /usr/local/qt/qws/lib/fonts/
#exit

elif [ "$BUILD_OPT" == Berlin_Linux ]; then
echo \"Berlin_Linux\" is not supported now
exit	# disable this option temporarily

	#export GALOIS_LINUX_ROOT=~/rootfs
	# check required environment variables
	if [ "$GALOIS_LINUX_ROOT" == "" ]; then
		echo -e "\nError: environment variable \"GALOIS_LINUX_ROOT\" is not set.\n"
		exit 1
	elif [ "$GALOIS_TOOLS_PREFIX" == "" ]; then
		echo -e "\nError: environment variable \"GALOIS_TOOLS_PREFIX\" is not set.\n"
		exit 1
	fi

	export QT_SYS_ROOT=${GALOIS_LINUX_ROOT}
	export QT_TOOLCHAIN=${GALOIS_TOOLS_PREFIX}
	export xplatform_opt="-xplatform qws/linux-arm-marvell-gnueabi-g++"
	export OUTPUT_PREFIX=/home/galois/netflix/qt
	export keyboard_opt=""

elif [ "$BUILD_OPT" == Berlin_Android ]; then

	# check required environment variables
	if [ "$ANDROID_BUILD_TOP" == "" ]; then
		echo -e "\nError: environment variable \"ANDROID_BUILD_TOP\" is not set. Please run envsetup.sh in Honeycomb to setup.\n"
		exit 1
	elif [ "$GALOIS_TOOLS_PREFIX" == "" ]; then
		echo -e "\nError: environment variable \"GALOIS_TOOLS_PREFIX\" is not set. Please run envsetup.sh in Honeycomb to setup.\n"
		exit 1
	fi

	export QT_SYS_ROOT=${ANDROID_BUILD_TOP}
	export QT_TOOLCHAIN=${GALOIS_TOOLS_PREFIX}
	export xplatform_opt="-xplatform qws/linux-arm-none-gnueabi-g++"
	export OUTPUT_PREFIX=/system/netflix/qt
	export keyboard_opt="-plugin-kbd-softir"

elif [ "$BUILD_OPT" == Berlin_anchovy ]; then

	# anchovy root and compiler are hard coded here
	export QT_SYS_ROOT=~/anchovy/MRVL/MV88DE3100_SDK/Customization_Data/File_Systems/rootfs_gtb
	export QT_TOOLCHAIN=~/anchovy/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-

	export xplatform_opt="-xplatform qws/linux-arm-gnueabi-g++"
	export OUTPUT_PREFIX=/home/galois/netflix/qt
	export keyboard_opt=""
fi

if [ $rebuild == 1 -a -e conf.output ]; then
	echo delete all files in build directory for rebuilding
	ls -a | xargs rm -rf > NULL
fi

if [ ! -e conf.output ] ; then
	echo Run configure
	${QT_TOP}/configure -platform qws/linux-x86-g++ ${xplatform_opt} ${keyboard_opt} \
	-prefix ${OUTPUT_PREFIX} -release -embedded arm -opensource -no-sse -no-sse2 \
	-openssl-linked -reduce-exports -reduce-relocations -qt-gfx-directfb -no-dbus -depths 32 -webkit -qt-gif -plugin-gfx-directfb -plugin-gfx-qvfb \
	-system-libpng -system-libjpeg -system-freetype -system-zlib \
	-no-3dnow -no-accessibility -no-cups -no-declarative -no-exceptions -no-glib -no-javascript-jit -no-iconv -no-libmng -no-libtiff -no-mmx -no-multimedia -no-nis -no-phonon -no-phonon-backend -no-qt3support -no-script -no-scripttools -no-sql-postgres -no-sql-mysql -no-sql-oracle -no-svg -no-xmlpatterns -nomake demos -nomake docs -nomake examples -nomake tools -nomake translations \
	-confirm-license -no-opengl \
	-qconfig netflix -no-sql-odbc -no-sql-tds -no-sql-sqlite2 \
	-DENABLE_3D_CANVAS=0 -DENABLE_BLOB_SLICE=0 -DENABLE_CLIENT_BASED_GEOLOCATION=0 -DENABLE_DATAGRID=0 -DENABLE_DATALIST=0 -DENABLE_FILTERS=0 -DENABLE_GEOLOCATION=0 -DENABLE_ICONDATABASE=0 -DENABLE_JAVASCRIPT_DEBUGGER=0 -DENABLE_MATHML=0 -DENABLE_NOTIFICATIONS=0 -DENABLE_OFFLINE_WEB_APPLICATIONS=0 -DENABLE_RUBY=0 -DENABLE_SANDBOX=0 -DENABLE_SHARED_WORKERS=0 -DENABLE_SVG=0 -DENABLE_SVG_ANIMATION=0 -DENABLE_SVG_AS_IMAGE=0 -DENABLE_SVG_DOM_OBJC_BINDINGS=0 -DENABLE_SVG_FONTS=0 -DENABLE_SVG_FOREIGN_OBJECT=0 -DENABLE_SVG_FOREIGN_USE=0 -DENABLE_SVG_TILED_BACKING_STORE=0 -DENABLE_VIDEO=0 -DENABLE_WEB_SOCKETS=0 -DENABLE_WML=0 -DENABLE_XHTMLMP=0 -DENABLE_WCSS=0 -DENABLE_WORKERS=0 -DENABLE_FTPDIR=0 -DENABLE_DRAG_SUPPORT=0 -DENABLE_NETSCAPE_PLUGIN_API=0 -DENABLE_DASHBOARD_SUPPORT=0 -DENABLE_PAN_SCROLLING=0 -DENABLE_INSPECTOR=0 \
	-v 2>&1 | tee conf.output
	if [ $? != 0 ]; then exit; fi
	echo configure success
else
	echo Continue build. Do not need to run configure
fi

if [ $build == 1 ]; then
	make -j20 2>&1 | tee make.output
	if [ $? != 0 ]; then exit; fi
	echo build success
fi

if [ $install == 1 ]; then
	rm -rf out
	mkdir -p out out/package out/runtime
	make install INSTALL_ROOT="$( cd out/package && pwd )" 2>&1 | tee -a make.output
	if [ $? != 0 ]; then exit; fi
	#cp ../fonts/*.ttf out/package/${OUTPUT_PREFIX}/lib/fonts/
	find out -name .svn | xargs rm -rf
	cp -rf out/package/* out/runtime
	rm -rf out/runtime/${OUTPUT_PREFIX}/{bin,include,mkspecs,lib/*.la,lib/*.prl}
	cd out/runtime && tar czfv netflix_qt.tgz *
	echo install success
fi
