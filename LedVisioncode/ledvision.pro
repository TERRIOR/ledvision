#-------------------------------------------------
#
# Project created by QtCreator 2018-05-28T15:08:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ledvision
TEMPLATE = app
INCLUDEPATH +=./include

SOURCES += main.cpp\
        mainwindow.cpp \
    rule.cpp \
    mytime.cpp \
    cvgloble.cpp \
    qgloble.cpp \
    ledclassify.cpp \
    thresholdparam.cpp \
    signaldialog.cpp

HEADERS  += mainwindow.h \
    rule.h \
    mytime.h \
    cvgloble.h \
    qgloble.h \
    ledclassify.h \
    thresholdparam.h \
    signaldialog.h

FORMS    += mainwindow.ui \
    signaldialog.ui

LIBS += -Lopencv_ffmpeg2412_64.dll
INCLUDEPATH += E:\opencv\opencv\build\include\opencv2
INCLUDEPATH += E:\opencv\opencv\build\include\opencv
INCLUDEPATH += E:\opencv\opencv\build\include
win32:CONFIG(debug, debug|release): {
LIBS += -LE:\opencv\opencv\build\x64\vc12\lib \
-lopencv_objdetect2412d\
-lopencv_ts2412d\
-lopencv_video2412d\
-lopencv_nonfree2412d\
-lopencv_ocl2412d\
-lopencv_photo2412d\
-lopencv_stitching2412d\
-lopencv_superres2412d\
-lopencv_videostab2412d\
-lopencv_calib3d2412d\
-lopencv_contrib2412d\
-lopencv_core2412d\
-lopencv_features2d2412d\
-lopencv_flann2412d\
-lopencv_gpu2412d\
-lopencv_highgui2412d\
-lopencv_imgproc2412d\
-lopencv_legacy2412d\
-lopencv_ml2412
} else:win32:CONFIG(release, debug|release): {
LIBS += -LE:\opencv\opencv\build\x64\vc12\lib \
-lopencv_objdetect2412 \
-lopencv_ts2412 \
-lopencv_video2412 \
-lopencv_nonfree2412 \
-lopencv_ocl2412 \
-lopencv_photo2412 \
-lopencv_stitching2412 \
-lopencv_superres2412 \
-lopencv_videostab2412 \
-lopencv_calib3d2412 \
-lopencv_contrib2412 \
-lopencv_core2412 \
-lopencv_features2d2412 \
-lopencv_flann2412 \
-lopencv_gpu2412 \
-lopencv_highgui2412 \
-lopencv_imgproc2412 \
-lopencv_legacy2412 \
-lopencv_ml2412


}

RESOURCES += \
    main.qrc \
    qss.qrc

DISTFILES += \
    main.ico \
    main.rc
