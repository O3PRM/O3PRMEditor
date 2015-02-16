#include "models/searchmodel.h"

namespace o3prm
{

    SearchModel::SearchModel(QObject* parent):
        QObject(parent)
    {
        __searchOption = CurrentFile;
        __searchValue = "";
        __replaceValue = "";

        __regexOption = false;
        __caseSensitiveOption = false;
        __wordsOnlyOption = false;
        __reverseSearchOption = false;
    }

    SearchModel::~SearchModel()
    {

    }

    SearchModel::SearchOption SearchModel::searchOption() const
    {
        return __searchOption;
    }

    void SearchModel::searchOption(SearchOption option)
    {
        __searchOption = option;
    }

    const QString& SearchModel::searchValue() const
    {
        return __searchValue;
    }

    void SearchModel::searchValue(const QString& value)
    {
        __searchValue = value;
    }

    const QString& SearchModel::replaceValue() const
    {
        return __replaceValue;
    }

    void SearchModel::replaceValue(const QString& value)
    {
        __replaceValue = value;
    }

    bool SearchModel::regularExpressionOption() const
    {
        return __regexOption;
    }

    void SearchModel::regularExpressionOption(bool option)
    {
        __regexOption = option;
    }

    bool SearchModel::caseSensitiveOption() const
    {
        return __caseSensitiveOption;
    }

    void SearchModel::caseSensitiveOption(bool option)
    {
        __caseSensitiveOption = option;
    }

    bool SearchModel::wordsOnlyOption() const
    {
        return __wordsOnlyOption;
    }

    void SearchModel::wordsOnlyOption(bool option)
    {
        __wordsOnlyOption = option;
    }

    bool SearchModel::reverseSearchOption() const
    {
        return __reverseSearchOption;
    }

    void SearchModel::reverseSearchOption(bool option)
    {
        __reverseSearchOption = option;
    }

}
