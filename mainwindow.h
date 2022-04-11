#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"qcustomplot/qcustomplot.h"
#include <QMainWindow>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/** КЛАСС ПОТОКА С ГЕНЕРАТОРОМ ЧИСЕЛ */

class GenerateThread : public QThread
{
    Q_OBJECT

    bool isPause    = false ;
    bool isStop     = false ;

    int minX = 0;

    void run() override {
        while(!isStop)
        {
            minX = randomBetween(100, minX);
            sleep(1);
            while(isPause)
            {
                sleep(1);
            }
        }
    }

    /*!
     * \brief randomBetween - функция генерации числа между заданными занчениями (страховка от слишком большого разброса значений в графике)
     * \param low   - минимальное возможное случайное значение
     * \param high  - максимальное возможное случайное значение
     * \return
     */
    int randomBetween(int interval = 100, int minValue = 0);
public slots:
    void setThreadStop()
    {
        isStop = true;
    }

    void setThreadPause()
    {
        isPause = !isPause;
    }
signals:
    /*!
     * \brief randValue - сигнал, возвращающий случайные значения для графика
     * \param valueX
     * \param valuesY
     */
    void randValue(int valueX, int valuesY);
};

/** КЛАСС, РЕАЛИЗУЮЩИЙ ФУНКЦИОНАЛ КНОПОК */
class LabelButton: public QLabel
{
    Q_OBJECT

    /* Эти флаги нужны, так как при быстром многократном нажатии на кнопки события иногда задваиваются */
    bool isPressed = false;
    bool isEntered = false;

    bool eventFilter(QObject *obj, QEvent *event);

public:
    LabelButton(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());

signals:
    void buttonClicked(LabelButton *btn);
};


/** КЛАСС, РЕАЛИЗУЮЩИЙ ОСНОВНОЙ ФУНКЦИОНАЛ */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    GenerateThread *genThread {};
    QCustomPlot    *custPlot  {};
    QVector<double> xv{}, yv{};

    enum ButtonAction
    {
        Play,
        Pause,
        Stop
    };

    const int botBotMargin      = 100;
    const int buttonSideSize    = 50;

    int maxScaleX = 1;
    int maxScaleY = 1;

    /*!
     * \brief addButton - добавить кнопку на виджет
     * \param nextLabel - указатель на объект QLabel, который будет использован в качестве кнопки
     * \param imgLink   - ссылка на иконку кнопки из файла ресурсов
     * \param xPosition - позиция кнопки по оси Х
     * \param action    - действие, которое будет выполнять кнопка
     */
    void addButton(LabelButton *nextLabel, const QString imgLink, const int xPosition, const ButtonAction action);

    /*!
     * \brief addPlot   - добавить график на виджет
     * \param plot      - указатель на объект графика
     */
    void addPlot(QCustomPlot *plot);



private slots:
    /*!
     * \brief addDotToPlot - добавление новой точки на график
     * \param x
     * \param y
     */
    void addDotToPlot(int x, int y);

    /*!
     * \brief processThread - управление потоком генератора случайных координат
     * \param currButton - нажатая кнопка управления потоком
     */
    void processThread(LabelButton *currButton);

};
#endif // MAINWINDOW_H
