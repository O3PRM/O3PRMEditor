#ifndef O3PRM_MENU_CONTROLLER_H
#define O3PRM_MENU_CONTROLLER_H

#include <QObject>

class MainWindow;

namespace o3prm
{

    class MenuController: public QObject
    {
        Q_OBJECT

        public:
            explicit MenuController(MainWindow* mw, QObject* parent=0);
            ~MenuController();

            void setupConnections();

        public slots:
            void setProjectExploratorVisibility( bool visible );

        private slots:
            void setStatusBarVisibility( bool visible );
            void setToolBarVisibility( bool visible );

            void increaseZoom();
            void decreaseZoom();
            void normalZoom();

            void setLineNumbersVisibility( bool checked );
            void setBookmarksVisibility( bool checked );
            void setUnprintableCharactersVisibility( bool checked );

        private:
            MainWindow* __mw;
    };

} // O3PRM

#endif // O3PRM_MENU_CONTROLLER_H
