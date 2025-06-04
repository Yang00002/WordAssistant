#pragma once
#include "../lib/betterqt.hpp"

///
/// 单词形式
/// 0 0 0 0 a_a|a b|b -> basel
/// state 0 0 0 0 a_a|a b|b -> workl
/// a_a|a b|b -> iol
/// state	[0 0]		 [0 0]		 a_a|a			b|b
///			w/e time	w/e pass	wd/ea/ec		ed/wa

typedef bq::errorarray<QStringList> ERL;
extern ERL elog;

enum ER : uint8_t
{
	BaselineWrongEleCount,
	IolineWrongEleCount,
	TmplineWrongEleCount,
	WorklineWrongEleCount,
	LineWrongEleCount,
	WorklineWrongType,
	LineWrongTime,
	LineWrongPass,
	WordWrongEleCount,
	WordWrongEC,
	WordIllegal,
	ExpIllegal,
	ExpWrongEleCount,
	FileSaveFailed,
	AccidentQuitNew,
	RememberWrongW,
	RememberWrongE,
	RememberExt,
	Repeat,
	NOERROR,
	EMPTY,
	FERROR
};

typedef std::pair<bool, bq::errorarray<QStringList>> ERT;

ER UpdateBaselineFromFile();

ERL AddLineToFile(QString txt);

ERL AddTmplineToTmp(QString txt);

void LogAccidentQuit(QString txt, const ER& type);

const QString& ErrorMessage(const ER& type);

const QString& ErrorMessage(const int& type);

void initializeErrorText();

ER LoadWorkLineFromFile();

ER LoadBaseLineFromFile();

class WorkLine
{
public:
	enum Condition : uint8_t { Undefined, True, False };

private:
	bool type = false;
	int wtime = 0;
	int etime = 0;
	int wpass = 0;
	int epass = 0;
	QStringList wdetail;
	QStringList edetail;
	QStringList wanota;
	QStringList eanota;
	QString wordjoined;
	int wc = 0;
	int ec = 0;
	int wl = 0;
	int el = 0;
	bool right = false;
	bool ext = false;

public:
	ER initial(const QString& workline);
	ER initial(const QString& workline, bool wetype);
	[[nodiscard]] int getworkwaittime() const;
	[[nodiscard]] QString getworkshown() const;
	[[nodiscard]] QString getdoneshown() const;
	[[nodiscard]] const QString& getword() const;
	void submit(const QString& str);
	void submit(bool cond);
	void extract();
	void put();
	[[nodiscard]] bool isext() const;
	[[nodiscard]] bool isright() const;
	[[nodiscard]] QString getIoline() const;
	[[nodiscard]] QString getBaseline() const;
	[[nodiscard]] QString getWorkline() const;
	void applystate();
	[[nodiscard]] bool gettype() const;
};

class WordList
{
	QList<WorkLine> list;
	int moveindex = 0;
	int index = 0;
	int count = 0;

public:
	void add(const WorkLine& wl);
	void add(WorkLine&& wl);
	[[nodiscard]] const int* getcount() const;
	[[nodiscard]] const int* getindex() const;
	[[nodiscard]] const int* getmoveindex() const;
	void submit(const QString& str);
	void submit(bool cond);
	void pre();
	void next();
	WorkLine* getworkline();
	WorkLine* getmoveline();
	ERL onEnd();
	ERL onTmpEnd();
	void onAccidentQuit();
	void clear();
};

extern WordList rmblist;

ER GetLineCount(int& work, int& base);
