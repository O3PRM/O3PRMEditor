#include "models/buildmodel.h"

#include <QPair>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>
#include <agrum/BN/io/BIF/BIFWriter.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

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

    void BuildModel::skeleton(ProjectItem* item, QList<QPair<QString, QString>> &result)
    {
        auto path = __project->dir().absoluteFilePath(item->path());
        gum::prm::o3prm::O3prmReader<double> reader;
        QString package = item->path().replace("/", ".");
        reader.readFile(path.toStdString(), package.toStdString());
        auto prm = reader.prm();
        
        if (not prm->systems().empty())
        {
          for ( auto sys: prm->systems() ) {
            QString name = QString::fromStdString(sys->name());
            QString skeleton = __skeleton(*sys);
            auto pair = qMakePair(name, skeleton);
            result.push_back(pair);
          }
        }

        delete prm;
    }

    QString BuildModel::__skeleton(gum::prm::System<double> &sys) 
    {
      QString sk = "graph " + QString::fromStdString(sys.name()).replace('.', '_') + " {";
    
      // List nodes
      for (const auto node: sys.skeleton().nodes())
      {
        const auto &inst = sys.get(node);
        const auto &c = inst.type();
        QString line = "\n%1[label=\"%2 %3\"]";
        sk.append(line.arg(QString::number(node), QString::fromStdString(c.name()), QString::fromStdString(inst.name())));
      }

      sk.append("\n");

      // list arcs
      for (const auto node: sys.skeleton().nodes())
      {
        const auto &inst = sys.get(node);
        const auto &c = inst.type();
        for (auto ref: c.referenceSlots())
        {

          for (auto iter = inst.begin(ref->id()); not iter.isEnd(); iter.operator++())
          {
            QString line = "\n\t%1 -- %2";
            sk.append(line.arg(QString::number(node), QString::number(sys.get(*iter))));
          }

        }      
      }
      sk.append("\n}");
      return sk;
    }

    void BuildModel::exportAsBIF(ProjectItem* item, QList<QPair<QString, QString>> &result) 
    {
      auto path = __project->dir().absoluteFilePath(item->path());
      gum::prm::o3prm::O3prmReader<double> reader;
      reader.addClassPath( __project->dir().absolutePath().toStdString() );
      QString package = item->path().replace("/", ".");
      reader.readFile(path.toStdString(), package.toStdString());
      auto prm = reader.prm();

      reader.showElegantErrorsAndWarnings();

      for ( auto sys: prm->systems() ) {
        gum::BayesNet<double> bn;
        gum::BayesNetFactory<double> bn_fact(&bn);
        sys->groundedBN(bn_fact);

        gum::BIFWriter<double> writer;
        std::stringstream str;

        writer.write(str, bn);

        auto name = QString::fromStdString(sys->name());
        auto ground = QString::fromStdString( str.str() );
        auto pair = qMakePair(name, ground);

        result.push_back(pair);
      }

      delete prm;
    }

} // o3prm
