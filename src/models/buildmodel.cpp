#include "models/buildmodel.h"

namespace o3prm
{

    BuildModel::BuildModel(Project* project, QObject* parent):
        QObject(parent), __project(project)
    {

    }

    BuildModel::~BuildModel()
    {

    }

    gum::prm::o3prmr::O3prmrInterpreter* BuildModel::build(ProjectItem* item)
    {
        auto interpreter = new gum::prm::o3prmr::O3prmrInterpreter();
        interpreter->addPath(__project->dir().absolutePath().toStdString() + '/');
        auto path = __project->dir().absoluteFilePath(item->path());
        interpreter->interpretFile(path.toStdString());
        return interpreter;
    }


} // o3prm
