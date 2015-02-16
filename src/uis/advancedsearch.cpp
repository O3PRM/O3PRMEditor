#include "advancedsearch.h"
#include "ui_advancedsearch.h"

namespace o3prm
{

    AdvancedSearch::AdvancedSearch( QWidget *parent ) :
        QDialog( parent ),
        __ui( new Ui::AdvancedSearch() )
    {
        __ui->setupUi( this );
        //this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }

    AdvancedSearch::~AdvancedSearch() 
    {
    }

    Ui::AdvancedSearch* AdvancedSearch::ui() const
    {
        return __ui;
    }

} //o3prm

