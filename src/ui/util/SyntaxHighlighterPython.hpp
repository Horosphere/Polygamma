#ifndef _POLYGAMMA_UI_UTIL_SYNTAXHIGHLIGHTERPYTHON_HPP__
#define _POLYGAMMA_UI_UTIL_SYNTAXHIGHLIGHTERPYTHON_HPP__

#include <QSyntaxHighlighter>

namespace pg
{

class SyntaxHighlighterPython final: QSyntaxHighlighter
{
	Q_OBJECT
public:
	SyntaxHighlighterPython(QTextDocument* parent);

protected:
	virtual void highlightBlock(QString const&) override;

private:

	struct Rule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};

	QVector<QRegExp> patternsKeyword;
	QTextCharFormat formatKeyword;

	Rule ruleQuoted;
	Rule ruleComment;
};

} // namespace pg

#endif // !_POLYGAMMA_UI_UTIL_SYNTAXHIGHLIGHTERPYTHON_HPP__
