#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qprincipalaxisgenerator.h"
#include "MeshTranslator.h"

#include<string>
#include<QLabel>
#include<QToolButton>
#include<QVBoxLayout>
#include<QPushButton>
#include<QFileDialog>
#include<QProgressDialog>
#include<iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    signalMapper=NULL;
    computedMeshes=false;

    ui->tabWidgetLeft->setCurrentIndex(0);
    ui->tabWidgetRight->setCurrentIndex(0);

    //policy for window resizing
    QSizePolicy sizePol(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->splitter->setSizePolicy(sizePol);

    leftWidgets= new MeshViewerWidget*[MAX_VIEWERS];
    rightWidgets= new MeshViewerWidget*[MAX_VIEWERS];
    nViewers=0;

    //even viewers - left viewers
    //odd viewers - right viewers
    leftWidgets[nViewers]=ui->viewer_2;
    rightWidgets[nViewers]=ui->viewer;
    nViewers++;

    leftWidgets[nViewers]=ui->viewer_4;
    rightWidgets[nViewers]=ui->viewer_3;
    nViewers++;

    leftWidgets[nViewers]=ui->viewer_6;
    rightWidgets[nViewers]=ui->viewer_5;
    nViewers++;




    //mouse events propagation between viewers
    for( int i=0; i<nViewers; i++ ){

        for( int j=0;j<nViewers;j++ ){
            connect( leftWidgets[i],SIGNAL(wheelMoved(QWheelEvent*)),rightWidgets[j],SLOT(wheelMoveSlot(QWheelEvent*)) );
            connect( leftWidgets[i],SIGNAL(mouseMoved(QMouseEvent*)),rightWidgets[j],SLOT(mouseMoveSlot(QMouseEvent*)) );
            connect( leftWidgets[i],SIGNAL(mousePressed(QMouseEvent*)),rightWidgets[j],SLOT(mousePresSlot(QMouseEvent*)) );
            connect( leftWidgets[i],SIGNAL(mouseReleased(QMouseEvent*)),rightWidgets[j],SLOT(mouseReleaseSlot(QMouseEvent*)) );

            connect( rightWidgets[i],SIGNAL(wheelMoved(QWheelEvent*)),leftWidgets[j],SLOT(wheelMoveSlot(QWheelEvent*)) );
            connect( rightWidgets[i],SIGNAL(mouseMoved(QMouseEvent*)),leftWidgets[j],SLOT(mouseMoveSlot(QMouseEvent*)) );
            connect( rightWidgets[i],SIGNAL(mousePressed(QMouseEvent*)),leftWidgets[j],SLOT(mousePresSlot(QMouseEvent*)) );
            connect( rightWidgets[i],SIGNAL(mouseReleased(QMouseEvent*)),leftWidgets[j],SLOT(mouseReleaseSlot(QMouseEvent*)) );
        }
    }



    //redirecting standard, error and log output to text box in the application
    ui->outputText->setReadOnly(true);
    qout=new QDebugStream(std::cout, ui->outputText);
    qlog=new QDebugStream(std::clog, ui->outputText);
    qerr=new QDebugStream(std::cerr, ui->outputText);


    ////Just for testing has to be commented
    /*
    ui->viewer->open_mesh_gui("../../../tetra.off");
    ui->viewer->insertAxis( 0, Axis3D(std::pair<Vec3f,Vec3f>( OpenMesh::Vec3f(0,0,0),OpenMesh::Vec3f(10,0,0) ),
                                      std::pair<Vec3f,Vec3f>( OpenMesh::Vec3f(0,0,0),OpenMesh::Vec3f(0,0,10) )
                                      ) );


    ui->viewer->open_mesh_gui("/media/dades/vibot/se/project/shrec_training_corr/0010.null.0.off");

    ui->viewer->open_mesh_gui("/media/dades/vibot/se/project/shrec_training_corr/0002.noise.4.off");

    ui->viewer->open_mesh_gui("/media/dades/vibot/se/project/shrec_training_corr/0002.partial.3.off");


    ui->viewer_2->open_mesh_gui("../../../tetra.off");
    ui->viewer_2->insertAxis( 0, Axis3D(std::pair<Vec3f,Vec3f>( OpenMesh::Vec3f(0,0,0),OpenMesh::Vec3f(10,0,0) ),
                                        std::pair<Vec3f,Vec3f>( OpenMesh::Vec3f(0,0,0),OpenMesh::Vec3f(0,0,10) )
                                        ) );

    ui->viewer_2->open_mesh_gui("/media/dades/vibot/se/project/shrec_training_corr/0010.null.0.off");

    ui->viewer_2->open_mesh_gui("/media/dades/vibot/se/project/shrec_training_corr/0002.noise.4.off");

    ui->viewer_2->open_mesh_gui("/media/dades/vibot/se/project/shrec_training_corr/0002.partial.3.off");
*/
    ////

    //needed after every modif of the number of meshes
    updateMenu();

}

