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


#include <QtTest/QtTest>

#include <qcombobox.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qtoolbar.h>


#include <qaction.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QActionGroup : public QObject
{
    Q_OBJECT

public:
    tst_QActionGroup();
    virtual ~tst_QActionGroup();

private slots:
    void enabledPropagation();
    void visiblePropagation();
    void dropDownDeleted();
    void exclusive();

    void separators();
    void testActionInTwoQActionGroup();
	void unCheckCurrentAction();
};

tst_QActionGroup::tst_QActionGroup()
{
}

tst_QActionGroup::~tst_QActionGroup()
{
}

void tst_QActionGroup::enabledPropagation()
{
    QActionGroup testActionGroup( 0 );

    QAction* childAction = new QAction( &testActionGroup );
    QAction* anotherChildAction = new QAction( &testActionGroup );
    QAction* freeAction = new QAction(0);

    QVERIFY( testActionGroup.isEnabled() );
    QVERIFY( childAction->isEnabled() );

    testActionGroup.setEnabled( false );
    QVERIFY( !testActionGroup.isEnabled() );
    QVERIFY( !childAction->isEnabled() );
    QVERIFY( !anotherChildAction->isEnabled() );

    childAction->setEnabled(true);
    QVERIFY( !childAction->isEnabled());

    anotherChildAction->setEnabled( false );

    testActionGroup.setEnabled( true );
    QVERIFY( testActionGroup.isEnabled() );
    QVERIFY( childAction->isEnabled() );
    QVERIFY( !anotherChildAction->isEnabled() );

    testActionGroup.setEnabled( false );
    QAction *lastChildAction = new QAction(&testActionGroup);

    QVERIFY(!lastChildAction->isEnabled());
    testActionGroup.setEnabled( true );
    QVERIFY(lastChildAction->isEnabled());

    freeAction->setEnabled(false);
    testActionGroup.addAction(freeAction);
    QVERIFY(!freeAction->isEnabled());
    delete freeAction;
}

void tst_QActionGroup::visiblePropagation()
{
    QActionGroup testActionGroup( 0 );

    QAction* childAction = new QAction( &testActionGroup );
    QAction* anotherChildAction = new QAction( &testActionGroup );
    QAction* freeAction = new QAction(0);

    QVERIFY( testActionGroup.isVisible() );
    QVERIFY( childAction->isVisible() );

    testActionGroup.setVisible( false );
    QVERIFY( !testActionGroup.isVisible() );
    QVERIFY( !childAction->isVisible() );
    QVERIFY( !anotherChildAction->isVisible() );

    anotherChildAction->setVisible(false);

    testActionGroup.setVisible( true );
    QVERIFY( testActionGroup.isVisible() );
    QVERIFY( childAction->isVisible() );

    QVERIFY( !anotherChildAction->isVisible() );

    testActionGroup.setVisible( false );
    QAction *lastChildAction = new QAction(&testActionGroup);

    QVERIFY(!lastChildAction->isVisible());
    testActionGroup.setVisible( true );
    QVERIFY(lastChildAction->isVisible());

    freeAction->setVisible(false);
    testActionGroup.addAction(freeAction);
    QVERIFY(!freeAction->isVisible());
    delete freeAction;
}

void tst_QActionGroup::exclusive()
{
    QActionGroup group(0);
    group.setExclusive(false);
    QVERIFY( !group.isExclusive() );

    QAction* actOne = new QAction( &group );
    actOne->setCheckable( true );
    QAction* actTwo = new QAction( &group );
    actTwo->setCheckable( true );
    QAction* actThree = new QAction( &group );
    actThree->setCheckable( true );

    group.setExclusive( true );
    QVERIFY( !actOne->isChecked() );
    QVERIFY( !actTwo->isChecked() );
    QVERIFY( !actThree->isChecked() );

    actOne->setChecked( true );
    QVERIFY( actOne->isChecked() );
    QVERIFY( !actTwo->isChecked() );
    QVERIFY( !actThree->isChecked() );

    actTwo->setChecked( true );
    QVERIFY( !actOne->isChecked() );
    QVERIFY( actTwo->isChecked() );
    QVERIFY( !actThree->isChecked() );
}

