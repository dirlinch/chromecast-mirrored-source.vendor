EXTRA_FLAGS=`cat $DIR/qtconfig.opts`

BUILD_SMALL=yes
BUILD_SMALLWK=yes
REAL_OPTS=
for opt in $OPTS; do
  case $opt in
  -small) BUILD_SMALL=yes ;;
  -big) BUILD_SMALL=no ;;
  -smallwk) BUILD_SMALLWK=yes ;;
  -bigwk) BUILD_SMALLWK=no ;;
  *) [ -n "$REAL_OPTS" ] && REAL_OPTS="$REAL_OPTS "
     REAL_OPTS="$REAL_OPTS $opt"
     ;;
  esac
done
OPTS="$REAL_OPTS"
if [ "$BUILD_SMALL" = "yes" ]; then
    echo "Building SMALL!!" >&2
    EXTRA_FLAGS="${EXTRA_FLAGS} `cat $DIR/qtconfig_small.opts`"
fi
if [ "$BUILD_SMALLWK" = "yes" ]; then
    echo "Building SMALL (webkit)!!" >&2
    EXTRA_FLAGS="${EXTRA_FLAGS} `cat $DIR/qtconfig_smallwk.opts`"
fi