void MainWindow::updateMenu(){
    //as all the meshes are the same in all viewers we
    //take one of them to create the menu

    //disconneciton of previous signals
    delete signalMapper;
    disconnect(this,SIGNAL(toggleMeshVisible(int)),this,SLOT( setMeshvisible(int) ));

    QAction *action;
    //using signal mapper to match non parameter signals with parameter slots
    signalMapper = new QSignalMapper(this);

    QVBoxLayout *layout=new QVBoxLayout();
    QWidget *wid=new QWidget;
    wid->setLayout(layout);
    ui->scrollArea->setWidget(wid);

    QHBoxLayout * titleLayout=new QHBoxLayout();
    QWidget *title=new QWidget;
    title->setLayout(titleLayout);

    QLabel *labeltext=new QLabel("Mesh list");
    //labeltext->setMinimumWidth(100);
    titleLayout->addWidget(labeltext);
    titleLayout->addWidget(new QLabel("Visible"));


    layout->addWidget(title);



    const float * color;
    for( int i=0;i<ui->viewer->getNumberOfMeshes();i++ ){
        color=ui->viewer->getMeshColor(i);
        QPixmap *pixmap=new QPixmap(QSize(22,22));

        pixmap->fill(QColor(color[0]*255,color[1]*255,color[2]*255));

        //action=ui->menuMesh->addAction( QString("") );
        action=new QAction(this);
        action->setCheckable(true);
        if(i<ui->viewer->getNumberOfMeshes()){
            action->setChecked( ui->viewer->isMeshVisible(i) );
        }else{
            action->setChecked(true);
        }

        QToolButton *tool=new QToolButton();

        tool->setDefaultAction(action);


        //layout->addWidget(tool);
        QHBoxLayout * hlayout=new QHBoxLayout;
        QLabel *pixlabel=new QLabel();
        pixlabel->setPixmap(*pixmap );
        hlayout->addWidget( pixlabel );

        QLabel *label=new QLabel(QString("Mesh id:  %1").arg(i));

        hlayout->addWidget(label);
        hlayout->addWidget( tool );
        layout->addLayout( hlayout );
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(action, i );

        //ui->listWidget->addItem(QString("Mesh id:  %1").arg(i));

        delete pixmap;
    }

    connect(signalMapper, SIGNAL(mapped(int)),
            this, SIGNAL( toggleMeshVisible(int) ));
    connect(this,SIGNAL(toggleMeshVisible(int)),this,SLOT( setMeshvisible(int) ));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete qout;
    delete qerr;
    delete qlog;
    delete [] leftWidgets;
    delete [] rightWidgets;
}

void MainWindow::on_axisButton_clicked()
{
    //first two meshes always have data, others only when axis are computed
    ui->viewer->switchAxis();
    ui->viewer_2->switchAxis();
    if(computedMeshes){
        for( int i=1; i<nViewers; i++ ){
            leftWidgets[i]->switchAxis();
            rightWidgets[i]->switchAxis();
        }
    }
}

