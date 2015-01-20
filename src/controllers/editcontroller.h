#ifndef EDITCONTROLLER_H
#define EDITCONTROLLER_H

#include <QObject>
#include <QCompleter>
#include <QShortcut>
#include<QAction>
#include "uis/properties.h"

/// Cross reference, do not replace by an include directive
class MainWindow; // "uis/mainwindow.h"

class EditController : public QObject {
    Q_OBJECT

  public:
    explicit EditController( MainWindow * mw, QObject * parent = 0 );
    ~EditController();

    //! Return the completer
    QCompleter * completer() const;

  public slots:

    void undo();
    void redo();

    void cut();
    void copy();
    void paste();

    void remove();
    void selectAll();

    void switchComment();

    void increaseIndentation();
    void decreaseIndentation();

    void autoComplete();

    void editPreferences();

  signals:
    void completerChanged();

  protected slots:
    void onProjectModelChanged();

  private slots:
    void triggerInit();

  private:
    MainWindow * mw;
    QAction *undoAct;

    struct PrivateData;
    PrivateData * d;
    QShortcut *shortcutz;

    Properties * pr;
};

#endif // EDITCONTROLLER_H
