#include "../include/WAserver.hpp"

#include <QFile>

#include "../include/WASettings.hpp"
#include "../lib/betterc.hpp"
#include "../lib/RandHelp.hpp"

#define vsplit split(' ', bq::split::SkipEmptyParts)
#define lsplit split('|', bq::split::SkipEmptyParts)
#define log(etype,line) elog[etype].emplace_back(line)
#define mlog(etype,line) elog[etype].emplace_back(std::move(line))
#define logto(logger,etype,line) logger[etype].emplace_back(line)
#define mlogto(logger,etype,line) logger[etype].emplace_back(std::move(line))

ERL elog;

static qint64 getTimetoNow()
{
	auto days = WA::Settings->_lastupdatedate.daysTo(WA::Settings->_defaultdate);
	if (days < 0)
		return 0;
	else
		return days;
}

static ER cBaselineEleCount(const QStringList& list)
{
	const auto& size = list.size();
	if (size == 0)return EMPTY;
	if (size != 6)return BaselineWrongEleCount;
	return NOERROR;
}

static ER gLineTime(const QString& wstr, const QString& estr, int& wtime, int& etime)
{
	bool ok = false;
	wtime = wstr.toInt(&ok);
	ifn(ok) return LineWrongTime;
	if (wtime < 0)return LineWrongTime;
	etime = estr.toInt(&ok);
	ifn(ok) return LineWrongTime;
	if (etime < 0)return LineWrongTime;
	return NOERROR;
}

static ER cLineTime(const QString& wstr, const QString& estr)
{
	bool ok = false;
	int time = wstr.toInt(&ok);
	ifn(ok) return LineWrongTime;
	if (time < 0)return LineWrongTime;
	time = estr.toInt(&ok);
	ifn(ok) return LineWrongTime;
	if (time < 0)return LineWrongTime;
	return NOERROR;
}

static ER cLinePass(const QString& wstr, const QString& estr)
{
	bool ok = false;
	int time = wstr.toInt(&ok);
	ifn(ok) return LineWrongPass;
	if (time < 0)return LineWrongPass;
	time = estr.toInt(&ok);
	ifn(ok) return LineWrongPass;
	if (time < 0)return LineWrongPass;
	return NOERROR;
}

static ER gWordEleCount(const QStringList& word, int& count)
{
	count = static_cast<int>(word.size());
	if ((count < 1) || (count > 3))
		return WordWrongEleCount;
	return NOERROR;
}

static ER cWordEC(const QString& ecstr)
{
	bool ok = false;
	int count = ecstr.toInt(&ok);
	if ((ok == false) || (count <= 0))
		return WordWrongEC;
	return NOERROR;
}

[[maybe_unused]] static ER gExpEleCount(const QStringList& exp, int& count)
{
	count = static_cast<int>(exp.size());
	if ((count < 1) || (count > 2))
		return ExpWrongEleCount;
	return NOERROR;
}

[[maybe_unused]] static ER cIolineEleCount(const QStringList& list)
{
	const auto& size = static_cast<int>(list.size());
	if (size == 0)return EMPTY;
	if (size != 2)return IolineWrongEleCount;
	return NOERROR;
}

static ER cWorklineEleCount(const QStringList& list)
{
	const auto& size = list.size();
	if (size == 0)return EMPTY;
	if (size != 7)return IolineWrongEleCount;
	return NOERROR;
}

static ER cWordEleCount(const QStringList& word)
{
	int count = static_cast<int>(word.size());
	if ((count < 1) || (count > 3))
		return WordWrongEleCount;
	return NOERROR;
}

static ER cExpEleCount(const QStringList& exp)
{
	int count = static_cast<int>(exp.size());
	if ((count < 1) || (count > 2))
		return ExpWrongEleCount;
	return NOERROR;
}

static ER cWorklineType(const QString& state)
{
	int count = static_cast<int>(state.size());
	if (count == 0)return NOERROR;
	if (count == 1)return NOERROR;
	return WorklineWrongType;
}

static ER cWordDetail(const QString& word)
{
	if (word.front() == '_')return WordIllegal;
	if (word.back() == '_')return WordIllegal;
	QStringList l = word.split('_');
	for (auto& i : l)if (i.isEmpty())return WordIllegal;
	return NOERROR;
}

static ER cExpDetail(const QString& word)
{
	if (word.front() == '_')return ExpIllegal;
	if (word.back() == '_')return ExpIllegal;
	QStringList l = word.split('_');
	for (auto& i : l)if (i.isEmpty())return ExpIllegal;
	return NOERROR;
}

ER UpdateBaselineFromFile()
{
#if DEBUG_ == 1
	auto time = 1;
#else
	auto time = getTimetoNow();
#endif
	ifn(time)return EMPTY;
	QFile f(wa_datapath);
	ifn(bq::openFile(wa_datapath, f, bq::mode::ReadOnly | bq::mode::Text))return FERROR;
	QStringList resultlist;
	{
		QTextStream stream(&f);
		while (!stream.atEnd())
		{
			QString line = stream.readLine().trimmed();
			if (line.size())
			{
				QStringList ele = line.vsplit;
				ER ret = cBaselineEleCount(ele);
				if (ret != NOERROR)
				{
					if (ret == EMPTY)
						continue;
					else
					{
						mlog(ret, line);
						continue;
					}
				}
				int wetime[2] = {};
				ret = gLineTime(ele[0], ele[1], wetime[0], wetime[1]);
				if (ret != NOERROR)
				{
					mlog(ret, line);
					continue;
				}
				ele[0] = QString::number(wetime[0] + time);
				ele[1] = QString::number(wetime[1] + time);
				resultlist.emplace_back(ele.join(' '));
			}
		}
	}
	f.close();
	ifn(bq::openFile(wa_datapath, f, bq::mode::WriteOnly | bq::mode::Truncate | bq::mode::Text))return FERROR;
	QTextStream stream(&f);
	for (auto& i : resultlist)
	{
		stream << i << '\n';
	}
	f.close();
	WA::Settings->_lastupdatedate = WA::Settings->_defaultdate;
	return NOERROR;
}

static bq::errorarray<QString> errormessage;