void tst_QActionGroup::dropDownDeleted()
{
    QSKIP("dropDownDeleted test for Qt 4.0 not expected to work since it is not implemented yet", SkipAll);

    QMainWindow mw;
    QToolBar *tb = new QToolBar(&mw);
    QActionGroup *actGroup = new QActionGroup(&mw);

    /// ### actGroup->setUsesDropDown(true);
    QAction *actOne = new QAction(actGroup);
    actOne->setText("test one");
    QAction *actTwo = new QAction(actGroup);
    actOne->setText("test one");
    QAction *actThree= new QAction(actGroup);
    actOne->setText("test one");

    QListIterator<QAction*> it(actGroup->actions());
    while (it.hasNext())
        tb->addAction(it.next());

    QList<QComboBox*> comboList = qFindChildren<QComboBox*>(tb);
    QCOMPARE(comboList[0]->count(), 3);

    delete actOne;
    QCOMPARE((int)comboList[0]->count(), 2);
    delete actTwo;
    QCOMPARE((int)comboList[0]->count(), 1);
    delete actThree;
    QCOMPARE((int)comboList[0]->count(), 0);

    delete actGroup;
}

void tst_QActionGroup::separators()
{
    QMainWindow mw;
    QMenu menu(&mw);
    QActionGroup actGroup(&mw);

    mw.show();

#ifdef QT_SOFTKEYS_ENABLED
    // Softkeys add extra "Select" and "Back" actions to menu by default.
    // Two first actions will be Select and Back when softkeys are enabled
    int numSoftkeyActions = 2;
#else
    int numSoftkeyActions = 0;
#endif

    QAction *action = new QAction(&actGroup);
    action->setText("test one");

    QAction *separator = new QAction(&actGroup);
    separator->setSeparator(true);
    actGroup.addAction(separator);

    QListIterator<QAction*> it(actGroup.actions());
    while (it.hasNext())
        menu.addAction(it.next());

    QCOMPARE((int)menu.actions().size(), 2 + numSoftkeyActions);

    it = QListIterator<QAction*>(actGroup.actions());
    while (it.hasNext())
        menu.removeAction(it.next());

    QCOMPARE((int)menu.actions().size(), 0 + numSoftkeyActions);

    action = new QAction(&actGroup);
    action->setText("test two");

    it = QListIterator<QAction*>(actGroup.actions());
    while (it.hasNext())
        menu.addAction(it.next());

    QCOMPARE((int)menu.actions().size(), 3 + numSoftkeyActions);
}

void tst_QActionGroup::testActionInTwoQActionGroup()
{
    QAction action1("Action 1", this);

    QActionGroup group1(this);
    QActionGroup group2(this);

    group1.addAction(&action1);
    group2.addAction(&action1);

    QCOMPARE(action1.actionGroup(), &group2);
    QCOMPARE(group2.actions().first(), &action1);
    QCOMPARE(group1.actions().isEmpty(), true);
}

void tst_QActionGroup::unCheckCurrentAction()
{
    QActionGroup group(0);
    QAction action1(&group) ,action2(&group);
    action1.setCheckable(true);
    action2.setCheckable(true);
    QVERIFY(!action1.isChecked());
    QVERIFY(!action2.isChecked());
    action1.setChecked(true);
    QVERIFY(action1.isChecked());
    QVERIFY(!action2.isChecked());
    QAction *current = group.checkedAction();
    QCOMPARE(current, &action1);
    current->setChecked(false);
    QVERIFY(!action1.isChecked());
    QVERIFY(!action2.isChecked());
    QVERIFY(group.checkedAction() == 0);
}


QTEST_MAIN(tst_QActionGroup)
#include "tst_qactiongroup.moc"
