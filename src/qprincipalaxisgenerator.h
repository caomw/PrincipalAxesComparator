#ifndef QPRINCIPALAXISGENERATOR_H
#define QPRINCIPALAXISGENERATOR_H
#include <QThread>
#include "PrincipalAxisGenerator.h"

class QPrincipalAxisGenerator : public QThread, public PrincipalAxisGenerator
{
    Q_OBJECT
public:
    QPrincipalAxisGenerator();
    void run ();
};

#endif // QPRINCIPALAXISGENERATOR_H