void initializeErrorText()
{
	errormessage.resize(NOERROR);
	errormessage.eat(BaselineWrongEleCount, QObject::tr("ErrorBaselineWrongEleCount"));
	errormessage.eat(IolineWrongEleCount, QObject::tr("ErrorIolineWrongEleCount"));
	errormessage.eat(TmplineWrongEleCount, QObject::tr("ErrorTmplineWrongEleCount"));
	errormessage.eat(LineWrongEleCount, QObject::tr("ErrorLineWrongEleCount"));
	errormessage.eat(WorklineWrongEleCount, QObject::tr("ErrorWorklineWrongEleCount"));
	errormessage.eat(LineWrongTime, QObject::tr("ErrorLineWrongTime"));
	errormessage.eat(LineWrongPass, QObject::tr("ErrorLineWrongPass"));
	errormessage.eat(WorklineWrongType, QObject::tr("ErrorWorklineWrongType"));
	errormessage.eat(WordWrongEleCount, QObject::tr("ErrorWordWrongEleCount"));
	errormessage.eat(WordIllegal, QObject::tr("ErrorWordIllegal"));
	errormessage.eat(ExpIllegal, QObject::tr("ErrorExpIllegal"));
	errormessage.eat(WordWrongEC, QObject::tr("ErrorWordWrongEC"));
	errormessage.eat(ExpWrongEleCount, QObject::tr("ErrorExpWrongEleCount"));
	errormessage.eat(FileSaveFailed, QObject::tr("ErrorFileSaveFailed"));
	errormessage.eat(AccidentQuitNew, QObject::tr("ErrorAccidentQuitNew"));
	errormessage.eat(RememberWrongW, QObject::tr("ErrorRememberWrongW"));
	errormessage.eat(RememberWrongE, QObject::tr("ErrorRememberWrongE"));
	errormessage.eat(RememberExt, QObject::tr("ErrorRememberExt"));
	errormessage.eat(Repeat, QObject::tr("ErrorRepeat"));
}

const QString& ErrorMessage(const ER& type)
{
	return errormessage.get(type);
}

const QString& ErrorMessage(const int& type)
{
	return errormessage.get(type);
}

void LogAccidentQuit(QString txt, const ER& type)
{
	QTextStream stream(&txt);
	if (type == AccidentQuitNew)
	{
		while (!stream.atEnd())
		{
			QString line = stream.readLine().trimmed();
			if (line.size())
				elog[type].append(std::move(line));
		}
	}
}

class LineCmp
{
public:
	[[noreturn]] LineCmp(const LineCmp& other)
	{
		throw std::runtime_error("unexpected");
	}

	LineCmp& operator=(const LineCmp& other)
	{
		if (this == &other)
			return *this;
		throw std::runtime_error("unexpected");
	}

private:
	QString* _line = nullptr;
	bool _onstack = false;
	int _wordindex = 0;
	int _expindex = 0;
	int _wordsize = 0;
	int _expsize = 0;

	void Init(const int& ltype)
	{
		static int M[3] = {0, -8, -10};
		const QString& line = *_line;
		int state = M[ltype - 1];
		int size = static_cast<int>(line.size());
		_expsize = 0;
		for (int i = 0; i < size; i++)
		{
			switch (state)
			{
				case 0:
					{
						if (line[i] == '|')
							state++;
						else if (line[i] != ' ')
						{
							state += 2;
							_wordindex = i;
						}
						break;
					}
				case 4:
					{
						if (line[i] == '|')
							state++;
						else if (line[i] != ' ')
						{
							state += 2;
							_expindex = i;
						}
						break;
					}
				case -9:
				case -7:
				case -5:
				case -3:
				case -1:
				case 3:
					{
						if (line[i] == ' ')
						{
							state++;
						}
						break;
					}
				case 2:
					{
						if (line[i] == '|')
						{
							state++;
							_wordsize = i - _wordindex;
						}
						else if (line[i] == ' ')
						{
							state += 2;
							_wordsize = i - _wordindex;
						}
						break;
					}
				case -10:
				case -8:
				case -6:
				case -4:
				case -2:
					{
						if (line[i] != ' ')
							state++;
						break;
					}
				case 1:
					{
						if (line[i] != '|')
						{
							_wordindex = i;
							state++;
						}
						break;
					}
				case 5:
					{
						if (line[i] != '|')
						{
							_expindex = i;
							state++;
						}
						break;
					}
				case 6:
					{
						if (line[i] == '|' || line[i] == ' ')
						{
							_expsize = i - _expindex;
							goto END; // NOLINT(cppcoreguidelines-avoid-goto, hicpp-avoid-goto)
						}
						break;
					}
				default:
					break;
			}
		}
		if (_expsize == 0)
			_expsize = size - _expindex;
	END:;
	}

public:
	LineCmp() = default;

	LineCmp(const QString& line, int ltype)
	{
		if ((ltype >= 1) && (ltype <= 3))
		{
			_line = new QString(line);
			Init(ltype);
		}
	}

	LineCmp(LineCmp&& cmp) noexcept
	{
		_line = cmp._line;
		cmp._line = nullptr;
		_wordindex = cmp._wordindex;
		_expindex = cmp._expindex;
		_wordsize = cmp._wordsize;
		_expsize = cmp._expsize;
		_onstack = cmp._onstack;
	}

	LineCmp(QString* line, int ltype) noexcept
	{
		if ((ltype >= 1) && (ltype <= 3))
		{
			_line = line;
			Init(ltype);
			_onstack = true;
		}
	}

	~LineCmp()
	{
		clear();
	}

	LineCmp& operator=(LineCmp&& cmp) noexcept
	{
		if (_line != nullptr)
			clear();
		_line = cmp._line;
		cmp._line = nullptr;
		_wordindex = cmp._wordindex;
		_expindex = cmp._expindex;
		_wordsize = cmp._wordsize;
		_expsize = cmp._expsize;
		_onstack = cmp._onstack;
		return *this;
	}

	void set(const QString& line, int ltype)
	{
		if ((ltype >= 1) && (ltype <= 3))
		{
			if (_line)
				clear();
			_line = new QString(line);
			_onstack = false;
			Init(ltype);
		}
	}

	void eat(QString&& line, int ltype)
	{
		if ((ltype >= 1) && (ltype <= 3))
		{
			if (_line)
				clear();
			_line = new QString(std::move(line));
			_onstack = false;
			Init(ltype);
		}
	}

	void tie(QString* line, int ltype)
	{
		if ((ltype >= 1) && (ltype <= 3))
		{
			if (_line)
				clear();
			_line = line;
			_onstack = true;
			Init(ltype);
		}
	}

	bool operator==(const LineCmp& line) const
	{
		if (_line == nullptr)
			return false;
		if (line._line == nullptr)
			return false;
		if (_wordsize != line._wordsize)
			return false;
		else
		{
			const QString& sa = *_line;
			const QString& sb = *line._line;
			auto idxa = sa.begin() + _wordindex;
			auto idxb = sb.begin() + line._wordindex;
			auto ea = idxa + _wordsize;
			for (; idxa != ea; idxa++, idxb++)
			{
				if (*idxa != *idxb)
					return false;
			}
		}
		if (_expsize != line._expsize)
			return false;
		else
		{
			const QString& sa = *_line;
			const QString& sb = *line._line;
			auto idxa = sa.begin() + _expindex;
			auto idxb = sb.begin() + line._expindex;
			auto ea = idxa + _expsize;
			for (; idxa != ea; idxa++, idxb++)
			{
				if (*idxa != *idxb)
					return false;
			}
		}
		return true;
	}

