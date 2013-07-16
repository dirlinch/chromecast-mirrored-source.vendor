/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qkbdsoftir.h"
#include <stdio.h>
#ifndef QT_NO_QWS_KEYBOARD

#include <QSocketNotifier>
#include <QMetaEnum>
#include <QGraphicsView>

#include <qplatformdefs.h>
#include <private/qcore_unix_p.h> // overrides QTQWSSoftIRKbPrivate_OPEN

#include "NRD_IRReceiver.h"



QT_BEGIN_NAMESPACE

class QWSSoftIRKbPrivate : public QObject, public IRReceiverCallback
{
    Q_OBJECT
    Q_PROPERTY(Qt::Key key_enum);

public:
    QWSSoftIRKbPrivate(QWSSoftKeyboardHandler *, const QString &);
    ~QWSSoftIRKbPrivate();
    static int KeyToKeyCode(QString keyName);
    virtual void processIREvent(const int nKeyCode, const int unicode, const unsigned char modifier, bool checkFroDrop, void * pContext);

    QWSSoftKeyboardHandler    *m_handler;
    static Qt::Key key_enum;

private Q_SLOTS:
    void readKeycode();

private:
    IRReceiver*  pIrReceiver;
    int  lastKeyCode;
    int lastUnicode;
    unsigned int lastModifiers;
    int  repeatRate;
    struct timeval lastTimestamp;
};

QWSSoftKeyboardHandler::QWSSoftKeyboardHandler(const QString &device)
    : QWSKeyboardHandler(device)
{
    d = new QWSSoftIRKbPrivate(this, device);
}

QWSSoftKeyboardHandler::~QWSSoftKeyboardHandler()
{
    delete d;
}

int QWSSoftKeyboardHandler::KeyToKeyCode(QString keyName)
{
    return QWSSoftIRKbPrivate::KeyToKeyCode(keyName);
}

bool QWSSoftKeyboardHandler::filterInputEvent(quint16 &, qint32 &)
{
    return false;
}

QWSSoftIRKbPrivate::QWSSoftIRKbPrivate(QWSSoftKeyboardHandler *h, const QString &device)
    : m_handler(h)
{
    Q_UNUSED(device);

    lastKeyCode = 0;
    lastUnicode = -1;
    lastModifiers = Qt::NoModifier;
    lastTimestamp.tv_sec = 0;
    lastTimestamp.tv_usec = 0;

    const QString dfbKeyboard = QString::fromLatin1(qgetenv("QWS_KEYBOARD"));
    const QStringList dfbKbdArgs = dfbKeyboard.split(QLatin1Char(':'),
                                                      QString::SkipEmptyParts);

    QRegExp repeatRateRegExp(QLatin1String("repeatRate=(.+)"));
    repeatRateRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    if (dfbKbdArgs.indexOf(repeatRateRegExp) != -1) {
        repeatRate = repeatRateRegExp.cap(1).toInt();
    }
    else {
        repeatRate = 150;
    }

    setObjectName(QLatin1String("Soft IR Keyboard Handler"));

    pIrReceiver = new IRReceiver;
    if (pIrReceiver != NULL) 
    {
        pIrReceiver->Init(this, this);
        int fd = pIrReceiver->GetFd();

        if (fd != -1)
        {
            QSocketNotifier *notifier;
            notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
            connect(notifier, SIGNAL(activated(int)), this, SLOT(readKeycode()));
        }
    }
}

QWSSoftIRKbPrivate::~QWSSoftIRKbPrivate()
{
    if (pIrReceiver != NULL)
    {
        pIrReceiver->Deinit();
        pIrReceiver = NULL;
    }
}

int QWSSoftIRKbPrivate::KeyToKeyCode(QString keyName)
{
    const QMetaObject &mo = QWSSoftIRKbPrivate::staticMetaObject;
    int prop_index = mo.indexOfProperty("key_enum");
    QMetaProperty metaProperty = mo.property(prop_index);
    QMetaEnum metaEnum = metaProperty.enumerator();
    QString str = "Key_" + keyName;
    int key = metaEnum.keyToValue(str.toAscii().constData());
    return key;
}

void QWSSoftIRKbPrivate::readKeycode()
{
    pIrReceiver->ReadEvent();
}

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract (struct timeval* result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }

  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

#define NRD_MODIFIER_SHIFT    (0x1)
#define NRD_MODIFIER_CTRL     (0x2)
#define NRD_MODIFIER_ALT      (0x4)
#define NRD_MODIFIER_KEYPAD   (0x8)

void QWSSoftIRKbPrivate::processIREvent(const int nKeyCode, const int unicode, const unsigned char modifier, bool checkForDrop, void *pContext)
{
    unsigned int mod = Qt::NoModifier;
    if(modifier & NRD_MODIFIER_SHIFT)
    {
        mod |= Qt::ShiftModifier;
    }
    if(modifier & NRD_MODIFIER_CTRL)
    {
        mod |= Qt::ControlModifier;
    }
    if(modifier & NRD_MODIFIER_ALT)
    {
        mod |= Qt::AltModifier;
    }
    if(modifier & NRD_MODIFIER_KEYPAD)
    {
        mod |= Qt::KeypadModifier;
    }
  
    if (nKeyCode == Qt::Key_F13)
    {
        kill(getpid(), SIGUSR1);
    }
    else if (nKeyCode == Qt::Key_F14)
    {
        kill(getpid(), SIGUSR2);
    }
    else if (nKeyCode > 0)
    {
        bool autoRepeat = false;
//        if (nKeyCode == lastKeyCode && mod == lastModifiers && unicode == lastUnicode) {
//            autoRepeat = true;
//        } else {
            lastKeyCode = nKeyCode;
            lastModifiers = mod;
            lastUnicode = unicode;
            gettimeofday(&lastTimestamp, NULL);
//        }

        bool dropEvent = false;
        if (autoRepeat) {
            struct timeval inputTimestamp;
            gettimeofday(&inputTimestamp, NULL);

            struct timeval diff;
            timeval_subtract(&diff, &inputTimestamp, &lastTimestamp);
            if ((diff.tv_usec + diff.tv_sec*1000000) < repeatRate * 1000) {
                dropEvent = true;
            } else {
                lastTimestamp.tv_sec = inputTimestamp.tv_sec;
                lastTimestamp.tv_usec = inputTimestamp.tv_usec;
            }
        }        
        //fprintf(stderr, "SoftIR: sending unicode = 0x%x keycode = 0x%x modifier=0x%x\n", (int)unicode, (int)nKeyCode, (int)mod);
        if (!checkForDrop || (checkForDrop && !dropEvent))
        {
            QWSSoftIRKbPrivate* kbPrivate = (QWSSoftIRKbPrivate*) pContext;
            
            kbPrivate->m_handler->processKeyEvent(unicode, nKeyCode, (Qt::KeyboardModifiers)mod, true, false);
            usleep(10*1000);
            kbPrivate->m_handler->processKeyEvent(unicode, nKeyCode, (Qt::KeyboardModifiers)mod, false, false);
        }
    }
    else
    {
        qWarning("%s:%d Look up keycode %d failed", __FILE__, __LINE__, nKeyCode);
    }
}

QT_END_NAMESPACE

#include "qkbdsoftir.moc"

#endif // QT_NO_QWS_KEYBOARD            pIrReceiver = new IRReceiver;
