set qtroot=//ppd/ps3/trunk/3rdParty/qt

p4 edit %qtroot%/bin/*.exe
p4 edit %qtroot%/include/Qt/qconfig.h
p4 edit %qtroot%/lib/*.a
p4 edit %qtroot%/plugins/bearer/*.a
p4 edit %qtroot%/plugins/imageformats/*.a
p4 edit %qtroot%/src/3rdparty/webkit/JavaScriptCore/release/*.a
p4 edit %qtroot%/src/3rdparty/webkit/WebCore/generated/*
p4 edit %qtroot%/src/corelib/global/qconfig.h
p4 edit %qtroot%/src/tools/uic/qclass_lib_map.h

pause