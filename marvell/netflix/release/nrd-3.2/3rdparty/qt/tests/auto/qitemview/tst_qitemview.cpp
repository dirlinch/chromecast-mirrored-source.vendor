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
#include <QtCore/QtCore>
#include "viewstotest.cpp"
#include <stdlib.h>

#if defined(Q_OS_UNIX)
#include <time.h>
#endif
#if defined(Q_OS_WIN)
#include <time.h>
#if defined(Q_OS_WINCE)
#include <aygshell.h>
#endif
#define random rand
#define srandom srand

#if defined(Q_OS_WINCE)
bool qt_wince_is_platform(const QString &platformString) {
    wchar_t tszPlatform[64];
    if (SystemParametersInfo(SPI_GETPLATFORMTYPE,
                             sizeof(tszPlatform)/sizeof(*tszPlatform),tszPlatform,0))
      if (0 == _tcsicmp(reinterpret_cast<const wchar_t *> (platformString.utf16()), tszPlatform))
            return true;
    return false;
}

bool qt_wince_is_pocket_pc() {
    return qt_wince_is_platform(QString::fromLatin1("PocketPC"));
}

bool qt_wince_is_smartphone() {
       return qt_wince_is_platform(QString::fromLatin1("Smartphone"));
}
bool qt_wince_is_mobile() {
     return (qt_wince_is_smartphone() || qt_wince_is_pocket_pc());
}
#endif
#endif

//TESTED_CLASS=
//TESTED_FILES=gui/itemviews/qtreeview.h gui/itemviews/qtreeview.cpp

/*!
    See viewstotest.cpp for instructions on how to have your view tested with these tests.

    Each test such as visualRect have a _data() function which populate the QTest data with
    tests specified by viewstotest.cpp and any extra data needed for that perticular test.

    setupWithNoTestData() fills QTest data with only the tests it is used by most tests.

    There are some basic qDebug statements sprikled about that might be helpfull for
    fixing your issues.
 */
class tst_QItemView : public QObject
{
    Q_OBJECT

public:
    tst_QItemView() {};
    virtual ~tst_QItemView() {};

public slots:
    void init();
    void cleanup();

private slots:
    void nonDestructiveBasicTest_data();
    void nonDestructiveBasicTest();

    void spider_data();
    void spider();

    void resize_data();
    void resize();

    void visualRect_data();
    void visualRect();

    void indexAt_data();
    void indexAt();

    void scrollTo_data();
    void scrollTo();

    void moveCursor_data();
    void moveCursor();

private:
    void setupWithNoTestData();
    void populate();
    void walkScreen(QAbstractItemView *view);

    QAbstractItemView *view;
    QAbstractItemModel *treeModel;
    ViewsToTest *testViews;
};

/*!
 * Views should not make invalid requests, sense a model might not check all the bad cases.
 */
class CheckerModel : public QStandardItemModel
{
    Q_OBJECT

public:
    CheckerModel() : QStandardItemModel() {};

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole ) const {
        Q_ASSERT(index.isValid());
        return QStandardItemModel::data(index, role);
    };

    Qt::ItemFlags flags(const QModelIndex & index) const {
        Q_ASSERT(index.isValid());
        if (index.row() == 2 || index.row() == rowCount() - 3
            || index.column() == 2 || index.column() == columnCount() - 3) {
            Qt::ItemFlags f = QStandardItemModel::flags(index);
            f &= ~Qt::ItemIsEnabled;
            return f;
        }
        return QStandardItemModel::flags(index);
    };

    QModelIndex parent ( const QModelIndex & child ) const {
        Q_ASSERT(child.isValid());
        return QStandardItemModel::parent(child);
    };

    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
        Q_ASSERT(section >= 0);
        if (orientation == Qt::Horizontal) { Q_ASSERT(section <= columnCount());};
        if (orientation == Qt::Vertical) { Q_ASSERT(section <= rowCount());};
        return QStandardItemModel::headerData(section, orientation, role);
    }

    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const {
        return QStandardItemModel::index(row, column, parent);
    };

    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole ) {
        Q_ASSERT(index.isValid());
        return QStandardItemModel::setData(index, value, role);
    }

    void sort( int column, Qt::SortOrder order = Qt::AscendingOrder ) {
        Q_ASSERT(column >= 0 && column <= columnCount());
        QStandardItemModel::sort(column, order);
    };

    QModelIndexList match ( const QModelIndex & start, int role, const QVariant & value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap ) ) const {
        Q_ASSERT(hits > 0);
        Q_ASSERT(value.isValid());
        return QAbstractItemModel::match(start, role, value, hits, flags);
    };

    bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole ) {
        Q_ASSERT(section >= 0);
        return QAbstractItemModel::setHeaderData(section, orientation, value, role);
    };
};

