QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Constants.cpp \
    Lexico.cpp \
    Semantico.cpp \
    Sintatico.cpp \
    frmtabela.cpp \
    main.cpp \
    compiler.cpp

HEADERS += \
    AnalysisError.h \
    Constants.h \
    LexicalError.h \
    Lexico.h \
    SemanticError.h \
    Semantico.h \
    Sintatico.h \
    SyntaticError.h \
    Token.h \
    compiler.h \
    frmtabela.h

FORMS += \
    compiler.ui \
    frmtabela.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target