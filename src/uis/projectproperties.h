#ifndef O3PRM_PROJECTPROPERTIES_H
#define O3PRM_PROJECTPROPERTIES_H

#include "models/project.h"
#include "ui_projectproperties.h"

#include <QDialog>

namespace o3prm
{
    class ProjectProperties : public QDialog
    {
        Q_OBJECT

        public:
            explicit ProjectProperties( Project * p, QWidget *parent = 0 );
            ~ProjectProperties();

            protected slots:
                virtual void onAddButtonClicked();
            virtual void onDelButtonClicked();

            virtual void accept();
            virtual void reject();

        private:
            Ui::ProjectProperties *ui;
            Project * p;
            QWidget * parent;
    };
}

#endif // O3PRM_PROJECTPROPERTIES_H