void tst_QItemView::init()
{
#ifdef Q_OS_IRIX
    return;
#endif

    testViews = new ViewsToTest();
    populate();
}

void tst_QItemView::cleanup()
{
    delete testViews;
    delete view;
    delete treeModel;
    view = 0;
    testViews = 0;
    treeModel = 0;
}

void tst_QItemView::setupWithNoTestData()
{
#ifdef Q_OS_IRIX
    //QSKIP("This test takes too long to execute on IRIX", SkipAll);
    return;
#endif

    ViewsToTest testViews;
    QTest::addColumn<QString>("viewType");
    QTest::addColumn<bool>("displays");
    QTest::addColumn<int>("vscroll");
    QTest::addColumn<int>("hscroll");
    for (int i = 0; i < testViews.tests.size(); ++i) {
        QString view = testViews.tests.at(i).viewType;
        QString test = view + " ScrollPerPixel";
        bool displayIndexes = (testViews.tests.at(i).display == ViewsToTest::DisplayRoot);
        QTest::newRow(test.toLatin1().data()) << view << displayIndexes
                                              << (int)QAbstractItemView::ScrollPerPixel
                                              << (int)QAbstractItemView::ScrollPerPixel
                                              ;
    }
    for (int i = 0; i < testViews.tests.size(); ++i) {
        QString view = testViews.tests.at(i).viewType;
        QString test = view + " ScrollPerItem";
        bool displayIndexes = (testViews.tests.at(i).display == ViewsToTest::DisplayRoot);
        QTest::newRow(test.toLatin1().data()) << view << displayIndexes
                                              << (int)QAbstractItemView::ScrollPerItem
                                              << (int)QAbstractItemView::ScrollPerItem
                                              ;
    }
}

void tst_QItemView::populate()
{
    treeModel = new CheckerModel;
    QModelIndex parent;
#if defined(QT_ARCH_ARM) || defined(Q_OS_SYMBIAN)
    const int baseInsert = 4;
#else
    const int baseInsert = 26;
#endif
    for (int i = 0; i < 40; ++i) {
        parent = treeModel->index(0, 0, parent);
        treeModel->insertRows(0, baseInsert + i, parent);
        treeModel->insertColumns(0, baseInsert + i, parent);
        // Fill in some values to make it easier to debug
        for (int x = 0; x < treeModel->rowCount(); ++x) {
            for (int y = 0; y < treeModel->columnCount(); ++y) {
                QModelIndex index = treeModel->index(x, y, parent);
                treeModel->setData(index, QString("%1_%2_%3").arg(x).arg(y).arg(i));
                treeModel->setData(index, QVariant(QColor(Qt::blue)), Qt::TextColorRole);
            }
        }
    }
}

void tst_QItemView::nonDestructiveBasicTest_data()
{
    setupWithNoTestData();
}

/*!
    nonDestructiveBasicTest trys to call a number of the basic functions (not all)
    to make sure the view doesn't segfault, testing the functions that makes sense.
 */
void tst_QItemView::nonDestructiveBasicTest()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif

#ifdef Q_OS_WINCE
     QTest::qWait(400);
