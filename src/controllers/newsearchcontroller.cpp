#include "controllers/newsearchcontroller.h"

#include <iostream>

#include "controllers/editorcontroller.h"
#include "uis/mainwindow.h"
#include "uis/advancedsearch.h"

namespace o3prm
{
    NewSearchController::NewSearchController(MainWindow* mw, QObject *parent):
        QObject(parent),
        __mw(mw),
        __dialog(new AdvancedSearch(mw)),
        __searchModel(new SearchModel(this))
    {
    }

    NewSearchController::~NewSearchController()
    {

    }

    void NewSearchController::setupConnections()
    {
        auto ui = __dialog->ui();

        connect(__mw->mainWindow()->actionFind, SIGNAL(triggered()),
                this, SLOT(showSearchDialog()));

        connect(ui->currentFileRadio, SIGNAL(toggled(bool)),
                this, SLOT(onCurrentFileOptionChecked(bool)));
        connect(ui->openedFilesRadio, SIGNAL(toggled(bool)),
                this, SLOT(onOpenedFileOptionChecked(bool)));
        connect(ui->currentProjectRadio, SIGNAL(toggled(bool)),
                this, SLOT(onCurrentProjectOptionChecked(bool)));

        connect(ui->searchEdit, SIGNAL(textChanged(const QString&)),
                this, SLOT(onQuickSearchEditTextChanged(QString)));
        connect(ui->searchEdit, SIGNAL(returnPressed()),
                this, SLOT(onSearchButtonClicked()));

        connect(ui->replaceEdit, SIGNAL(textChanged(const QString&)),
                this, SLOT(onQuickReplaceEditTextChanged(QString)));
        connect(ui->replaceEdit, SIGNAL(returnPressed()),
                this, SLOT(onSearchButtonClicked()));

        connect(ui->regexBox, SIGNAL(toggled(bool)),
                this, SLOT(onRegularExpressionOptionChecked(bool)));
        connect(ui->caseBox, SIGNAL(toggled(bool)),
                this, SLOT(onCaseSensitiveOptionChecked(bool)));
        connect(ui->wordBox, SIGNAL(toggled(bool)),
                this, SLOT(onWordOptionChecked(bool)));
        connect(ui->reverseBox, SIGNAL(toggled(bool)),
                this, SLOT(onReverseSearchOptionChecked(bool)));

        connect(ui->cancelButton, SIGNAL(clicked()),
                this, SLOT(onCancelButtonClicked()));
        connect(ui->searchButton, SIGNAL(clicked()),
                this, SLOT(onSearchButtonClicked()));
        connect(ui->replaceAndNextButton, SIGNAL(clicked()),
                this, SLOT(onReplaceandSearchButtonClicked()));
        connect(ui->replaceAllButton, SIGNAL(clicked()),
                this, SLOT(onReplaceAllButtonClicked()));
    }

    void NewSearchController::showSearchDialog()
    {
        auto sci = __mw->editorController()->currentDocument();
        if ( sci != 0 && sci->hasSelectedText() )
        {
            __dialog->ui()->searchEdit->setText( sci->selectedText() );
        }

        __dialog->show();
        __dialog->raise();
        __dialog->activateWindow();
    }

    void NewSearchController::onQuickSearchEditTextChanged( QString value)
    {
        __searchModel->searchValue(value);
    }

    void NewSearchController::onQuickReplaceEditTextChanged( QString value)
    {
        __searchModel->replaceValue(value);
    }

    void NewSearchController::onCurrentFileOptionChecked( bool checked )
    {
        __searchModel->searchOption(SearchModel::SearchOption::CurrentFile);
    }

    void NewSearchController::onOpenedFileOptionChecked( bool checked )
    {
        __searchModel->searchOption(SearchModel::SearchOption::OpenedFile);
    }

    void NewSearchController::onCurrentProjectOptionChecked( bool checked )
    {
        __searchModel->searchOption(SearchModel::SearchOption::CurrentProject);
    }

