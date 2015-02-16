#ifndef O3PRM_SEARCH_MODEL_H
#define O3PRM_SEARCH_MODEL_H

#include <QObject>
#include <QHash>

#include "models/project.h"
#include "qsciscintillaextended.h"

namespace o3prm
{

    class NewSearchController;

    class SearchModel: public QObject
    {

        Q_OBJECT

        public:

            enum SearchOption
            {
                CurrentFile = 0,
                OpenedFile = 1,
                CurrentProject = 2
            };

            SearchModel(QObject* parent=0);

            ~SearchModel();

            SearchOption searchOption() const;
            void searchOption(SearchOption option);

            const QString& searchValue() const;
            void searchValue(const QString& value);

            const QString& replaceValue() const;
            void replaceValue(const QString& value);

            bool regularExpressionOption() const;
            void regularExpressionOption(bool option);

            bool caseSensitiveOption() const;
            void caseSensitiveOption(bool option);

            bool wordsOnlyOption() const;
            void wordsOnlyOption(bool option);

            bool reverseSearchOption() const;
            void reverseSearchOption(bool option);

        private:
            SearchOption __searchOption;

            QString __searchValue;
            QString __replaceValue;

            bool __regexOption;
            bool __caseSensitiveOption;
            bool __wordsOnlyOption;
            bool __reverseSearchOption;
    };

}
#endif // O3PRM_SEARCH_MODEL_H