	[[nodiscard]] bool totalsame(const LineCmp& line) const
	{
		if (_line == nullptr)
			return false;
		if (line._line == nullptr)
			return false;
		if (_wordsize != line._wordsize)
			return false;
		if (_expsize != line._expsize)
			return false;
		int len1 = static_cast<int>(_line->size()) - _wordindex;
		if (len1 != (line._line->size() - line._wordindex))return false;
		const QString& sa = *_line;
		const QString& sb = *line._line;
		auto idxa = sa.begin() + _wordindex;
		auto idxb = sb.begin() + line._wordindex;
		auto ea = idxa + len1;
		for (; idxa != ea; idxa++, idxb++)
		{
			if (*idxa != *idxb)
				return false;
		}
		return true;
	}

	[[nodiscard]] QString& str() const
	{
		return *(_line);
	}

	void clear()
	{
		if (_onstack == false)
		{
			delete _line;
		}
		_line = nullptr;
	}

	[[nodiscard]] inline bool empty() const
	{
		return _line == nullptr;
	}
};

namespace
{
	ERL getWorklineLWEFromFile(QList<LineCmp>& list)
	{
		ERL error;
		QFile f(wa_rememberpath);
		ifn(bq::openFile(wa_rememberpath, f, bq::mode::ReadOnly | bq::mode::Text))
		{
			error.settag(FERROR);
			return error;
		}
		QTextStream stream(&f);
		while (!stream.atEnd())
		{
			QString line = stream.readLine().trimmed();
			if (line.size())
			{
				QStringList ele = line.vsplit;
				ER res = cWorklineEleCount(ele);
				if (res != NOERROR)
				{
					if (res == EMPTY)
						continue;
					else
					{
						mlogto(error, res, line);
						continue;
					}
				}
				QStringList word = ele[5].lsplit;
				res = cWordEleCount(word);
				if (res != NOERROR)
				{
					mlogto(error, res, line);
					continue;
				}
				QStringList exp = ele[6].lsplit;
				res = cExpEleCount(exp);
				if (res != NOERROR)
				{
					mlogto(error, res, line);
					continue;
				}
				list.emplace_back(LineCmp(line, 3));
			}
		}
		f.close();
		if (error == false)error.settag(NOERROR);
		return error;
	}

	ERL getBaselineLWEFromFile(QList<LineCmp>& list)
	{
		ERL error;
		QFile f(wa_datapath);
		ifn(bq::openFile(wa_datapath, f, bq::mode::ReadOnly | bq::mode::Text))
		{
			error.settag(FERROR);
			return error;
		}
		QTextStream stream(&f);
		while (!stream.atEnd())
		{
			QString line = stream.readLine().trimmed();
			if (line.size())
			{
				QStringList ele = line.vsplit;
				ER res = cBaselineEleCount(ele);
				if (res != NOERROR)
				{
					if (res == EMPTY)
						continue;
					else
					{
						mlogto(error, res, line);
						continue;
					}
				}
				QStringList word = ele[4].lsplit;
				res = cWordEleCount(word);
				if (res != NOERROR)
				{
					mlogto(error, res, line);
					continue;
				}
				QStringList exp = ele[5].lsplit;
				res = cExpEleCount(exp);
				if (res != NOERROR)
				{
					mlogto(error, res, line);
					continue;
				}
				list.emplace_back(LineCmp(line, 2));
			}
		}
		f.close();
		if (error == false)error.settag(NOERROR);
		return error;
	}

	ERL RemoveRepeat(QList<LineCmp>& target, int ioc, int basec, int workc)
	{
		ERL error;
		QList<LineCmp> wlist;
		ERL we = getWorklineLWEFromFile(wlist);
		if (we.tag() == FERROR)
		{
			error.settag(FERROR);
			return error;
		}
		QList<LineCmp> blist;
		ERL be = getBaselineLWEFromFile(blist);
		if (be.tag() == FERROR)
		{
			error.settag(FERROR);
			return error;
		}
		if (ioc > 0)
		{
			QList<LineCmp>::iterator&& i = target.begin();
			QList<LineCmp>::iterator&& e = i + ioc;
			for (; i != e; ++i)
			{
				if (i->empty())
					continue;
				QList<LineCmp>::iterator&& j = i + 1;
				for (; j != e; ++j)
				{
					if (i->totalsame(*j))
					{
						j->clear();
					}
				}
				for (auto& k : blist)
				{
					if (i->totalsame(k))
					{
						i->clear();
						goto END0;
					}
				}
				for (auto& k : wlist)
				{
					if (i->totalsame(k))
					{
						i->clear();
						goto END0;
					}
				}
			END0:;
			}
		}
		if (basec > 0)
		{
			QList<LineCmp>::iterator&& i = target.begin() + ioc;
			QList<LineCmp>::iterator&& e = i + basec;
			for (; i != e; ++i)
			{
				if (i->empty())
					continue;
				QList<LineCmp>::iterator&& j = i + 1;
				for (; j != e; ++j)
				{
					if (i->totalsame(*j))
					{
						j->clear();
					}
				}
				for (auto& k : blist)
				{
					if (i->totalsame(k))
					{
						i->clear();
						goto END;
					}
				}
			END:;
			}
		}
		if (workc > 0)
		{
			QList<LineCmp>::iterator&& i = target.begin() + ioc + basec;
			QList<LineCmp>::iterator&& e = i + workc;
			for (; i != e; ++i)
			{
				if (i->empty())
					continue;
				QList<LineCmp>::iterator&& j = i + 1;
				for (; j != e; ++j)
				{
					if (i->totalsame(*j))
					{
						j->clear();
					}
				}
				for (auto& k : wlist)
				{
					if (i->totalsame(k))
					{
						i->clear();
						goto END1;
					}
				}
			END1:;
			}
		}
		bool wr = false, br = false;
		{
			auto e = target.end();
			auto i = target.begin();
			for (; i != e; ++i)
			{
				if (i->empty())
					continue;
				decltype(i) j = i + 1;
				bool repeat = false;
				for (; j != e; ++j)
				{
					if ((*i) == (*j))
					{
						logto(error, Repeat, j->str());
						j->clear();
						repeat = true;
					}
				}
				for (auto& k : wlist)
				{
					if ((*i) == k)
					{
						wr = true;
						mlogto(error, Repeat, k.str());
						logto(error, Repeat, i->str());
						k.clear();
						i->clear();
						goto END4;
					}
				}
				for (auto& k : blist)
				{
					if ((*i) == k)
					{
						br = true;
						mlogto(error, Repeat, k.str());
						logto(error, Repeat, i->str());
						k.clear();
						i->clear();
						goto END4;
					}
				}
				if (repeat)
				{
					logto(error, Repeat, i->str());
					i->clear();
					continue;
				}
			END4:;
			}
		}
		if (wr || (we.tag() != NOERROR))
		{
			QFile f(wa_rememberpath);
			ifn(bq::openFile(wa_rememberpath, f, bq::mode::WriteOnly | bq::mode::Truncate | bq::mode::Text))
			{
				error.clear();
				error.settag(FERROR);
				return error;
			}
			QTextStream stream(&f);
			for (auto& i : wlist)
			{
				if (i.empty() == false)
					stream << i.str() << '\n';
			}
			f.close();
			for (int i = we.getBegin(); i != -1; i = we.next(i))
			{
				elog[i].append(std::move(we[i]));
				we.clear();
			}
		}
		if (br || (be.tag() != NOERROR))
		{
			QFile f(wa_datapath);
			ifn(bq::openFile(wa_datapath, f, bq::mode::WriteOnly | bq::mode::Truncate | bq::mode::Text))
			{
				error.clear();
				error.settag(FERROR);
				return error;
			}
			QTextStream stream(&f);
			for (auto& i : blist)
			{
				if (i.empty() == false)
					stream << i.str() << '\n';
			}
			f.close();
			for (int i = be.getBegin(); i != -1; i = be.next(i))
			{
				elog[i].append(std::move(be[i]));
				be.clear();
			}
		}
		if (error == false)error.settag(NOERROR);
		return error;
	}