#endif

    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    QFETCH(int, vscroll);
    QFETCH(int, hscroll);
    view->setVerticalScrollMode((QAbstractItemView::ScrollMode)vscroll);
    view->setHorizontalScrollMode((QAbstractItemView::ScrollMode)hscroll);

    // setSelectionModel() will assert
    //view->setSelectionModel(0);
    // setItemDelegate() will assert
    //view->setItemDelegate(0);

    // setSelectionMode
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::SingleSelection);
    view->setSelectionMode(QAbstractItemView::ContiguousSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::ContiguousSelection);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::ExtendedSelection);
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::MultiSelection);
    view->setSelectionMode(QAbstractItemView::NoSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::NoSelection);

    // setSelectionBehavior
    view->setSelectionBehavior(QAbstractItemView::SelectItems);
    QCOMPARE(view->selectionBehavior(), QAbstractItemView::SelectItems);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    QCOMPARE(view->selectionBehavior(), QAbstractItemView::SelectRows);
    view->setSelectionBehavior(QAbstractItemView::SelectColumns);
    QCOMPARE(view->selectionBehavior(), QAbstractItemView::SelectColumns);

    // setEditTriggers
    view->setEditTriggers(QAbstractItemView::EditKeyPressed);
    QCOMPARE(view->editTriggers(), QAbstractItemView::EditKeyPressed);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QCOMPARE(view->editTriggers(), QAbstractItemView::NoEditTriggers);
    view->setEditTriggers(QAbstractItemView::CurrentChanged);
    QCOMPARE(view->editTriggers(), QAbstractItemView::CurrentChanged);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);
    QCOMPARE(view->editTriggers(), QAbstractItemView::DoubleClicked);
    view->setEditTriggers(QAbstractItemView::SelectedClicked);
    QCOMPARE(view->editTriggers(), QAbstractItemView::SelectedClicked);
    view->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    QCOMPARE(view->editTriggers(), QAbstractItemView::AnyKeyPressed);
    view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    QCOMPARE(view->editTriggers(), QAbstractItemView::AllEditTriggers);

    // setAutoScroll
    view->setAutoScroll(false);
    QCOMPARE(view->hasAutoScroll(), false);
    view->setAutoScroll(true);
    QCOMPARE(view->hasAutoScroll(), true);

    // setTabKeyNavigation
    view->setTabKeyNavigation(false);
    QCOMPARE(view->tabKeyNavigation(), false);
    view->setTabKeyNavigation(true);
    QCOMPARE(view->tabKeyNavigation(), true);
#ifndef QT_NO_DRAGANDDROP
    // setDropIndicatorShown
    view->setDropIndicatorShown(false);
    QCOMPARE(view->showDropIndicator(), false);
    view->setDropIndicatorShown(true);
    QCOMPARE(view->showDropIndicator(), true);

    // setDragEnabled
    view->setDragEnabled(false);
    QCOMPARE(view->dragEnabled(), false);
    view->setDragEnabled(true);
    QCOMPARE(view->dragEnabled(), true);
#endif

    // setAlternatingRowColors
    view->setAlternatingRowColors(false);
    QCOMPARE(view->alternatingRowColors(), false);
    view->setAlternatingRowColors(true);
    QCOMPARE(view->alternatingRowColors(), true);

    // setIconSize
    view->setIconSize(QSize(16, 16));
    QCOMPARE(view->iconSize(), QSize(16, 16));
    view->setIconSize(QSize(32, 32));
    QCOMPARE(view->iconSize(), QSize(32, 32));
    // Should this happen?
    view->setIconSize(QSize(-1, -1));
    QCOMPARE(view->iconSize(), QSize(-1, -1));

    QCOMPARE(view->currentIndex(), QModelIndex());
    QCOMPARE(view->rootIndex(), QModelIndex());

    view->keyboardSearch("");
    view->keyboardSearch("foo");
    view->keyboardSearch("1");

    QCOMPARE(view->visualRect(QModelIndex()), QRect());

    view->scrollTo(QModelIndex());

    QCOMPARE(view->sizeHintForIndex(QModelIndex()), QSize());
    QCOMPARE(view->indexAt(QPoint(-1, -1)), QModelIndex());

    if (!view->model()){
        QCOMPARE(view->indexAt(QPoint(10, 10)), QModelIndex());
        QCOMPARE(view->sizeHintForRow(0), -1);
        QCOMPARE(view->sizeHintForColumn(0), -1);
    } else if (view->itemDelegate()){
        view->sizeHintForRow(0);
        view->sizeHintForColumn(0);
    }
    view->openPersistentEditor(QModelIndex());
    view->closePersistentEditor(QModelIndex());

    view->reset();
    view->setRootIndex(QModelIndex());
    view->doItemsLayout();
    view->selectAll();
    // edit() causes warning by default
    //view->edit(QModelIndex());
    view->clearSelection();
    view->setCurrentIndex(QModelIndex());
}

