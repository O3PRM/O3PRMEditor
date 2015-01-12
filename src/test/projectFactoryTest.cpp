#include "gmock/gmock.h"

#include <QString>

#include "models/project.h"
#include "models/projectFactory.h"

#include "test/mockdir.h"

class MockFileEngine
{

};


class MockFile
{

};

namespace o3prm
{
    template <class QDir>
    class ProjectFactory
    {

        public:
            Project* newProject(const QDir& dir, QString name)
            {
                auto project = new Project(dir.absolutePath(), name);
                auto file = dir.absoluteFilePath(name + ".o3prmproject");
                return project;
            }
    };
}

TEST(ProjectManagement, NewProject) 
{
    // Arrange
    o3prm::ProjectFactory<MockQDir> factory;
    QString name("MyNewProject");
    QString filename = name + ".o3prmproject";
    QString root("/home/lto/o3prm/");
    MockQDir dir;
    EXPECT_CALL(dir, absolutePath())
        .WillOnce(::testing::Return(root));
    EXPECT_CALL(dir, absoluteFilePath(::testing::_))
        .WillOnce(::testing::Return(root + filename));
    // Act
    auto project = factory.newProject(dir, name);
    // Assert
    ASSERT_NE((o3prm::Project*)0, project);
    ASSERT_EQ(name, project->name());
}
