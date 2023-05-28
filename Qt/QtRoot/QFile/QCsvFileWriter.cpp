#include <QCsvFileWriter.h>
#include <utility>



QCsvFileWriter::QCsvFileWriter(const QString& _path, QStringList   _columnNames, QString  _separator) :
QTextFile(_path),
separator(std::move(_separator)),
columnNames(std::move(_columnNames))
{
	QTextFile::open(QTextFile::QFileMode::Append);
	if(isOpen() && isEmpty())
		writeHeader();
}

QCsvFileWriter &QCsvFileWriter::operator<<(const QCsvFileWriter::QCsvLine &line) {
	QTextFile::writeLine(line.get());
	return *this;
}

void QCsvFileWriter::writeHeader() {
	QString header;
	header << "SEP=" << separator << "\n";
	for(size_t i = 0; i < columnNames.size(); i++) {
		header << columnNames[i];
		if(i < columnNames.size() - 1)
			header << separator;
	}
	QTextFile::writeLine(header);
}