void tst_QItemView::spider_data()
{
    setupWithNoTestData();
}

void touch(QWidget *widget, Qt::KeyboardModifier modifier, Qt::Key keyPress){
    int width = widget->width();
    int height = widget->height();
    for (int i = 0; i < 5; ++i) {
        QTest::mouseClick(widget, Qt::LeftButton, modifier, QPoint(random() % width, random() % height));
        QTest::mouseDClick(widget, Qt::LeftButton, modifier, QPoint(random() % width, random() % height));
        QPoint press(random() % width, random() % height);
        QPoint releasePoint(random() % width, random() % height);
        QTest::mousePress(widget, Qt::LeftButton, modifier, press);
        QTest::mouseMove(widget, releasePoint);
        if (random() % 1 == 0)
            QTest::mouseRelease(widget, Qt::LeftButton, 0, releasePoint);
        else
            QTest::mouseRelease(widget, Qt::LeftButton, modifier, releasePoint);
        QTest::keyClick(widget, keyPress);
    }
}

/*!
    This is a basic stress testing application that tries a few basics such as clicking around
    the screen, and key presses.

    The main goal is to catch any easy segfaults, not to test every case.
  */
void tst_QItemView::spider()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif
    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    QFETCH(int, vscroll);
    QFETCH(int, hscroll);
    view->setVerticalScrollMode((QAbstractItemView::ScrollMode)vscroll);
    view->setHorizontalScrollMode((QAbstractItemView::ScrollMode)hscroll);
    view->setModel(treeModel);
    view->show();
#if defined(Q_OS_WINCE) || defined(Q_OS_SYMBIAN)
    srandom(0);
#else
    srandom(time(0));
#endif
    touch(view->viewport(), Qt::NoModifier, Qt::Key_Left);
    touch(view->viewport(), Qt::ShiftModifier, Qt::Key_Enter);
    touch(view->viewport(), Qt::ControlModifier, Qt::Key_Backspace);
    touch(view->viewport(), Qt::AltModifier, Qt::Key_Up);
}

void tst_QItemView::resize_data()
{
    setupWithNoTestData();
}

/*!
    The main goal is to catch any infinite loops from layouting
  */
void tst_QItemView::resize()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif
   return;
    // This test needs to be re-thought out, it takes too long and
    // doesn't really catch theproblem.
    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    QFETCH(int, vscroll);
    QFETCH(int, hscroll);
    view->setVerticalScrollMode((QAbstractItemView::ScrollMode)vscroll);
    view->setHorizontalScrollMode((QAbstractItemView::ScrollMode)hscroll);
    view->setModel(treeModel);
    view->show();

    for (int w = 100; w < 400; w+=10) {
        for (int h = 100; h < 400; h+=10) {
            view->resize(w, h);
            QTest::qWait(1);
            qApp->processEvents();
        }
    }
}

void tst_QItemView::visualRect_data()
{
    setupWithNoTestData();
}

void tst_QItemView::visualRect()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif
    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    QFETCH(int, vscroll);
    QFETCH(int, hscroll);
    view->setVerticalScrollMode((QAbstractItemView::ScrollMode)vscroll);
    view->setHorizontalScrollMode((QAbstractItemView::ScrollMode)hscroll);
    QCOMPARE(view->visualRect(QModelIndex()), QRect());

    // Add model
    view->setModel(treeModel);
    QCOMPARE(view->visualRect(QModelIndex()), QRect());

    QModelIndex topIndex = treeModel->index(0,0);

    QFETCH(bool, displays);
    if (!displays){
        QVERIFY(view->visualRect(topIndex) == QRect());
        return;
    }

    QVERIFY(view->visualRect(topIndex) != QRect());
    view->show();
    QVERIFY(view->visualRect(topIndex) != QRect());

    QVERIFY(topIndex == view->indexAt(view->visualRect(topIndex).center()));
    QVERIFY(topIndex == view->indexAt(view->visualRect(topIndex).bottomLeft()));
    QVERIFY(topIndex == view->indexAt(view->visualRect(topIndex).bottomRight()));
    QVERIFY(topIndex == view->indexAt(view->visualRect(topIndex).topLeft()));
    QVERIFY(topIndex == view->indexAt(view->visualRect(topIndex).topRight()));

    testViews->hideIndexes(view);
    QModelIndex hiddenIndex = treeModel->index(1, 0);
    QVERIFY(view->visualRect(hiddenIndex) == QRect());
}