void MainWindow::setMeshvisible(int index){
    //first two meshes always have data, others only when axis are computed
    ui->viewer->setMeshVisible(index);
    ui->viewer_2->setMeshVisible(index);

    if(computedMeshes){
        for( int i=1; i<nViewers; i++ ){
            leftWidgets[i]->setMeshVisible(index);
            rightWidgets[i]->setMeshVisible(index);
        }
    }

}

void MainWindow::on_openButton_clicked()
{

    QFileDialog dialog(this);
    dialog.setDirectory(".");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(trUtf8("Off files (*.off);;All files (*.*)"));

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    QStringList::const_iterator constIterator;
    for (constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd();
         ++constIterator){

        ui->viewer->open_mesh_gui( *constIterator );
        ui->viewer_2->open_mesh_gui( *constIterator );
    }

    updateMenu();

}

void MainWindow::on_resetPositionButton_clicked()
{
    //first two meshes always have data, others only when axis are computed
    ui->viewer->resetScenePos();
    ui->viewer_2->resetScenePos();
    if(computedMeshes){
        for( int i=1; i<nViewers; i++ ){
            leftWidgets[i]->resetScenePos();
            rightWidgets[i]->resetScenePos();
        }
    }
}

void MainWindow::on_computeButton_clicked()
{
    //disabling actions that could make the application crash
    ui->openButton->setEnabled(false);
    ui->computeButton->setEnabled(false);

    QPrincipalAxisGenerator generator;

    if( computedMeshes ){
        //cleaning previous calculation
        for( int i=1;i<nViewers; i++ ){
            leftWidgets[i]->deleteMeshes();
            rightWidgets[i]->deleteMeshes();
        }
    }

    vector<Method> methods2compute;
    //add them in the order of the tabs
    methods2compute.push_back( JUST_PCA );
    methods2compute.push_back( ROBUS_DETERMINATION );

    int progressIt=0;

    QProgressDialog progress;
    progress.setWindowTitle("Loading");
    progress.setLabelText("Computing meshes...");
    progress.setCancelButton(NULL);
    progress.setMinimum(0);
    progress.setMaximum(100);
    progress.show();

    for( int i=0; i< leftWidgets[0]->getNumberOfMeshes(); i++ ){

        generator.setMesh( leftWidgets[0]->getMesh(i) );

        for( int j=0; j<methods2compute.size(); j++ ){

            generator.setMethod( methods2compute.at(j) );//selecting method to generate axis

            generator.start();//computing on other thread not to freeze the main window

            while(generator.isRunning()){
                //forcing window refresh events so the window not freezes during the wait
                qApp->processEvents();
                update();
                repaint();
                ui->centralWidget->repaint();
            }
            //tab position, 0 is the original tab
            leftWidgets[j+1]->insert_mesh( leftWidgets[0]->getMesh(i),generator.getAxis() );
            rightWidgets[j+1]->insert_mesh( rightWidgets[0]->getMesh(i),generator.getAxis() );

            leftWidgets[j+1]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );
            rightWidgets[j+1]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );


            progressIt++;
            progress.setValue( progressIt*100/(leftWidgets[0]->getNumberOfMeshes()*2) );
            progress.update();
        }
        /*
         //stating way, just for debuging
        generator.setMethod( JUST_PCA );//selecting method to generate axis

        generator.start();//computing on other thread not to freeze the main window

        while(generator.isRunning()){
            //forcing window refresh events so the window not freezes during the wait
            qApp->processEvents();
            update();
            repaint();
            ui->centralWidget->repaint();
        }

        leftWidgets[1]->insert_mesh( leftWidgets[0]->getMesh(i),generator.getAxis() );
        rightWidgets[1]->insert_mesh( rightWidgets[0]->getMesh(i),generator.getAxis() );

        leftWidgets[1]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );
        rightWidgets[1]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );


        progressIt++;
        progress.setValue( progressIt*100/(leftWidgets[0]->getNumberOfMeshes()*2) );
        progress.update();
        //sleep(2);

        generator.setMethod( ROBUS_DETERMINATION );
        generator.start();

        while(generator.isRunning()){
            //forcing window refresh events so the window not freezes during the wait
            qApp->processEvents();
            update();
            repaint();
            ui->centralWidget->repaint();
        }


        leftWidgets[2]->insert_mesh( leftWidgets[0]->getMesh(i),generator.getAxis() );
        rightWidgets[2]->insert_mesh( rightWidgets[0]->getMesh(i),generator.getAxis() );


        leftWidgets[2]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );
        rightWidgets[2]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );

        progressIt++;
        progress.setValue( progressIt*100/(leftWidgets[0]->getNumberOfMeshes()*2) );
        progress.update();
        */
        //sleep(2);
    }

    progress.setValue(100);
    progress.close();

    //apply translation and rotation

    cout <<"Computing axis completed"<<endl;
    computedMeshes=true;
    ui->openButton->setEnabled(true);
    ui->computeButton->setEnabled(true);
    ui->alignButton->setEnabled(true);
}



