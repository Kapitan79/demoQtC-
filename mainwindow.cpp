
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /** Непосредственно график */
    addPlot(new QCustomPlot(this));

    /** Кнопки управления */
    addButton(new LabelButton(this),   ":/icon/icon/play.png", (this->width()/2 - buttonSideSize * 2),   ButtonAction::Play) ;
    addButton(new LabelButton(this),   ":/icon/icon/pause.png",(this->width()/2 - buttonSideSize / 2),   ButtonAction::Pause);
    addButton(new LabelButton(this),   ":/icon/icon/stop.png", (this->width()/2 + buttonSideSize),       ButtonAction::Stop) ;

    setFixedSize(800,600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addButton(LabelButton *nextLabel, const QString imgLink, const int xPosition, const ButtonAction action)
{
    const int yPosition   = this->height() - botBotMargin/2 -  buttonSideSize/2;
    QPixmap picPlay(imgLink);

    nextLabel->setScaledContents(true);
    nextLabel->setParent(this);
    nextLabel->setPixmap(picPlay);
    nextLabel->setGeometry(xPosition, yPosition, buttonSideSize, buttonSideSize);
    nextLabel->installEventFilter(nextLabel);

    switch(action)
    {
        case ButtonAction::Play:
    {
        nextLabel->setObjectName("play");
        nextLabel->setEnabled(true);
        break;
    }
    case ButtonAction::Pause:
    {
        nextLabel->setObjectName("pause");
        nextLabel->setEnabled(false);
        break;
    }
    case ButtonAction::Stop:
    {
        nextLabel->setObjectName("stop");
        nextLabel->setEnabled(false);
        break;
    }
    }
    connect(nextLabel, &LabelButton::buttonClicked, this, &MainWindow::processThread);
}

void MainWindow::addPlot(QCustomPlot *plot)
{
    custPlot = plot;
    plot->setGeometry(0,0,this->width(), this->height()-botBotMargin);
    plot->legend->setVisible(true);
    plot->legend->setFont(QFont("Helvetica", 9));
    QPen pen;
    QStringList lineNames;
    lineNames << "Случайные значения" ;
    plot->addGraph();
    pen.setColor(QColor(qSin(0*1+1.2)*80+80, qSin(0*0.3+0)*80+80, qSin(0*0.3+1.5)*80+80));
    plot->graph()->setPen(pen);
    plot->graph()->setName(lineNames.at(0-QCPGraph::lsNone));
    plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    plot->graph()->rescaleAxes(true);
    plot->yAxis->scaleRange(1.1, plot->yAxis->range().center());
    plot->xAxis->scaleRange(1.1, plot->xAxis->range().center());
    plot->xAxis->setTicks(false);
    plot->yAxis->setTicks(true);
    plot->xAxis->setTickLabels(false);
    plot->yAxis->setTickLabels(true);
    plot->axisRect()->setupFullAxesBox();
}

void MainWindow::addDotToPlot(int x, int y)
{
    if(x > maxScaleX){maxScaleX = x+1;}
    if(y > maxScaleY){maxScaleY = y+1;}
    xv.push_back(x);
    yv.push_back(y);
    custPlot->graph(0)->setData(xv, yv);
    custPlot->xAxis->setRange(0, maxScaleX);
    custPlot->yAxis->setRange(0, maxScaleY);
    custPlot->replot();
}

void MainWindow::processThread(LabelButton *currButton)
{
    currButton->setEnabled(false);
    if(currButton->objectName() == "play"){
        if (!genThread)
        {
            genThread = new GenerateThread(this);
            connect(genThread, &GenerateThread::randValue, this, &MainWindow::addDotToPlot);
            connect(genThread, &GenerateThread::finished, genThread, &QObject::deleteLater);
            genThread->start();
        }
        else
        {
            genThread->setThreadPause();

        }
        findChild<LabelButton*>("pause")->setEnabled(true);
        findChild<LabelButton*>("stop")->setEnabled(true);
    } else if(currButton->objectName() == "pause") {
        genThread->setThreadPause();

        findChild<LabelButton*>("play")->setEnabled(true);
        findChild<LabelButton*>("stop")->setEnabled(true);
    } else if(currButton->objectName() == "stop") {
        genThread->setThreadStop();        
        custPlot->graph(0)->data()->clear();
        xv.clear();
        yv.clear();
        custPlot->replot();
        findChild<LabelButton*>("play")->setEnabled(true);
        findChild<LabelButton*>("pause")->setEnabled(false);
        genThread->quit();
        genThread = nullptr;
    }

}

LabelButton::LabelButton(QWidget *parent, Qt::WindowFlags f):QLabel(parent, f)
{

}

bool LabelButton::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type())
    {
        case QEvent::MouseButtonPress:
    {
        if (!isPressed)
        {
            if(this->isEnabled())
            {
                isPressed = true;
                setGeometry(x()+3, y()+3, width()-3, height()-3);
            }
        }
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        if(isPressed)
        {
            if(this->isEnabled())
            {
                emit buttonClicked(this);
                isPressed = false;
                setGeometry(x()-3, y()-3, width()+3, height()+3);
            }
        }
        break;
    }
    case QEvent::Enter:
    {
        if(!isEntered)
        {
            isEntered = true;
            setGeometry(x(), y()-5, width(), height());
        }
        break;
    }
    case QEvent::Leave:
    {
        if(isEntered)
        {
            isEntered = false;
            setGeometry(x(), y()+5, width(), height());
        }
        break;
    }
    }
    return false;
}

int GenerateThread::randomBetween(int interval, int minValue)
{
    QRandomGenerator rand;
    int valueY= QRandomGenerator::global()->generate() % interval;
    int valueX = QRandomGenerator::global()->generate() % interval + minValue;
    emit randValue(valueX, valueY);
    return valueX;
}

GenerateThread::GenerateThread(QObject *parent):QThread(parent)
{

}