void tst_QItemView::walkScreen(QAbstractItemView *view)
{
    QModelIndex hiddenIndex = view->model() ? view->model()->index(1, 0) : QModelIndex();
    int width = view->width();
    int height = view->height();
    for (int w = 0; w < width; ++w)
    {
        for (int h = 0; h < height; ++h)
        {
            QPoint point(w, h);
            QModelIndex index = view->indexAt(point);

            // If we have no model then we should *never* get a valid index
            if (!view->model() || !view->isVisible())
                QVERIFY(!index.isValid());
            // index should not be the hidden one
            if (hiddenIndex.isValid())
                QVERIFY(hiddenIndex != index);
            // If we are valid then check the visualRect for that index
            if (index.isValid()){
                QRect visualRect = view->visualRect(index);
                if (!visualRect.contains(point))
                    qDebug() << point << visualRect;
                QVERIFY(visualRect.contains(point));
            }
        }
    }
}

void walkIndex(QModelIndex index, QAbstractItemView *view)
{
    QRect visualRect = view->visualRect(index);
    //if (index.column() == 0)
    //qDebug() << index << visualRect;
    int width = visualRect.width();
    int height = visualRect.height();

    for (int w = 0; w < width; ++w)
    {
        for (int h = 0; h < height; ++h)
        {
            QPoint point(visualRect.x()+w, visualRect.y()+h);
            if (view->indexAt(point) != index) {
                qDebug() << "index" << index << "visualRect" << visualRect << point << view->indexAt(point);
            }
            QVERIFY(view->indexAt(point) == index);
        }
    }

}

/*!
    A model that returns an index of parent X should also return X when asking
    for the parent of the index.

    This recursive function does pretty extensive testing on the whole model in an
    effort to catch edge cases.

    This function assumes that rowCount(), columnCount() and index() work.  If they have
    a bug it will point it out, but the above tests should have already found the basic bugs
    because it is easier to figure out the problem in those tests then this one.
 */
void checkChildren(QAbstractItemView *currentView, const QModelIndex &parent = QModelIndex(), int currentDepth=0)
{
    QAbstractItemModel *currentModel = currentView->model();

    int rows = currentModel->rowCount(parent);
    int columns = currentModel->columnCount(parent);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            QModelIndex index = currentModel->index(r, c, parent);
            walkIndex(index, currentView);
            if (QTest::currentTestFailed())
                return;

            // recursivly go down
            if (currentModel->hasChildren(index) && currentDepth < 2) {
                checkChildren(currentView, index, ++currentDepth);
                // Because this is recursive we will return at the first failure rather then
                // reporting it over and over
                if (QTest::currentTestFailed())
                    return;
            }
        }
    }
}


void tst_QItemView::indexAt_data()
{
    setupWithNoTestData();
}

void tst_QItemView::indexAt()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif
    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    QFETCH(int, vscroll);
    QFETCH(int, hscroll);
    view->setVerticalScrollMode((QAbstractItemView::ScrollMode)vscroll);
    view->setHorizontalScrollMode((QAbstractItemView::ScrollMode)hscroll);
    view->show();
    view->setModel(treeModel);
#if 0
    checkChildren(view);

    QModelIndex index = view->model()->index(0, 0);
    while (view->model()->hasChildren(index))
        index = view->model()->index(0, 0, index);
    QCOMPARE(view->model()->hasChildren(index), false);
    QVERIFY(index.isValid());
    view->setRootIndex(index);
    //qDebug() << view->indexAt(QPoint(view->width()/2, view->height()/2)) << view->rootIndex();
    QPoint p(1, view->height()/2);
    QModelIndex idx = view->indexAt(p);
    QCOMPARE(idx, QModelIndex());
#endif
}

void tst_QItemView::scrollTo_data()
{
    setupWithNoTestData();
}

