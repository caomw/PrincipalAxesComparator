#include <QApplication>
#include "mainwindow.h"
#ifdef ARCH_DARWIN
#include <glut.h>
#else
#include <GL/glut.h>
#endif

int main(int argc, char *argv[])
{


    // OpenGL check
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a(argc, argv);
  #if !defined(__APPLE__)
    glutInit(&argc,argv);
  #endif

    if ( !QGLFormat::hasOpenGL() ) {
      QString msg = "System has no OpenGL support!";
      QMessageBox::critical( 0, QString("OpenGL"), msg + QString(argv[1]) );
      return -1;
    }


    MainWindow w;
    w.show();
    
    return a.exec();
}
