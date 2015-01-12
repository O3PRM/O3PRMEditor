#ifndef O3PRM_TEST_MOCK_QDIR
#define O3PRM_TEST_MOCK_QDIR

#include <QDir>

class MockQDir : public QDir {
    public:
        MOCK_METHOD1(setPath,
                void(const QString &path));
        MOCK_CONST_METHOD0(path,
                QString());
        MOCK_CONST_METHOD0(absolutePath,
                QString());
        MOCK_CONST_METHOD0(canonicalPath,
                QString());
        MOCK_CONST_METHOD0(dirName,
                QString());
        MOCK_CONST_METHOD1(filePath,
                QString(const QString &fileName));
        MOCK_CONST_METHOD1(absoluteFilePath,
                QString(const QString &fileName));
        MOCK_CONST_METHOD1(relativeFilePath,
                QString(const QString &fileName));
        MOCK_METHOD1(cd,
                bool(const QString &dirName));
        MOCK_METHOD0(cdUp,
                bool());
        MOCK_CONST_METHOD0(nameFilters,
                QStringList());
        MOCK_METHOD1(setNameFilters,
                void(const QStringList &nameFilters));
        MOCK_CONST_METHOD0(filter,
                Filters());
        MOCK_METHOD1(setFilter,
                void(Filters filter));
        MOCK_CONST_METHOD0(sorting,
                SortFlags());
        MOCK_METHOD1(setSorting,
                void(SortFlags sort));
        MOCK_CONST_METHOD0(count,
                uint());
        MOCK_CONST_METHOD2(entryList,
                QStringList(Filters, SortFlags));
        MOCK_CONST_METHOD3(entryList,
                QStringList(QStringList, Filters, SortFlags));
        MOCK_CONST_METHOD2(entryInfoList,
                QFileInfoList(Filters, SortFlags));
        MOCK_CONST_METHOD3(entryInfoList,
                QFileInfoList(QStringList, Filters, SortFlags));
        MOCK_CONST_METHOD1(mkdir,
                bool(const QString &dirName));
        MOCK_CONST_METHOD1(rmdir,
                bool(const QString &dirName));
        MOCK_CONST_METHOD1(mkpath,
                bool(const QString &dirPath));
        MOCK_CONST_METHOD1(rmpath,
                bool(const QString &dirPath));
        MOCK_CONST_METHOD0(isReadable,
                bool());
        MOCK_CONST_METHOD0(exists,
                bool());
        MOCK_CONST_METHOD0(isRoot,
                bool());
        MOCK_CONST_METHOD0(isRelative,
                bool());
        MOCK_CONST_METHOD0(isAbsolute,
                bool());
        MOCK_METHOD0(makeAbsolute,
                bool());
        MOCK_METHOD1(remove,
                bool(const QString &fileName));
        MOCK_METHOD2(rename,
                bool(const QString &oldName, const QString &newName));
        MOCK_CONST_METHOD1(exists,
                bool(const QString &name));
        MOCK_CONST_METHOD0(refresh,
                void());
};

#endif // O3PRM_TEST_MOCK_QDIR