void MainWindow::on_clearButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("All meshes are going to be deleted from the viewer. Do you want to proceed?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    if( ret== QMessageBox::Ok ){
        for( int i=0; i< nViewers; i++ ){
            leftWidgets[i]->deleteMeshes();
            rightWidgets[i]->deleteMeshes();
        }

    }
    updateMenu();
}

void MainWindow::on_alignButton_clicked()
{
    ui->alignButton->setEnabled(false);

    vector<Axis3D> axis[nViewers];

    if( computedMeshes ){
        MeshTranslator translator;

        for( int i=1;i<nViewers; i++ ){
            axis[i]=leftWidgets[i]->getAxisVector();
            leftWidgets[i]->deleteMeshes();
            rightWidgets[i]->deleteMeshes();
        }
        //aligning all meshes refeering to the first mesh
        for( int i=0; i< leftWidgets[0]->getNumberOfMeshes(); i++ ){

            //PCA axis viewers
            if( i==0 ){//first index has not to be rotated as it is the reference

                    leftWidgets[1]->insert_mesh( leftWidgets[0]->getMesh(i),axis[1].at(i) );
                rightWidgets[1]->insert_mesh( rightWidgets[0]->getMesh(i),axis[1].at(i) );
            }else{
                leftWidgets[1]->insert_mesh( translator.translateMesh( leftWidgets[0]->getMesh(i),axis[1].at(i) ,leftWidgets[1]->getAxis(0) ) ,
                                             leftWidgets[1]->getAxis(0) );

                rightWidgets[1]->insert_mesh( translator.translateMesh( leftWidgets[0]->getMesh(i),axis[1].at(i) ,rightWidgets[1]->getAxis(0) ),
                                              rightWidgets[1]->getAxis(0) );

            }
            leftWidgets[1]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );
            rightWidgets[1]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );

            //Robust axis viewers
            if( i==0 ){//first index has not to be rotated as it is the reference
                leftWidgets[2]->insert_mesh( leftWidgets[0]->getMesh(i),axis[2].at(i)  );
                rightWidgets[2]->insert_mesh( rightWidgets[0]->getMesh(i),axis[2].at(i)  );

            }else{


                leftWidgets[2]->insert_mesh( translator.translateMesh( leftWidgets[0]->getMesh(i),axis[2].at(i) ,leftWidgets[2]->getAxis(0) ) ,
                                             leftWidgets[2]->getAxis(0)  );

                rightWidgets[2]->insert_mesh( translator.translateMesh( leftWidgets[0]->getMesh(i),axis[2].at(i) ,rightWidgets[2]->getAxis(0) ),
                                              leftWidgets[2]->getAxis(0)  );
            }

            leftWidgets[2]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );
            rightWidgets[2]->setMeshVisiblility( i,leftWidgets[0]->isMeshVisible(i) );
        }


    }
}
