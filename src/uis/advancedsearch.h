#ifndef O3PRM__ADVANCED_SEARCH_H
#define O3PRM__ADVANCED_SEARCH_H

#include <QDialog>

#include "ui_advancedsearch.h"

namespace o3prm
{

    class AdvancedSearch : public QDialog
    {
        Q_OBJECT

        public:
            explicit AdvancedSearch( QWidget *parent = 0 );
            ~AdvancedSearch();

            Ui::AdvancedSearch* ui() const;

        private:
            Ui::AdvancedSearch* __ui;
    };

} // o3prm

#endif // O3PRM__ADVANCED_SEARCH_H
