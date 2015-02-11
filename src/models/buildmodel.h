#ifndef O3PRM_BUILD_MODEL_H
#define O3PRM_BUILD_MODEL_H

#include <QObject>

#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

#include "models/project.h"

namespace o3prm
{

    class BuildModel: public QObject
    {
        Q_OBJECT

        public:
            BuildModel(Project* project, QObject* parent=0);

            ~BuildModel();

            /*!
              Returns the O3prmInterpreter builded from item.
              Item should contain a request.
             */
            gum::prm::o3prmr::O3prmrInterpreter* build(ProjectItem* item);

        private:
            Project* __project;
    };

}
#endif // O3PRM_BUILD_MODEL_H