	ERL RemoveTmpRepeat(QList<LineCmp>& target)
	{
		ERL error;
		QList<LineCmp>::iterator&& i = target.begin();
		QList<LineCmp>::iterator&& e = target.end();
		for (; i != e; ++i)
		{
			if (i->empty())
				continue;
			QList<LineCmp>::iterator&& j = i + 1;
			for (; j != e; ++j)
			{
				if (i->totalsame(*j))
				{
					j->clear();
				}
			}
		}
		{
			auto ee = target.end();
			auto ii = target.begin();
			for (; ii != ee; ++ii)
			{
				if (ii->empty())
					continue;
				decltype(ii) j = ii + 1;
				bool repeat = false;
				for (; j != ee; ++j)
				{
					if ((*ii) == (*j))
					{
						logto(error, Repeat, j->str());
						j->clear();
						repeat = true;
					}
				}
				if (repeat)
				{
					logto(error, Repeat, ii->str());
					ii->clear();
					continue;
				}
			}
		}
		if (error == false)error.settag(NOERROR);
		return error;
	}
}


[[maybe_unused]] ERL AddLineToFile(QString txt)
{
	ERL error;
	ER ret{};
	ret = UpdateBaselineFromFile();
	QTextStream stream(&txt);
	if (ret == FERROR)
	{
		while (!stream.atEnd())
		{
			QString str = stream.readLine().trimmed();
			if (str.size())
			{
				mlog(FileSaveFailed, str);
			}
		}
		error.settag(FERROR);
		return error;
	}
	QStringList ioinput;
	QStringList baseinput;
	QStringList workinput;
	QList<LineCmp> inputhandler;
	int buffer = 0;
	while (!stream.atEnd())
	{
		QString line = stream.readLine().trimmed();
		if (line.size())
		{
			QStringList ele = line.vsplit;
			int esize = static_cast<int>(ele.size());
			if (esize > 0)
			{
				if (esize == 2)
				{
					QStringList det = ele[0].lsplit;
					ret = gWordEleCount(det, buffer);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cWordDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					if (buffer == 3)
					{
						ret = cWordEC(det[2]);
						if (ret != NOERROR)
						{
							mlogto(error, ret, line);
							continue;
						}
					}
					det = ele[1].lsplit;
					ret = cExpEleCount(det);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cExpDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ioinput.emplace_back(std::move(line));
					continue;
				}
				if (esize == 6)
				{
					ret = cLineTime(ele[0], ele[1]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cLinePass(ele[2], ele[3]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					QStringList det = ele[4].lsplit;
					ret = gWordEleCount(det, buffer);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cWordDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					if (buffer == 3)
					{
						ret = cWordEC(det[2]);
						if (ret != NOERROR)
						{
							mlogto(error, ret, line);
							continue;
						}
					}
					det = ele[5].lsplit;
					ret = cExpEleCount(det);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cExpDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					baseinput.emplace_back(std::move(line));
					continue;
				}
				if (esize == 7)
				{
					ret = cWorklineType(ele[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cLineTime(ele[1], ele[2]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cLinePass(ele[3], ele[4]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					QStringList det = ele[5].lsplit;
					ret = gWordEleCount(det, buffer);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cWordDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					if (buffer == 3)
					{
						ret = cWordEC(det[2]);
						if (ret != NOERROR)
						{
							mlogto(error, ret, line);
							continue;
						}
					}
					det = ele[6].lsplit;
					ret = cExpEleCount(det);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cExpDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					workinput.emplace_back(std::move(line));
					continue;
				}
				mlogto(error, LineWrongEleCount, line);
			}
		}
	}
	for (auto& i : ioinput)
	{
		inputhandler.emplace_back(LineCmp(&i, 1));
	}
	for (auto& i : baseinput)
	{
		inputhandler.emplace_back(LineCmp(&i, 2));
	}
	for (auto& i : workinput)
	{
		inputhandler.emplace_back(LineCmp(&i, 3));
	}
	ERL res = RemoveRepeat(inputhandler, static_cast<int>(ioinput.size()), static_cast<int>(baseinput.size()),
		static_cast<int>(workinput.size()));
	// res : 重复项
	// inputhandler: 所有好集合
	// error: 输入问题
	if (res.tag() == FERROR)
	{
		elog[FileSaveFailed].append(std::move(ioinput));
		elog[FileSaveFailed].append(std::move(baseinput));
		elog[FileSaveFailed].append(std::move(workinput));
		for (int i = error.getBegin(); i != -1; i = error.next(i))
		{
			elog[i].append(std::move(error[i]));
		}
		error.clear();
		error.settag(FERROR);
		return error;
	}

	// inputhandler: 所有好集合
	// error: 所有问题
	if (res.tag() != NOERROR)
	{
		for (int i = res.getBegin(); i != -1; i = res.next(i))
		{
			error[i].append(std::move(res[i]));
		}
		res.clear();
	}
	QFile f(wa_datapath);
	ifn(bq::openFile(wa_datapath, f, bq::mode::WriteOnly | bq::mode::Append | bq::mode::Text))
	{
		elog[FileSaveFailed].append(std::move(ioinput));
		elog[FileSaveFailed].append(std::move(baseinput));
		elog[FileSaveFailed].append(std::move(workinput));
		for (int i = error.getBegin(); i != -1; i = error.next(i))
		{
			elog[i].append(std::move(error[i]));
		}
		error.clear();
		error.settag(FERROR);
		return error;
	}
	QTextStream s3(&f);
	if (static_cast<int>(ioinput.size()))
	{
		auto i = inputhandler.begin();
		decltype(i) e = i + ioinput.size();
		for (; i != e; ++i)
		{
			if (i->empty() == false)
				s3 << "0 0 0 0 " << i->str() << '\n';
		}
	}
	if (static_cast<int>(baseinput.size()))
	{
		auto i = inputhandler.begin() + ioinput.size();
		decltype(i) e = i + baseinput.size();
		for (; i != e; ++i)
		{
			if (i->empty() == false)
				s3 << i->str() << '\n';
		}
	}
	f.close();
	QFile f1(wa_rememberpath);
	ifn(bq::openFile(wa_rememberpath, f1, bq::mode::WriteOnly | bq::mode::Append | bq::mode::Text))
	{
		elog[FileSaveFailed].append(std::move(workinput));
		for (int i = error.getBegin(); i != -1; i = error.next(i))
		{
			elog[i].append(std::move(error[i]));
		}
		error.clear();
		error.settag(FERROR);
		return error;
	}
	QTextStream s4(&f1);
	if (static_cast<int>(workinput.size()))
	{
		auto i = inputhandler.begin() + ioinput.size() + baseinput.size();
		decltype(i) e = inputhandler.end();
		for (; i != e; ++i)
		{
			if (i->empty() == false)
				s4 << i->str() << '\n';
		}
	}
	f1.close();
	if (error == false)error.settag(NOERROR);
	return error;
}


class BaseList
{
public:
	BaseList() = default;
	BaseList(const BaseList&) = delete;
	BaseList(BaseList&&) = delete;
	BaseList& operator=(const BaseList&) = delete;
	BaseList& operator=(BaseList&&) = delete;

	struct Node
	{
		QString line;
		int priority = 0;
		bool type = false;
		Node* next = nullptr;
	};

private:
	int _count = 0;
	int _maxsize = 0;
	int _minpriority = 0;
	int _maxpriority = 0;
	Node* _base = nullptr;
	Node* _end = nullptr;
	Node* _tmp = nullptr;

public:
	void setMax(const int& max)
	{
		_maxsize = max;
	}

	ER Add(QString& str, QStringList& swap)
	{
		if (str.size() == 0)return EMPTY;
		ifn(_tmp)_tmp = new Node;
		{
			QStringList element = str.vsplit;
			if (static_cast<int>(element.size()) == 0)return EMPTY;
			if (static_cast<int>(element.size()) != 6)return BaselineWrongEleCount;
			bool ok = false;
			int wtime = element[0].toInt(&ok);
			ifn(ok)return LineWrongTime;
			if (wtime < 0)return LineWrongTime;
			int etime = element[1].toInt(&ok);
			ifn(ok)return LineWrongTime;
			if (etime < 0)return LineWrongTime;
			int wpass = element[2].toInt(&ok);
			ifn(ok)return LineWrongPass;
			if (wpass < 0)return LineWrongPass;
			int epass = element[3].toInt(&ok);
			ifn(ok)return LineWrongPass;
			if (epass < 0)return LineWrongPass;
			int wp = wtime - WA::Settings->GetTime(wpass);
			int ep = etime - WA::Settings->GetTime(epass);
			if ((wp > ep) || ((wp == ep) && RandomBool()))
			{
				_tmp->type = false;
				_tmp->priority = wp;
			}
			else
			{
				_tmp->type = true;
				_tmp->priority = ep;
			}
		}
		const int& priority = _tmp->priority;
		if (priority < 0)
		{
			swap.emplace_back(std::move(str));
			return NOERROR;
		}
		if (_count < _maxsize)
		{
			_count++;
			if (_base == nullptr)
			{
				_tmp->line = std::move(str);
				_tmp->next = nullptr;
				_end = _tmp;
				_base = _tmp;
				_tmp = nullptr;
				_maxpriority = priority;
				_minpriority = priority;
				return NOERROR;
			}
			if (priority >= _maxpriority)
			{
				_tmp->line = std::move(str);
				_tmp->next = nullptr;
				_end->next = _tmp;
				_end = _tmp;
				_tmp = nullptr;
				_maxpriority = priority;
				return NOERROR;
			}
			if (priority <= _minpriority)
			{
				_tmp->line = std::move(str);
				_tmp->next = _base;
				_base = _tmp;
				_tmp = nullptr;
				_minpriority = priority;
				return NOERROR;
			}
			Node* p = _base;
			while (p->next)
			{
				if (p->next->priority >= priority)
				{
					_tmp->line = std::move(str);
					_tmp->next = p->next;
					p->next = _tmp;
					_tmp = nullptr;
					return NOERROR;
				}
				p = p->next;
			}
		}
		else
		{
			if (_count == 0)
			{
				swap.emplace_back(std::move(str));
				return NOERROR;
			}
			if (priority <= _minpriority)
			{
				swap.emplace_back(std::move(str));
				return NOERROR;
			}
			if (priority >= _maxpriority)
			{
				_tmp->line = std::move(str);
				_tmp->next = nullptr;
				_end->next = _tmp;
				_tmp = _base;
				_end = _end->next;
				_base = _base->next;
				_minpriority = _base->priority;
				_maxpriority = priority;
				swap.emplace_back(std::move(_tmp->line));
				return NOERROR;
			}
			Node* p = _base;
			while (p->next)
			{
				if (p->next->priority >= priority)
				{
					_tmp->line = std::move(str);
					_tmp->next = p->next;
					p->next = _tmp;
					_tmp = _base;
					_base = _base->next;
					_minpriority = _base->priority;
					swap.emplace_back(std::move(_tmp->line));
					return NOERROR;
				}
				p = p->next;
			}
		}
		return NOERROR;
	}

	[[nodiscard]] const int& count() const
	{
		return _count;
	}

	void run(const std::function<void(struct Node*)>& f) const
	{
		if (_count == 0)return;
		Node* p = _base;
		while (p != nullptr)
		{
			f(p);
			p = p->next;
		}
	}

	void randomrun(const std::function<void(struct Node*)>& f) const
	{
		if (_count == 0)return;
		Node** index = new Node*[_count];
		Node* p = _base;
		while (p != nullptr)
		{
			(*index) = p;
			p = p->next;
			index++;
		}
		index -= _count;
		int* rl = new int[_count];
		RandomIntList(rl, 0, _count - 1, _count, false);
		for (int i = 0; i < _count; i++)
		{
			f(index[rl[i]]);
		}
		delete[] index;
		delete[] rl;
	}

	~BaseList()
	{
		delete _tmp;
		if (_count != 0)
		{
			Node* p = _base;
			while (p != nullptr)
			{
				Node* pe = p;
				p = p->next;
				delete pe;
			}
		}
	}
};

WordList rmblist;

ERL AddTmplineToTmp(QString txt)
{
	ERL error;
	if (txt.isEmpty() == false)
	{
		ER ret{};
		QTextStream stream(&txt);
		QStringList ioinput;
		QStringList tmpinput;
		QList<bool> tmptag;
		int buffer = 0;
		while (!stream.atEnd())
		{
			QString line = stream.readLine().trimmed();
			if (line.size())
			{
				QStringList ele = line.vsplit;
				int esize = static_cast<int>(ele.size());
				if (esize == 0)continue;
				if (esize == 2)
				{
					QStringList det = ele[0].lsplit;
					ret = gWordEleCount(det, buffer);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cWordDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					if (buffer == 3)
					{
						ret = cWordEC(det[2]);
						if (ret != NOERROR)
						{
							mlogto(error, ret, line);
							continue;
						}
					}
					det = ele[1].lsplit;
					ret = cExpEleCount(det);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cExpDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ioinput.emplace_back(std::move(line));
					continue;
				}
				if (esize == 3)
				{
					bool type = false;
					buffer = ele[0].toInt(&type);
					ifn(type)
					{
						mlogto(error, WorklineWrongType, line);
						continue;
					}
					if ((buffer != 0) && (buffer != 1))
					{
						mlogto(error, WorklineWrongType, line);
						continue;
					}
					type = (buffer == 1);
					QStringList det = ele[1].lsplit;
					ret = gWordEleCount(det, buffer);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cWordDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					if (buffer == 3)
					{
						ret = cWordEC(det[2]);
						if (ret != NOERROR)
						{
							mlogto(error, ret, line);
							continue;
						}
					}
					det = ele[2].lsplit;
					ret = cExpEleCount(det);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ret = cExpDetail(det[0]);
					if (ret != NOERROR)
					{
						mlogto(error, ret, line);
						continue;
					}
					ele.removeAt(0);
					tmpinput.emplace_back(ele.join(' '));
					tmptag.emplace_back(type);
					continue;
				}
				mlogto(error, TmplineWrongEleCount, line);
			}
		}
		if ((ioinput.isEmpty() == false) || (tmpinput.isEmpty() == false))
		{
			QList<LineCmp> inputhandler;
			for (auto& i : ioinput)
			{
				inputhandler.emplace_back(LineCmp(&i, 1));
			}
			ERL res = RemoveTmpRepeat(inputhandler);
			if (res.tag() != NOERROR)
			{
				for (int i = res.getBegin(); i != -1; i = res.next(i))
				{
					error[i].append(std::move(res[i]));
				}
				res.clear();
			}

			int ihsize = static_cast<int>(inputhandler.size());
			int tmsize = static_cast<int>(tmpinput.size());
			int allsize = ihsize * 2 + tmsize;
			int ihdiv = ihsize * 2;
			int* ra = new int[allsize];
			RandomIntList(ra, 0, allsize - 1, allsize, false);
			for (int i = 0; i < allsize; i++)
			{
				auto j = ra[i];
				if (j < ihdiv)
				{
					if (j < ihsize)
					{
						auto& c = inputhandler[j];
						if (c.empty() == false)
						{
							WorkLine l;
							ret = l.initial("0 0 0 0 0 " + c.str());
							if (ret == EMPTY)continue;
							if (ret != NOERROR)
							{
								mlogto(error, ret, c.str());
								continue;
							}
							rmblist.add(std::move(l));
						}
					}
					else
					{
						auto& c = inputhandler[j - ihsize];
						if (c.empty() == false)
						{
							WorkLine l;
							ret = l.initial("1 0 0 0 0 " + c.str());
							if (ret == EMPTY)continue;
							if (ret != NOERROR)
							{
								mlogto(error, ret, c.str());
								continue;
							}
							rmblist.add(std::move(l));
						}
					}
				}
				else
				{
					auto& c = tmpinput[j - ihdiv];
					WorkLine l;
					if (tmptag[j - ihdiv])
						ret = l.initial("1 0 0 0 0 " + c);
					else
						ret = l.initial("0 0 0 0 0 " + c);
					if (ret == EMPTY)continue;
					if (ret != NOERROR)
					{
						mlogto(error, ret, c);
						continue;
					}
					rmblist.add(std::move(l));
				}
			}
			delete[] ra;
		}
	}
	if (error == false)error.settag(NOERROR);
	return error;
}

ER LoadWorkLineFromFile()
{
	QFile f(wa_rememberpath);
	ifn(bq::openFile(wa_rememberpath, f, bq::mode::ReadOnly | bq::mode::Text)) return FERROR;
	QTextStream s1(&f);
	WorkLine workline;
	while (!s1.atEnd())
	{
		QString line = s1.readLine().trimmed();
		ER res = workline.initial(line); // NOLINT(bugprone-use-after-move)
		if (res == EMPTY)
			continue;
		if (res == NOERROR)
		{
			rmblist.add(std::move(workline));
			continue;
		}
		mlog(res, line);
	}
	return NOERROR;
}

ER LoadBaseLineFromFile()
{
	int max = WA::Settings->_wordcount - *rmblist.getcount();
	if (max <= 0)return NOERROR;
	QFile f(wa_datapath);
	ifn(bq::openFile(wa_datapath, f, bq::mode::ReadOnly | bq::mode::Text)) return FERROR;
	QTextStream s1(&f);
	QStringList store;
	BaseList baselist;
	baselist.setMax(max);
	bool haveerr = false;
	while (!s1.atEnd())
	{
		QString line = s1.readLine().trimmed();
		ER res = baselist.Add(line, store);
		if (res != NOERROR)
		{
			mlog(res, line);
			haveerr = true;
			continue;
		}
	}
	f.close();
	if (baselist.count() != 0 || haveerr == true)
	{
		ifn(bq::openFile(wa_datapath, f, bq::mode::WriteOnly | bq::mode::Truncate))
		{
			elog[FileSaveFailed].append(std::move(store));
			baselist.run([](BaseList::Node* node)
			{
				mlog(FileSaveFailed, node->line);
			});
			return FERROR;
		}
		QTextStream s2(&f);
		for (auto& i : store)
			s2 << i << '\n';
		f.close();
	}
	baselist.randomrun([](BaseList::Node* node)
	{
		WorkLine l;
		ER res = l.initial(node->line, node->type);
		if (res == EMPTY)return;
		if (res != NOERROR)
		{
			mlog(res, node->line);
			return;
		}
		rmblist.add(std::move(l));
	});
	return NOERROR;
}

ER WorkLine::initial(const QString& workline)
{
	if (workline.size() == 0)return EMPTY;
	QStringList element = workline.vsplit;
	if (static_cast<int>(element.size()) == 0)return EMPTY;
	if (element.size() != 7)return WorklineWrongEleCount;
	bool ok = false;
	wtime = element[0].toInt(&ok);
	ifn(ok)return WorklineWrongType;
	if (wtime != 0 && wtime != 1)return WorklineWrongType;
	if (wtime)type = true;
	else type = false;
	wtime = element[1].toInt(&ok);
	ifn(ok)return LineWrongTime;
	if (wtime < 0)return LineWrongTime;
	etime = element[2].toInt(&ok);
	ifn(ok)return LineWrongTime;
	if (etime < 0)return LineWrongTime;
	wpass = element[3].toInt(&ok);
	ifn(ok)return LineWrongPass;
	if (wpass < 0)return LineWrongPass;
	epass = element[4].toInt(&ok);
	ifn(ok)return LineWrongPass;
	if (epass < 0)return LineWrongPass;
	QStringList word = element[5].lsplit;
	int size = static_cast<int>(word.size());
	if (size < 1 || size > 3)return WordWrongEleCount;
	wdetail = word[0].split('_', bq::split::SkipEmptyParts);
	wc = static_cast<int>(wdetail.size());
	wordjoined = wdetail.join(' ');
	if (size > 1)eanota = word[1].split('_', bq::split::SkipEmptyParts);
	if (size > 2)
	{
		ec = word[2].toInt(&ok);
		ifn(ok)return WordWrongEC;
	}
	else
		ec = 1;
	QStringList exp = element[6].lsplit;
	size = static_cast<int>(exp.size());
	if (size < 1 || size > 2)return ExpWrongEleCount;
	edetail = exp[0].split('_', bq::split::SkipEmptyParts);
	if (size > 1)wanota = exp[1].split('_', bq::split::SkipEmptyParts);
	wl = 0;
	for (auto& i : wdetail)
	{
		wl += static_cast<int>(i.size());
	}
	el = 0;
	for (auto& i : edetail)
	{
		el += static_cast<int>(i.size());
	}
	return NOERROR;
}

ER WorkLine::initial(const QString& workline, bool wetype)
{
	if (workline.size() == 0)return EMPTY;
	QStringList element = workline.vsplit;
	if (static_cast<int>(element.size()) == 0)return EMPTY;
	if (static_cast<int>(element.size()) != 6)return BaselineWrongEleCount;
	bool ok = false;
	type = wetype;
	wtime = element[0].toInt(&ok);
	ifn(ok)return LineWrongTime;
	if (wtime < 0)return LineWrongTime;
	etime = element[1].toInt(&ok);
	ifn(ok)return LineWrongTime;
	if (etime < 0)return LineWrongTime;
	wpass = element[2].toInt(&ok);
	ifn(ok)return LineWrongPass;
	if (wpass < 0)return LineWrongPass;
	epass = element[3].toInt(&ok);
	ifn(ok)return LineWrongPass;
	if (epass < 0)return LineWrongPass;
	QStringList word = element[4].lsplit;
	int size = static_cast<int>(word.size());
	if (size < 1 || size > 3)return WordWrongEleCount;
	wdetail = word[0].split('_', bq::split::SkipEmptyParts);
	wc = static_cast<int>(wdetail.size());
	wordjoined = wdetail.join(' ');
	if (size > 1)eanota = word[1].split('_', bq::split::SkipEmptyParts);
	if (size > 2)
	{
		ec = word[2].toInt(&ok);
		ifn(ok)return WordWrongEC;
	}
	else
		ec = 1;
	QStringList exp = element[5].lsplit;
	size = static_cast<int>(exp.size());
	if (size < 1 || size > 2)return ExpWrongEleCount;
	edetail = exp[0].split('_', bq::split::SkipEmptyParts);
	if (size > 1)wanota = exp[1].split('_', bq::split::SkipEmptyParts);
	wl = 0;
	for (auto& i : wdetail)
	{
		wl += static_cast<int>(i.size());
	}
	el = 0;
	for (auto& i : edetail)
	{
		el += static_cast<int>(i.size());
	}
	return NOERROR;
}

int WorkLine::getworkwaittime() const
{
	if (type)
		return static_cast<int>(WA::Settings->_wbasetime + static_cast<float>(wl) * WA::Settings->_wltime + static_cast<
			                        float>(wc) * WA::Settings->_wctime);
	return static_cast<int>(WA::Settings->_ebasetime + static_cast<float>(el) * WA::Settings->_eltime + static_cast<
		                        float>(ec) * WA::Settings->_ectime + static_cast<float>(wl) *
	                        WA::Settings
	                        ->
	                        _wrtime);
}

QString WorkLine::getworkshown() const
{
	if (type)
	{
		if (wanota.isEmpty())
			return edetail.join(' ');
		return edetail.join(' ') + '\n' + wanota.join(' ');
	}
	if (eanota.isEmpty())
		return wordjoined;
	return wordjoined + '\n' + eanota.join(' ');
}

QString WorkLine::getdoneshown() const
{
	return wordjoined + '\n' + edetail.join(' ');
}

const QString& WorkLine::getword() const
{
	return wordjoined;
}

void WorkLine::submit(const QString& str)
{
	if (str.isEmpty())
	{
		right = false;
		return;
	}
	QStringList l = str.vsplit;
	if (l.size() != wdetail.size())
	{
		right = false;
		return;
	}
	auto idx = l.begin();
	auto jdx = wdetail.begin();
	auto e = l.end();
	for (; idx != e; ++idx, ++jdx)
	{
		if ((*idx) != (*jdx))
		{
			right = false;
			return;
		}
	}
	right = true;
	return;
}

void WorkLine::submit(bool cond)
{
	if (cond)right = true;
	else right = false;
}

void WorkLine::extract()
{
	ext = true;
}

void WorkLine::put()
{
	ext = false;
}

bool WorkLine::isext() const
{
	return ext;
}

bool WorkLine::isright() const
{
	return right;
}

QString WorkLine::getIoline() const
{
	QString ret;
	if (static_cast<int>(eanota.size()))
	{
		if (ec != 1)
		{
			ret += QString("%1|%2|%3 ").arg(wdetail.join('_')).arg(eanota.join('_')).arg(ec);
		}
		else
		{
			ret += QString("%1|%2 ").arg(wdetail.join('_')).arg(eanota.join('_'));
		}
	}
	else
	{
		ret += wdetail.join('_') + ' ';
	}
	if (static_cast<int>(wanota.size()))
	{
		ret += QString("%1|%2").arg(edetail.join('_')).arg(wanota.join('_'));
	}
	else
	{
		ret += edetail.join('_');
	}
	return ret;
}

QString WorkLine::getBaseline() const
{
	QString ret = QString::asprintf("%d %d %d %d ", wtime, etime, wpass, epass);
	if (static_cast<int>(eanota.size()))
	{
		if (ec != 1)
		{
			ret += QString("%1|%2|%3 ").arg(wdetail.join('_')).arg(eanota.join('_')).arg(ec);
		}
		else
		{
			ret += QString("%1|%2 ").arg(wdetail.join('_')).arg(eanota.join('_'));
		}
	}
	else
	{
		ret += wdetail.join('_') + ' ';
	}
	if (static_cast<int>(wanota.size()))
	{
		ret += QString("%1|%2").arg(edetail.join('_')).arg(wanota.join('_'));
	}
	else
	{
		ret += edetail.join('_');
	}
	return ret;
}

QString WorkLine::getWorkline() const
{
	QString ret = QString::asprintf("%d %d %d %d %d ", type ? 1 : 0, wtime, etime, wpass, epass);
	if (static_cast<int>(eanota.size()))
	{
		if (ec != 1)
		{
			ret += QString("%1|%2|%3 ").arg(wdetail.join('_')).arg(eanota.join('_')).arg(ec);
		}
		else
		{
			ret += QString("%1|%2 ").arg(wdetail.join('_')).arg(eanota.join('_'));
		}
	}
	else
	{
		ret += wdetail.join('_') + ' ';
	}
	if (static_cast<int>(wanota.size()))
	{
		ret += QString("%1|%2").arg(edetail.join('_')).arg(wanota.join('_'));
	}
	else
	{
		ret += edetail.join('_');
	}
	return ret;
}

void WorkLine::applystate()
{
	if (right)
	{
		if (type)
		{
			epass++;
			etime = 0;
		}
		else
		{
			wpass++;
			wtime = 0;
		}
	}
	else
	{
		if (ext)
		{
			if (type)
			{
				if (etime > 0)
					etime--;
			}
			else
			{
				if (wtime > 0)
					wtime--;
			}
		}
		else
		{
			if (type)
			{
				epass = 0;
				etime = 0;
			}
			else
			{
				wpass = 0;
				wtime = 0;
			}
		}
	}
}

bool WorkLine::gettype() const
{
	return type;
}

void WordList::add(const WorkLine& wl)
{
	const QString& word = wl.getword();
	auto& controller = *WA::controller;
	list.emplace_back(wl);
	count++;
	if (controller.contain(word) == false)
	{
		controller.addWord(word);
	}
}

void WordList::add(WorkLine&& wl)
{
	QString word = wl.getword();
	auto& controller = *WA::controller;
	list.emplace_back(std::move(wl));
	count++;
	if (controller.contain(word) == false)
	{
		controller.addWord(word);
	}
}

const int* WordList::getcount() const
{
	return &count;
}

const int* WordList::getindex() const
{
	return &index;
}

const int* WordList::getmoveindex() const
{
	return &moveindex;
}

void WordList::submit(const QString& str)
{
	list[index].submit(str);
	moveindex = index;
	index++;
}

void WordList::submit(bool cond)
{
	list[index].submit(cond);
	moveindex = index;
	index++;
}

void WordList::pre()
{
	if (moveindex > 0)
		moveindex--;
}

void WordList::next()
{
	if (moveindex < count - 1)
		moveindex++;
}

WorkLine* WordList::getworkline()
{
	if (index < count)
		return &list[index];
	else
		return nullptr;
}

WorkLine* WordList::getmoveline()
{
	if (moveindex < count)
		return &list[moveindex];
	else
		return nullptr;
}

ERL WordList::onEnd()
{
	QStringList savelist;
	ERL error;
	for (auto& i : list)
	{
		i.applystate();
		if (i.isext())
		{
			error[RememberExt].emplace_back(i.getBaseline());
		}
		else
		{
			savelist.emplace_back(i.getBaseline());
			ifn(i.isright())
			{
				if (i.gettype())
					error[RememberWrongE].emplace_back("1 " + i.getIoline());
				else
					error[RememberWrongW].emplace_back("0 " + i.getIoline());
			}
			else
			{
				WA::Settings->_wordcount--;
			}
		}
	}
	clear();
	QFile f(wa_datapath);
	ifn(bq::openFile(wa_datapath, f, bq::mode::WriteOnly | bq::mode::Append))
	{
		elog[FileSaveFailed].append(std::move(error[RememberExt]));
		elog[FileSaveFailed].append(std::move(savelist));
		error.clear();
		error.settag(FERROR);
		return error;
	}
	QTextStream s(&f);
	for (auto& i : savelist)
	{
		s << i << '\n';
	}
	f.close();
	QFile f2(wa_rememberpath);
	ifn(bq::openFile(wa_rememberpath, f2, bq::mode::WriteOnly | bq::mode::Truncate))
	{
		elog[FileSaveFailed].append(std::move(error[RememberExt]));
		error.clear();
		error.settag(FERROR);
		return error;
	}
	f2.close();
	if (error == false)error.settag(NOERROR);
	return error;
}

ERL WordList::onTmpEnd()
{
	ERL error;
	for (auto& i : list)
	{
		ifn(i.isright())
		{
			if (i.gettype())
				error[RememberWrongE].emplace_back("1 " + i.getIoline());
			else
				error[RememberWrongW].emplace_back("0 " + i.getIoline());
		}
	}
	clear();
	if (error == false)error.settag(NOERROR);
	return error;
}

void WordList::onAccidentQuit()
{
	if (count == 0)return;
	QStringList savelist;
	QStringList backlist;
	auto i = list.begin();
	decltype(i) e = i + index;
	decltype(i) ed = list.end();
	for (; i < e; ++i)
	{
		i->applystate();
		if (i->isext())
		{
			log(RememberExt, i->getBaseline());
		}
		else
		{
			savelist.emplace_back(i->getBaseline());
			ifn(i->isright())
			{
				if (i->gettype())
					log(RememberWrongE, i->getIoline());
				else
					log(RememberWrongW, i->getIoline());
			}
			else
			{
				WA::Settings->_wordcount--;
			}
		}
	}
	for (; e < ed; ++e)
	{
		backlist.emplace_back(e->getWorkline());
	}
	clear();
	QFile f(wa_datapath);
	ifn(bq::openFile(wa_datapath, f, bq::mode::WriteOnly | bq::mode::Append))
	{
		elog[FileSaveFailed].append(std::move(savelist));
		elog[FileSaveFailed].append(std::move(backlist));
		return;
	}
	QTextStream s(&f);
	for (auto& ii : savelist)
	{
		s << ii << '\n';
	}
	f.close();
	QFile f2(wa_rememberpath);
	ifn(bq::openFile(wa_rememberpath, f2, bq::mode::WriteOnly | bq::mode::Truncate))
	{
		elog[FileSaveFailed].append(std::move(backlist));
		return;
	}
	QTextStream s2(&f2);
	for (auto& ii : backlist)
	{
		s2 << ii << '\n';
	}
	f2.close();
	return;
}

void WordList::clear()
{
	list.clear();
	moveindex = 0;
	index = 0;
	count = 0;
}

ER GetLineCount(int& work, int& base)
{
	{
		base = 0;
		QFile file(wa_datapath);
		ifn(bq::openFile(wa_datapath, file, bq::mode::ReadOnly | bq::mode::Text))
		{
			return FERROR;
		}
		QTextStream s(&file);
		while (!s.atEnd())
		{
			QString str = s.readLine().trimmed();
			if (str.isEmpty() == false)
			{
				if (str.vsplit.isEmpty() == false)
					base++;
			}
		}
		file.close();
	}
	{
		work = 0;
		QFile file(wa_rememberpath);
		ifn(bq::openFile(wa_rememberpath, file, bq::mode::ReadOnly | bq::mode::Text))
		{
			return FERROR;
		}
		QTextStream s(&file);
		while (!s.atEnd())
		{
			QString str = s.readLine().trimmed();
			if (str.isEmpty() == false)
			{
				if (str.vsplit.isEmpty() == false)
					work++;
			}
		}
		file.close();
	}
	return NOERROR;
}