void tst_QItemView::scrollTo()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif
    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    QFETCH(int, vscroll);
    QFETCH(int, hscroll);
    view->setVerticalScrollMode((QAbstractItemView::ScrollMode)vscroll);
    view->setHorizontalScrollMode((QAbstractItemView::ScrollMode)hscroll);
    view->setModel(treeModel);
    view->show();

    QModelIndex parent;
    for (int row = 0; row < treeModel->rowCount(parent); ++row) {
        for (int column = 0; column < treeModel->columnCount(parent); ++column) {
            QModelIndex idx = treeModel->index(row, column, parent);
            view->scrollTo(idx);
            QRect rect = view->visualRect(idx);
            view->scrollTo(idx);
            QCOMPARE(rect, view->visualRect(idx));
        }
    }

    QModelIndex idx = treeModel->index(0, 0, parent);
    view->scrollTo(idx);
    QRect rect = view->visualRect(idx);
    view->scrollToBottom();
    view->scrollTo(idx);
    QCOMPARE(rect, view->visualRect(idx));
}

void tst_QItemView::moveCursor_data()
{
    setupWithNoTestData();
}

class Event {
public:
    Event(){}
    Event(Qt::Key k, QModelIndex s, QModelIndex e, QString n) : key(k), start(s), end(e), name(n){}
    Qt::Key key;
    QModelIndex start;
    QModelIndex end;
    QString name;
};


