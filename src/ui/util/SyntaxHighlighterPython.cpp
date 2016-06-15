#include "SyntaxHighlighterPython.hpp"

pg::SyntaxHighlighterPython::SyntaxHighlighterPython(QTextDocument* parent):
	QSyntaxHighlighter(parent)
{
	Rule rule;

	formatKeyword.setForeground(Qt::blue);
	QStringList patternKeywords;

	for (auto&& keyword :
	        {"and", "as", "assert", "break", "class",
	        "continue", "def", "del", "elif", "else",
	        "except", "exec", "finally", "for", "from",
	        "global", "if", "import", "in", "is",
	        "lambda", "not", "or", "pass", "print",
	        "raise", "return", "try", "while", "with",
	        "yield"
	        })
	{
		patternKeywords << "\\b" + QString(keyword) + "\\b";
	}
	for (QString const& pattern: patternKeywords)
	{
		patternsKeyword.append(QRegExp(pattern));
	}

	// This regex is composed of two parts.
	// /"(?:[^"\\]|\\.)*"/ : Matches any double quoted string with escaped double
	// quotes
	// /'(?:[^'\\]|\\.)*'/ : Matches any single quoted string with escaped single
	// quotes
	// Four \'s are used to escape the slashes which are again escaped by the
	// regex.
	// Example: "aaa\"bbb", "aa'bb", 'aa\'bb"cc' are matched.
	ruleQuoted.pattern = QRegExp("\"(?:[^\"\\\\]|\\\\.)*\"|'(?:[^'\\\\]|\\\\.)*'");
	ruleQuoted.format.setForeground(Qt::darkGreen);

	ruleComment.pattern = QRegExp("#[^\n]*");
	ruleComment.format.setForeground(Qt::green);

}

void pg::SyntaxHighlighterPython::highlightBlock(QString const& text)
{
	for (auto const& pattern: patternsKeyword)
	{
		int index = pattern.indexIn(text);
		while (index >= 0)
		{
			int length = pattern.matchedLength();
			setFormat(index, length, formatKeyword);
			index = pattern.indexIn(text, index + length);
		}
	}
}
