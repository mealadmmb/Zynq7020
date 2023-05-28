#pragma once
#include <QTextFile.h>
#include <QStringList.h>

#include <utility>



class QCsvFileWriter : private QTextFile {
public:
	class QCsvLine {
	public:
		explicit QCsvLine(QString separator = ",") : sep(std::move(separator)) {}
		~QCsvLine() = default;
		virtual QString get() const = 0;

	protected:
		const QString sep;
	};

	using QTextFile::isOpen;
	explicit QCsvFileWriter(const QString& _path, QStringList  _columnNames, QString  _separator = ",");
	QCsvFileWriter& operator<<(const QCsvLine& line);
private:
	void writeHeader();
	const QString separator;
	const QStringList columnNames;
};
