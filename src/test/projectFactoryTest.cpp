#include "gmock/gmock.h"

#include <QString>
#include <QFileDialog>

#include "models/project.h"
#include "models/projectFactory.h"

#include "test/mockqdir.h"


class MockQFileDialog {

    public:
        static QString getExistingDirectory ( QWidget * parent = 0,
                const QString & caption = QString(),
                const QString & dir = QString(),
                QFileDialog::Options options = QFileDialog::ShowDirsOnly )
        {
            return singleton().getExistingDirectoryImpl(parent, caption, dir, options);
        }

        MOCK_METHOD4(getExistingDirectoryImpl,
                     QString( QWidget * parent, const QString & caption, const QString & dir, QFileDialog::Options options));

        static MockQFileDialog& singleton()
        {
            return *__singleton;
        }

    private:
        static MockQFileDialog *__singleton;
};

MockQFileDialog* MockQFileDialog::__singleton = new MockQFileDialog();

TEST(ProjectFactory, NewProject) 
{
    // Arrange
    o3prm::ProjectFactory<MockQFileDialog> factory;

    QDir dir("/home/lto/o3prm/MyNewProject/");
    QString name = "MyNewProject";
    EXPECT_CALL(MockQFileDialog::singleton(), getExistingDirectoryImpl(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(dir.absolutePath()));
    // Act
    auto project = factory.newProject();
    // Assert
    ASSERT_NE((o3prm::Project*)0, project);
    ASSERT_EQ(name.toStdString(), project->name().toStdString());
    ASSERT_EQ(dir.absolutePath().toStdString(), project->dir().absolutePath().toStdString());
}

