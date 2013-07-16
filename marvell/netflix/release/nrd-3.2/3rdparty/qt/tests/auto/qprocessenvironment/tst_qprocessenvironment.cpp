/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest>
#include <QObject>
#include <QProcessEnvironment>

// Note:
// in cross-platform tests, ALWAYS use UPPERCASE variable names
// That's because on Windows, the variables are uppercased

class tst_QProcessEnvironment: public QObject
{
    Q_OBJECT
private slots:
    void operator_eq();
    void clearAndIsEmpty();
    void insert();
    void emptyNull();
    void toStringList();

    void caseSensitivity();
    void systemEnvironment();
    void putenv();
};

void tst_QProcessEnvironment::operator_eq()
{
    QProcessEnvironment e1;
    QVERIFY(e1 == e1);
    e1.clear();
    QVERIFY(e1 == e1);

    e1 = QProcessEnvironment();
    QProcessEnvironment e2;
    QVERIFY(e1 == e2);

    e1.clear();
    QVERIFY(e1 != e2);

    e2.clear();

    QVERIFY(e1 == e2);
}

void tst_QProcessEnvironment::clearAndIsEmpty()
{
    QProcessEnvironment e;
    e.insert("FOO", "bar");
    QVERIFY(!e.isEmpty());
    e.clear();
    QVERIFY(e.isEmpty());
}

void tst_QProcessEnvironment::insert()
{
    QProcessEnvironment e;
    e.insert("FOO", "bar");
    QVERIFY(!e.isEmpty());
    QVERIFY(e.contains("FOO"));
    QCOMPARE(e.value("FOO"), QString("bar"));

    e.remove("FOO");
    QVERIFY(!e.contains("FOO"));
    QVERIFY(e.value("FOO").isNull());

    e.clear();
    QVERIFY(!e.contains("FOO"));
}

void tst_QProcessEnvironment::emptyNull()
{
    QProcessEnvironment e;

    e.insert("FOO", "");
    QVERIFY(e.contains("FOO"));
    QVERIFY(e.value("FOO").isEmpty());
    QVERIFY(!e.value("FOO").isNull());

    e.insert("FOO", QString());
    QVERIFY(e.contains("FOO"));
    QVERIFY(e.value("FOO").isEmpty());
    // don't test if it's NULL, since we shall not make a guarantee

    e.remove("FOO");
    QVERIFY(!e.contains("FOO"));
}

void tst_QProcessEnvironment::toStringList()
{
    QProcessEnvironment e;
    QVERIFY(e.isEmpty());
    QVERIFY(e.toStringList().isEmpty());

    e.insert("FOO", "bar");
    QStringList result = e.toStringList();
    QVERIFY(!result.isEmpty());
    QCOMPARE(result.length(), 1);
    QCOMPARE(result.at(0), QString("FOO=bar"));

    e.clear();
    e.insert("BAZ", "");
    result = e.toStringList();
    QCOMPARE(result.at(0), QString("BAZ="));

    e.insert("FOO", "bar");
    e.insert("A", "bc");
    e.insert("HELLO", "World");
    result = e.toStringList();
    QCOMPARE(result.length(), 4);

    // order is not specified, so use contains()
    QVERIFY(result.contains("FOO=bar"));
    QVERIFY(result.contains("BAZ="));
    QVERIFY(result.contains("A=bc"));
    QVERIFY(result.contains("HELLO=World"));
}

void tst_QProcessEnvironment::caseSensitivity()
{
    QProcessEnvironment e;
    e.insert("foo", "bar");

#ifdef Q_OS_WIN
    // on Windows, it's uppercased
    QVERIFY(e.contains("foo"));
    QVERIFY(e.contains("FOO"));
    QVERIFY(e.contains("FoO"));

    QCOMPARE(e.value("foo"), QString("bar"));
    QCOMPARE(e.value("FOO"), QString("bar"));
    QCOMPARE(e.value("FoO"), QString("bar"));

    QStringList list = e.toStringList();
    QCOMPARE(list.at(0), QString("FOO=bar"));
#else
    // otherwise, it's case sensitive
    QVERIFY(e.contains("foo"));
    QVERIFY(!e.contains("FOO"));

    e.insert("FOO", "baz");
    QVERIFY(e.contains("FOO"));
    QCOMPARE(e.value("FOO"), QString("baz"));
    QCOMPARE(e.value("foo"), QString("bar"));

    QStringList list = e.toStringList();
    QVERIFY(list.contains("foo=bar"));
    QVERIFY(list.contains("FOO=baz"));
#endif
}

void tst_QProcessEnvironment::systemEnvironment()
{
    static const char envname[] = "THIS_ENVIRONMENT_VARIABLE_HOPEFULLY_DOESNT_EXIST";
    QByteArray path = qgetenv("PATH");
    QByteArray nonexistant = qgetenv(envname);
    QProcessEnvironment system = QProcessEnvironment::systemEnvironment();

    QVERIFY(nonexistant.isNull());

#ifdef Q_WS_WINCE
    // Windows CE has no environment
    QVERIFY(path.isEmpty());
    QVERIFY(!system.contains("PATH"));
    QVERIFY(system.isEmpty());
#else
    // all other system have environments
    if (path.isEmpty())
        QFAIL("Could not find the PATH environment variable -- please correct the test environment");

    QVERIFY(system.contains("PATH"));
    QCOMPARE(system.value("PATH"), QString::fromLocal8Bit(path));

    QVERIFY(!system.contains(envname));

# ifdef Q_OS_WIN
    // check case-insensitive too
    QVERIFY(system.contains("path"));
    QCOMPARE(system.value("path"), QString::fromLocal8Bit(path));

    QVERIFY(!system.contains(QString(envname).toLower()));
# endif
#endif
}

void tst_QProcessEnvironment::putenv()
{
#ifdef Q_WS_WINCE
    QSKIP("Windows CE has no environment", SkipAll);
#else
    static const char envname[] = "WE_RE_SETTING_THIS_ENVIRONMENT_VARIABLE";
    static bool testRan = false;

    if (testRan)
        QFAIL("You cannot run this test more than once, since we modify the environment");
    testRan = true;

    QByteArray valBefore = qgetenv(envname);
    if (!valBefore.isNull())
        QFAIL("The environment variable we set in the environment is already set! -- please correct the test environment");
    QProcessEnvironment eBefore = QProcessEnvironment::systemEnvironment();

    qputenv(envname, "Hello, World");
    QByteArray valAfter = qgetenv(envname);
    if (valAfter != "Hello, World")
        QSKIP("Could not test: qputenv did not do its job", SkipAll);

    QProcessEnvironment eAfter = QProcessEnvironment::systemEnvironment();

    QVERIFY(!eBefore.contains(envname));
    QVERIFY(eAfter.contains(envname));
    QCOMPARE(eAfter.value(envname), QString("Hello, World"));

# ifdef Q_OS_WIN
    // check case-insensitive too
    QString lower = envname;
    lower = lower.toLower();
    QVERIFY(!eBefore.contains(lower));
    QVERIFY(eAfter.contains(lower));
    QCOMPARE(eAfter.value(lower), QString("Hello, World"));
# endif
#endif
}

QTEST_MAIN(tst_QProcessEnvironment)

#include "tst_qprocessenvironment.moc"
