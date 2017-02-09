#ifndef SIGNALSENDERINTERFACE_H
#define SIGNALSENDERINTERFACE_H

#include <QObject>

class SignalSenderInterface : public QObject
{
    Q_OBJECT
  public:
    static SignalSenderInterface &instance();

    explicit SignalSenderInterface(QObject *parent = 0);
    void print(QString text)
    {
        emit printsend(text);
    }
  signals:
    void printsend(QString);

  public slots:
};

#endif // SIGNALSENDERINTERFACE_H
