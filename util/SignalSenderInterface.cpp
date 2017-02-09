#include "SignalSenderInterface.h"

SignalSenderInterface &SignalSenderInterface::instance()
{
    static SignalSenderInterface base;
    return base;
}

SignalSenderInterface::SignalSenderInterface(QObject *parent) : QObject(parent)
{
}
