TEMPLATE     = app
HEADERS	     = SimpleAsker.h
SOURCES	     = SimpleAsker.cpp main.cpp
QT          += widgets multimedia
RESOURCES   += resources/commonRes.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
windows:TARGET = ../SimpleAsker
windows:RC_FILE = resources/common/icon.rc

DISTFILES += \
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

