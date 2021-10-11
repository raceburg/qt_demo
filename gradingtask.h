#ifndef GRADINGTASK_H
#define GRADINGTASK_H
#include <QObject>
#include <QRunnable>
#include <QList>
#include <QThreadPool>
#include <functional>

// --------------------------------
// класс для создания потока
// --------------------------------

class GradingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    GradingTask(std::function<void (QObject *, QObject*)> taskFunction,
                QObject *taskInputArgs,
                QObject *taskOutput)
    {
        fn = taskFunction;
        args = taskInputArgs;
        results = taskOutput;
    }

    void run() override{
        try {
            fn(args, results);
        } catch (...) {
            emit error("Error in task");
        }
        if (results != nullptr)
            emit result(results);
        emit finished();
    }

signals:
//    end task
    void finished();
//    error task
    void error(QString);
//    result of task
    void result(QObject*);


private:
    std::function<void (QObject *, QObject*)> fn;
    QObject* args;
    QObject* results;
};



#endif // GRADINGTASK_H