void tst_QItemView::moveCursor()
{
#ifdef Q_OS_IRIX
    QSKIP("This test takes too long to execute on IRIX", SkipAll);
#endif
    QFETCH(QString, viewType);
    view = testViews->createView(viewType);
    if (view->objectName() == "QHeaderView")
        return;

    view->setModel(treeModel);
    testViews->hideIndexes(view);
    view->resize(100, 100);

    QModelIndex invalidIndex = QModelIndex();
    QModelIndex firstRow = treeModel->index(0, 0);
    QModelIndex hiddenRowT = treeModel->index(1, 0);
    QModelIndex disabledRowT = treeModel->index(2, 0);
    QModelIndex secondRow = treeModel->index(3, 0);

    QModelIndex secondToLastRow = treeModel->index(treeModel->rowCount() - 4, 0);
    QModelIndex disabledRowB = treeModel->index(treeModel->rowCount() - 3, 0);
    QModelIndex hiddenRowB = treeModel->index(treeModel->rowCount() - 2, 0);
    QModelIndex lastRow = treeModel->index(treeModel->rowCount() - 1, 0);

    QStack<Event> events;

    events.push(Event(Qt::Key_Up, invalidIndex,      firstRow, "inv, first"));
    events.push(Event(Qt::Key_Up, hiddenRowT,        firstRow, "hid, first"));
    events.push(Event(Qt::Key_Up, disabledRowT,      firstRow, "dis, first"));
    events.push(Event(Qt::Key_Up, firstRow,          firstRow, "first, first"));
    events.push(Event(Qt::Key_Up, secondRow,         firstRow, "sec, first"));
    events.push(Event(Qt::Key_Up, hiddenRowB,        firstRow, "hidB, first"));
    events.push(Event(Qt::Key_Up, disabledRowB,      secondToLastRow, "disB, secLast"));
    events.push(Event(Qt::Key_Up, lastRow,           secondToLastRow, "last, secLast"));

    events.push(Event(Qt::Key_Down, invalidIndex,    firstRow, "inv, first"));
    events.push(Event(Qt::Key_Down, hiddenRowT,      firstRow, "hid, first"));
    events.push(Event(Qt::Key_Down, disabledRowT,    secondRow, "dis, sec"));
    events.push(Event(Qt::Key_Down, firstRow,        secondRow, "first, sec"));
    events.push(Event(Qt::Key_Down, secondToLastRow, lastRow, "secLast, last" ));
    events.push(Event(Qt::Key_Down, disabledRowB,    lastRow, "disB, last"));
    events.push(Event(Qt::Key_Down, hiddenRowB,      firstRow, "hidB, first"));
    events.push(Event(Qt::Key_Down, lastRow,         lastRow, "last, last"));

    events.push(Event(Qt::Key_Home, invalidIndex,    firstRow, "inv, first"));
    events.push(Event(Qt::Key_End, invalidIndex,     firstRow, "inv, first"));

    if (view->objectName() == "QTableView") {
        // In a table we move to the first/last column
        events.push(Event(Qt::Key_Home, hiddenRowT,      firstRow, "hid, first"));
        events.push(Event(Qt::Key_Home, disabledRowT,    disabledRowT, "dis, dis"));
        events.push(Event(Qt::Key_Home, firstRow,        firstRow, "first, first"));
        events.push(Event(Qt::Key_Home, secondRow,       secondRow, "sec, sec"));
        events.push(Event(Qt::Key_Home, disabledRowB,    disabledRowB, "disB, disB"));
        events.push(Event(Qt::Key_Home, hiddenRowB,      firstRow, "hidB, first"));
        events.push(Event(Qt::Key_Home, secondToLastRow, secondToLastRow, "secLast, secLast"));
        events.push(Event(Qt::Key_Home, lastRow,         lastRow, "last, last"));

        int col = treeModel->columnCount() - 1;
        events.push(Event(Qt::Key_End, hiddenRowT,      firstRow, "hidT, hidT"));
        events.push(Event(Qt::Key_End, disabledRowT,    disabledRowT, "disT, disT"));
        events.push(Event(Qt::Key_End, firstRow,        firstRow.sibling(firstRow.row(), col), "first, first_C"));
        events.push(Event(Qt::Key_End, secondRow,       secondRow.sibling(secondRow.row(), col), "sec, sec_C"));
        events.push(Event(Qt::Key_End, disabledRowB,    disabledRowB, "disB, disB"));
        events.push(Event(Qt::Key_End, hiddenRowB,      firstRow, "hidB, hidB"));
        events.push(Event(Qt::Key_End, secondToLastRow, secondToLastRow.sibling(secondToLastRow.row(), col), "secLast, secLast_C"));
        events.push(Event(Qt::Key_End, lastRow,         lastRow.sibling(lastRow.row(), col), "last, last_C"));
    } else {
        events.push(Event(Qt::Key_Home, hiddenRowT,      firstRow, "hid, first"));
        events.push(Event(Qt::Key_Home, disabledRowT,    firstRow, "dis, first"));
        events.push(Event(Qt::Key_Home, firstRow,        firstRow, "first, first"));
        events.push(Event(Qt::Key_Home, secondRow,       firstRow, "sec, first"));
        events.push(Event(Qt::Key_Home, disabledRowB,    firstRow, "disB, first"));
        events.push(Event(Qt::Key_Home, hiddenRowB,      firstRow, "hidB, first"));
        events.push(Event(Qt::Key_Home, secondToLastRow, firstRow, "sec, first"));
        events.push(Event(Qt::Key_Home, lastRow,         firstRow, "last, first"));

        events.push(Event(Qt::Key_End, hiddenRowT,       firstRow, "hid, last"));
        events.push(Event(Qt::Key_End, disabledRowT,     lastRow, "dis, last"));
        events.push(Event(Qt::Key_End, firstRow,         lastRow, "first, last"));
        events.push(Event(Qt::Key_End, secondRow,        lastRow, "sec, last"));
        events.push(Event(Qt::Key_End, disabledRowB,     lastRow, "disB, last"));
        events.push(Event(Qt::Key_End, hiddenRowB,       firstRow, "hidB, last"));
        events.push(Event(Qt::Key_End, secondToLastRow,  lastRow, "sec, last"));
        events.push(Event(Qt::Key_End, lastRow,          lastRow, "last, last"));
    }

    events.push(Event(Qt::Key_PageDown, invalidIndex,    firstRow, "inv, first"));
    events.push(Event(Qt::Key_PageDown, firstRow,        QModelIndex(), "first, x"));
    events.push(Event(Qt::Key_PageDown, secondRow,       QModelIndex(), "sec, x"));
    events.push(Event(Qt::Key_PageDown, hiddenRowT,      QModelIndex(), "hid, x"));
    events.push(Event(Qt::Key_PageDown, disabledRowT,    QModelIndex(), "dis, x"));
    events.push(Event(Qt::Key_PageDown, disabledRowB,    lastRow, "disB, last"));
    events.push(Event(Qt::Key_PageDown, hiddenRowB,      lastRow, "hidB, last"));
    events.push(Event(Qt::Key_PageDown, secondToLastRow, lastRow, "secLast, last"));
    events.push(Event(Qt::Key_PageDown, lastRow,         lastRow, "last, last"));

    events.push(Event(Qt::Key_PageUp, invalidIndex,    firstRow, "inv, first"));
    events.push(Event(Qt::Key_PageUp, firstRow,        firstRow, "first, first"));
    events.push(Event(Qt::Key_PageUp, secondRow,       firstRow, "sec, first"));
    events.push(Event(Qt::Key_PageUp, secondToLastRow, QModelIndex(), "secLast, x"));
    events.push(Event(Qt::Key_PageUp, lastRow,         QModelIndex(), "last, x"));

    if (view->objectName() == "QTableView") {
        events.push(Event(Qt::Key_Left, firstRow,                      firstRow, "first_0, first"));
        events.push(Event(Qt::Key_Left, firstRow.sibling(0, 1),        firstRow, "first_1, first"));
        events.push(Event(Qt::Key_Left, firstRow.sibling(0, 2),        firstRow, "first_2, first"));
        events.push(Event(Qt::Key_Left, firstRow.sibling(0, 3),        firstRow, "first_3, first"));
        events.push(Event(Qt::Key_Left, secondRow,        secondRow, "sec, sec"));

        events.push(Event(Qt::Key_Right, firstRow,                      firstRow.sibling(0, 3), "first, first_3"));
        events.push(Event(Qt::Key_Right, firstRow.sibling(0, 1),        firstRow, "first_1, first"));
        events.push(Event(Qt::Key_Right, firstRow.sibling(0, 2),        firstRow.sibling(0, 3), "first_2, first_3"));
        events.push(Event(Qt::Key_Right, firstRow.sibling(0, treeModel->columnCount()-1), firstRow.sibling(0, treeModel->columnCount()-1), "first_3, sec"));
    }

#if 0 // ### disable this part for now

    // ### hide the first/last row,column and re-run all of these tests
    // ### Not 100% sure, but I think the next to are tableview specific only and everyone else just does up/down
    // QAbstractItemView::MoveNext, AbstractItemView::MovePrevious

    while (!events.isEmpty()) {
        Event event = events.pop();
        view->setCurrentIndex(event.start);
        QCOMPARE(view->currentIndex(), event.start);

        if (event.key == Qt::Key_PageUp && event.end == QModelIndex()) {
            QModelIndex x = view->indexAt(QPoint(1,1));
            if (x.row() == 0)
                event.end = x;
            else
                event.end = x.sibling(x.row() - 1, x.column());
        }
        if (event.key == Qt::Key_PageDown && event.end == QModelIndex()) {
            QModelIndex x = view->indexAt(QPoint(1, view->viewport()->height() - 10));
            if (x.row() == view->model()->rowCount() - 1)
                event.end = x;
            else
                event.end = x.sibling(x.row() + 1, x.column());
        }

        QTest::keyPress(view, event.key);
        QTest::keyRelease(view, event.key);
        QModelIndex current = view->currentIndex();
        if (event.key == Qt::Key_PageUp) {
            int diff = event.end.row() - current.row();
            QVERIFY(diff <= 2);
            continue;
        }
        if (event.key == Qt::Key_PageDown) {
            int diff = current.row() - event.end.row();
            QVERIFY(diff <= 2);
            continue;
        }

        if (current != event.end) {
            QString k;
            if (event.key == Qt::Key_Up) k = "up";
            if (event.key == Qt::Key_Right) k = "right";
            if (event.key == Qt::Key_Left) k = "left";
            if (event.key == Qt::Key_PageUp) k = "page up";
            if (event.key == Qt::Key_PageDown) k = "page down";
            if (event.key == Qt::Key_Down) k = "down";
            if (event.key == Qt::Key_Home) k = "home";
            if (event.key == Qt::Key_End) k = "end";
            qDebug() << k << event.name << event.start << event.end << current;
        }
        QCOMPARE(current, event.end);
    }
#endif
}

QTEST_MAIN(tst_QItemView)
#include "tst_qitemview.moc"