    void NewSearchController::onRegularExpressionOptionChecked( bool checked )
    {
        __searchModel->regularExpressionOption(checked);
    }

    void NewSearchController::onCaseSensitiveOptionChecked( bool checked )
    {
        __searchModel->caseSensitiveOption(checked);
    }

    void NewSearchController::onWordOptionChecked( bool checked )
    {
        __searchModel->wordsOnlyOption(checked);
    }

    void NewSearchController::onReverseSearchOptionChecked( bool checked )
    {
        __searchModel->reverseSearchOption(checked);
    }

    void NewSearchController::onCancelButtonClicked()
    {
        __dialog->hide();
    }

    void NewSearchController::onSearchButtonClicked()
    {
        switch (__searchModel->searchOption())
        {
            case SearchModel::SearchOption::CurrentFile:
            {
                __searchCurrentDoc();
                break;
            }
            case SearchModel::SearchOption::OpenedFile:
            {
                __searchOpenedFiles();
                break;
            }
            case SearchModel::SearchOption::CurrentProject:
            {
                __searchCurrentProject();
                break;
            }
            default: { }
        }
    }

    void NewSearchController::onReplaceandSearchButtonClicked()
    {
        std::cout << "Searching " << __searchModel->searchValue().toStdString()
            << " and replacing with " << __searchModel->replaceValue().toStdString() << std::endl;
    }

    void NewSearchController::onReplaceAllButtonClicked()
    {
        std::cout << "Searching " << __searchModel->searchValue().toStdString()
            << " and replacing ALL with " << __searchModel->replaceValue().toStdString() << std::endl;
    }

    void NewSearchController::__searchCurrentDoc()
    {
        auto sci = __mw->editorController()->currentDocument();

        if ( sci )
        {
            bool result = sci->findFirst( __searchModel->searchValue(),
                    __searchModel->regularExpressionOption(),
                    __searchModel->caseSensitiveOption(),
                    __searchModel->wordsOnlyOption(),
                    true, true, -1, -1, true );

            if ( not result )
            {
                __dialog->ui()->searchEdit->setStyleSheet( "background-color: red" );
                __dialog->ui()->searchEdit->setFocus();
            }
            else
            {
                __dialog->ui()->searchEdit->setStyleSheet( "" );
            }
        }
    }

    void NewSearchController::__searchOpenedFiles()
    {
        std::cout << "In __searchOpenedFiles()" << std::endl;
        int index = __mw->mainWindow()->tabWidget->currentIndex();
        if (index > -1)
        {
            for (int i = 0; i < __mw->mainWindow()->tabWidget->count(); ++i)
            {
                std::cout << "Index: " << index << std::endl;
                auto sci = __mw->editorController()->findDocument(index);
                // We want to start at selection for the first doc, then at
                // the begining for the other ones.
                int line = (i==0?-1:0);
                int column = (i==0?-1:0);

                if ( sci )
                {
                    std::cout << "sci: " << sci << std::endl;
                    bool result = sci->findFirst( __searchModel->searchValue(),
                            __searchModel->regularExpressionOption(),
                            __searchModel->caseSensitiveOption(),
                            __searchModel->wordsOnlyOption(),
                            false, true, line, column, true );

                    std::cout << "results: " << (result?"true":"false") << std::endl;
                    if ( not result )
                    {
                        // We proceed to the nex tab
                        ++index;
                        if (index == __mw->mainWindow()->tabWidget->count())
                        {
                            index = 0;
                        }
                        __mw->mainWindow()->tabWidget->setCurrentIndex(index);
                    }
                    else
                    {
                        __dialog->ui()->searchEdit->setStyleSheet( "" );
                        break;
                    }
                }
            }
            std::cout << "Found nothing..." << std::endl;
            // We get here if search is unsucessfull
            __dialog->ui()->searchEdit->setStyleSheet( "background-color: red" );
            __dialog->ui()->searchEdit->setFocus();
        }
    }

    void NewSearchController::__searchCurrentProject()
    {

    }

}
