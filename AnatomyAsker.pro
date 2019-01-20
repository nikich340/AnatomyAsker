TEMPLATE     = app
HEADERS	     = AnatomyAsker.h GraphicsView.h
SOURCES	     = AnatomyAsker.cpp GraphicsView.cpp main.cpp
QT          += widgets xml multimedia
RESOURCES   += commonRes.qrc \
               osteoRes1.qrc osteoRes2.qrc osteoRes3.qrc osteoRes4.qrc osteoRes5.qrc osteoRes6.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
windows:TARGET = ../AnatomyAsker
windows:RC_FILE = icon.rc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

