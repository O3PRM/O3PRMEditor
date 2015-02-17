#ifndef O3PRM_NEW_SEARCH_CONTROLLER
#define O3PRM_NEW_SEARCH_CONTROLLER

#include <QObject>

class MainWindow;

#include "QsciScintillaExtended.h"
#include "models/searchmodel.h"

namespace o3prm
{
    class AdvancedSearch;

    class NewSearchController: public QObject
    {
        Q_OBJECT

        public:
            NewSearchController(MainWindow* mw, QObject* parent=0);
            ~NewSearchController();

            void setupConnections();

        private slots:
            void showSearchDialog();

            void onQuickSearchEditTextChanged( QString value );
            void onQuickReplaceEditTextChanged( QString value);

            void onCurrentFileOptionChecked( bool checked );
            void onOpenedFileOptionChecked( bool checked );
            void onCurrentProjectOptionChecked( bool checked );

            void onRegularExpressionOptionChecked( bool checked );
            void onCaseSensitiveOptionChecked( bool checked );
            void onWordOptionChecked( bool checked );
            void onReverseSearchOptionChecked( bool checked );

            void onCancelButtonClicked();
            void onSearchButtonClicked();
            void onReplaceandSearchButtonClicked();
            void onReplaceAllButtonClicked();

        private:
            MainWindow* __mw;
            AdvancedSearch* __dialog;

            SearchModel* __searchModel;

            void __searchCurrentDoc();

            void __searchOpenedFiles();

            void __searchCurrentProject();
    };
}

#endif //O3PRM_NEW_SEARCH_CONTROLLER

