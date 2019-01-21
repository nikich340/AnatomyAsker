TEMPLATE     = app
HEADERS	     = AnatomyAsker.h GraphicsView.h
SOURCES	     = AnatomyAsker.cpp GraphicsView.cpp main.cpp
QT          += widgets xml multimedia
RESOURCES   += resources/commonRes.qrc \
               resources/osteoRes1.qrc resources/osteoRes2.qrc resources/osteoRes3.qrc \
               resources/osteoRes4.qrc resources/osteoRes5.qrc resources/osteoRes6.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
windows:TARGET = ../AnatomyAsker
windows:RC_FILE = resources/common/icon.rc

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

