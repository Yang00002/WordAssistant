#pragma once
#include <QNetworkAccessManager>

#include "AudioController.hpp"

#define wa_dirdata  "./data"
#define wa_dirlog  "./log"
#define wa_dirasset "./assets"
#define wa_datapath (wa_dirdata  "/word.txt")
#define wa_rememberpath (wa_dirdata  "/remember.txt")
#define wa_settingspath (wa_dirdata  "/cache.ini")
#define wa_videopath (wa_dirasset  "/video.png")

#define wa_audioapi "http://dict.youdao.com/dictvoice?audio="

class QSettings;

namespace WA
{
	void Init();
	void End();
	extern QIcon* vico;
	extern QNetworkAccessManager* manager;
	extern AudioController* controller;
}

class WASettings
{
	friend void WA::Init();
	friend void WA::End();

private:
	QSettings* _settings = nullptr;
	WASettings();
	~WASettings();
	int _timelen = 0;
	int* _timelist = nullptr;
	int _defaultwordcount = 0;
	void TimeInit(const QString& str);
	[[nodiscard]] QString GetTimeStr() const;

public:
	WASettings(const WASettings&) = delete;
	WASettings(WASettings&&) = delete;
	WASettings& operator=(const WASettings&) = delete;
	WASettings& operator=(WASettings&&) = delete;
	const QDate _defaultdate = QDate::currentDate();
	QDate _lastupdatedate;
	int _wordcount = 0;
	float _wbasetime = 0;
	float _ebasetime = 0;
	float _wctime = 0;
	float _ectime = 0;
	float _wltime = 0;
	float _eltime = 0;
	float _wrtime = 0;
	[[nodiscard]] int GetTime(int pass) const;
};

namespace WA
{
	extern WASettings* Settings;
};
