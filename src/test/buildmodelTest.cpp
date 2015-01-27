#include "gmock/gmock.h"

#include <iostream>

#include <QFile>
#include <QtXml>

#include "models/project.h"
#include "models/buildmodel.h"

class BuildModelTest : public ::testing::Test {
    protected:
        virtual void SetUp()
        {
            QFile file("/Users/lto/Projets/o3prm/SimplePRM/SimplePRM.o3prmproject");
            QDomDocument doc("o3prmproject");
            if( doc.setContent( &file ) )
            {
                QFileInfo info(file);
                auto type = o3prm::ProjectItem::ItemType::Project;
                auto tag = o3prm::Project::itemType2String(type).toLower();
                auto projects = doc.elementsByTagName(tag);
                auto node = projects.at(0);
                auto elt = static_cast<QDomElement*>(&node);
                _project = o3prm::Project::load(info.absoluteDir().absolutePath(), *elt);
            }

            
        }

        virtual void TearDown()
        {
            if (_project)
            {
                delete _project;
                _project = 0;
            }
        }

        o3prm::Project* _project;
};


TEST_F(BuildModelTest, LoadProject)
{
    ASSERT_EQ(_project->root()->child(0)->text().toStdString(), "SimpleClass.o3prm");
    ASSERT_EQ(_project->root()->child(1)->text().toStdString(), "SimpleSystem.o3prm");
    ASSERT_EQ(_project->root()->child(2)->text().toStdString(), "SimpleRequest.o3prm");
}

TEST_F(BuildModelTest, LoadClassFile)
{
    // Arrange
    o3prm::BuildModel build(_project);
    // Act
    auto interpreter = build.build(_project->root()->child(2));
    // Assert
    ASSERT_TRUE(interpreter);
    auto errors = interpreter->errorsContainer();
    for (int i = 0; i < errors.count(); ++i)
    {
        auto parse_error = errors.error(i);
        std::cout << parse_error.msg << std::endl;
    }
    ASSERT_FALSE(interpreter->errors());
    ASSERT_TRUE(interpreter->results().size());
    for (size_t i = 0; i < interpreter->results().size(); ++i)
    {
        auto result = interpreter->results()[i];
        std::cout << result.command << ": " << std::endl;
        for (size_t j = 0; j < result.values.size(); ++j)
        {
            auto value = result.values[j];
            std::cout << "  " << value.label << ": " << value.p << std::endl;
        }
    }
}

