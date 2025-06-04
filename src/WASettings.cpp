#include "../include/WASettings.hpp"

#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "../include/WAserver.hpp"
#include "../lib/betterqt.hpp"

WASettings::WASettings()
{
	_settings = new QSettings(wa_settingspath, QSettings::IniFormat);
	_settings->beginGroup("common");
	QDate date = QDate::fromString(_settings->value("time", _defaultdate.toString(Qt::ISODate)).toString(),
	                               Qt::ISODate);
	if (date.isValid() == false)
	{
		_lastupdatedate = _defaultdate;
	}
	else
	{
		_lastupdatedate = date;
	}
	bool ok = false;
	_defaultwordcount = _settings->value("defaultwordcount", "200").toInt(&ok);
	ifn(ok || _defaultwordcount < 0)_defaultwordcount = 200;
	_wbasetime = _settings->value("wbasetime", "8").toFloat(&ok);
	ifn(ok || _wbasetime < 0)_wbasetime = 8;
	_ebasetime = _settings->value("ebasetime", "8").toFloat(&ok);
	ifn(ok || _ebasetime < 0)_ebasetime = 8;
	_wltime = _settings->value("wltime", "0.5").toFloat(&ok);
	ifn(ok || _wltime < 0)_wltime = 0.5f;
	_eltime = _settings->value("eltime", "0.1").toFloat(&ok);
	ifn(ok || _eltime < 0)_eltime = 0.5f;
	_wctime = _settings->value("wctime", "0.5").toFloat(&ok);
	ifn(ok || _wctime < 0)_wctime = 0.5f;
	_ectime = _settings->value("ectime", "2").toFloat(&ok);
	ifn(ok || _ectime < 0)_ectime = 0.5f;
	_wrtime = _settings->value("wrtime", "0.1").toFloat(&ok);
	ifn(ok || _wrtime < 0)_wrtime = 0.1f;
	QStringList str = _settings->value("wordcount",
	                                   _defaultdate.toString(Qt::ISODate) + " " + QString::number(_defaultwordcount)).
	                             toString().split(' ', bq::split::SkipEmptyParts);
	if (str.size() != 2)
	{
		_wordcount = _defaultwordcount;
	}
	else
	{
		QDate d = QDate::fromString(str[0], Qt::ISODate);
		if (d.isValid() == false || d != _defaultdate)
		{
			_wordcount = _defaultwordcount;
		}
		else
		{
			bool ok1 = false;
			_wordcount = str[1].toInt(&ok1);
			if (ok1 == false)
			{
				_wordcount = _defaultwordcount;
			}
		}
	}
	QString s = _settings->value("waittime", "-1 1 2 4 8 16 32 64").toString();
	TimeInit(s);
	_settings->endGroup();
}

WASettings::~WASettings()
{
	_settings->beginGroup("common");
	_settings->setValue("time", _lastupdatedate.toString(Qt::ISODate));
	_settings->setValue("defaultwordcount", QString::number(_defaultwordcount));
	_settings->setValue("wordcount", _defaultdate.toString(Qt::ISODate) + " " + QString::number(_wordcount));
	_settings->setValue("wbasetime", QString::number(_wbasetime));
	_settings->setValue("ebasetime", QString::number(_ebasetime));
	_settings->setValue("wltime", QString::number(_wltime));
	_settings->setValue("eltime", QString::number(_eltime));
	_settings->setValue("wctime", QString::number(_wctime));
	_settings->setValue("ectime", QString::number(_ectime));
	_settings->setValue("wrtime", QString::number(_wrtime));
	_settings->setValue("waittime", GetTimeStr());
	_settings->endGroup();
	delete _settings;
	delete _timelist;
}

void WASettings::TimeInit(const QString& str)
{
	if (str.size() != 0)
	{
		QStringList l = str.split(' ', bq::split::SkipEmptyParts);
		if (_timelen != 0)
		{
			_timelen = static_cast<int>(l.size());
			_timelist = new int[_timelen];
			bool ok = false;
			for (int i = 0; i < _timelen; i++)
			{
				_timelist[i] = l[i].toInt(&ok);
				ifn(ok)break;
			}
			if (ok)
			{
				for (int i = 0; i < _timelen - 1; i++)
				{
					if (_timelist[i] > _timelist[i + 1])
					{
						ok = false;
						break;
					}
				}
				if (ok)return;
			}
			delete _timelist;
		}
	}
	_timelen = 8;
	_timelist = new int[8];
	_timelist[0] = -1;
	_timelist[1] = 1;
	for (int i = 1; i < 7; i++)
	{
		_timelist[i + 1] = _timelist[i] << 1;
	}
}

QString WASettings::GetTimeStr() const
{
	QString ret = QString::number(_timelist[0]);
	for (int i = 1; i < _timelen; i++)
		ret += ' ' + QString::number(_timelist[i]);
	return ret;
}

int WASettings::GetTime(int pass) const
{
	if (pass < 0)return 0;
	if (pass < _timelen)return _timelist[pass];
	return _timelist[_timelen - 1];
}

static bool canReadWriteFile(const char* path)
{
	QFileInfo fi(path);
	if (fi.exists() == false)
	{
		QFile f(path);
		if (f.open(bq::mode::NewOnly) == false)
			return false;
		f.close();
		fi.refresh();
	}
	if ((fi.isReadable() && fi.isWritable()) == false)
		return false;
	return true;
}

void WA::Init()
{
	QDir dir = QDir::current();
	if (dir.exists() == false)
		exit(-1);
	{
		if (dir.exists(wa_dirdata) == false)
		{
			if (dir.mkdir(wa_dirdata) == false)
				exit(-1);
		}
	}
	{
		if (dir.exists(wa_dirlog) == false)
		{
			if (dir.mkdir(wa_dirlog) == false)
				exit(-1);
		}
	}
	{
		if (dir.exists(wa_dirasset) == false)
		{
			if (dir.mkdir(wa_dirasset) == false)
				exit(-1);
		}
	}
	{
		if (canReadWriteFile(wa_datapath) == false)
			exit(-1);
		if (canReadWriteFile(wa_rememberpath) == false)
			exit(-1);
		if (canReadWriteFile(wa_settingspath) == false)
			exit(-1);
	}
	initializeErrorText();
	Settings = new WASettings();
	vico = new QIcon(wa_videopath);
	if (vico->isNull())
		exit(-1);
	manager = new QNetworkAccessManager;
	controller = new AudioController;
}

void WA::End()
{
	elog.writelog(wa_dirlog, Settings->_defaultdate, [](const int& idx, const QStringList& er)-> QString
	{
		QString ret = ErrorMessage(idx) + "\n\t";
		ret.append(er.join("\n\t"));
		return ret;
	});
	delete Settings;
	delete vico;
	delete manager;
	delete controller;
}

WASettings* WA::Settings = nullptr;

QIcon* WA::vico = nullptr;

QNetworkAccessManager* WA::manager = nullptr;

AudioController* WA::controller = nullptr;
