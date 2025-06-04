#include "../include/AudioController.hpp"

#include <QAudioOutput>
#include <QBuffer>
#include <QCache>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "../include/WASettings.hpp"
#include "../lib/betterqt.hpp"


AudioController::AudioController(QObject *parent)
	: QObject(parent)
{
	
	_player = new QMediaPlayer(this);
	_output = new QAudioOutput(this);
	_player->setAudioOutput(_output);
	AudioBuffer::setPlayer(_player);
	_audiocache = new QCache<QString, AudioBuffer>;
	_audiocache->setMaxCost(4096);
}

AudioController::~AudioController()
{
	delete _audiocache;
}

void AudioController::addWord(const QString& word) const
{
	_audiocache->insert(word, new AudioBuffer(word), word.size());
}

bool AudioController::contain(const QString& word) const
{
	return _audiocache->contains(word);
}

void AudioController::play() const
{
	if (_focusword.isEmpty() == false)
	{
		if (_audiocache->contains(_focusword))
		{
			auto p = _audiocache->object(_focusword);
			if (p->isReady() && (p->isEmpty() == false))
			{
				auto source = p->GetAudio();
				if (_player->sourceDevice() != source)
					_player->setSourceDevice(source);
				_player->play();
			}
			else
			{
				_player->setSourceDevice(nullptr);
			}
		}
		else
		{
			_audiocache->insert(_focusword, new AudioBuffer(_focusword, true), _focusword.size());
		}
	}
}

void AudioController::setFocusWord(const QString& word)
{
	_focusword = word;
}

void AudioController::changeVolume(int v) const
{
	_output->setVolume(static_cast<float>(v));
}

AudioBuffer::AudioBuffer(const QString& word, bool autoplay) :QObject(nullptr)
{
	_autoplay = autoplay;
	_reply = WA::manager->get(QNetworkRequest(QUrl(wa_audioapi + word)));
	connect(_reply, &QNetworkReply::finished, qu_slot(onDownloaded));
}



AudioBuffer::~AudioBuffer()
{
	if (_reply)
	{
		disconnect(_reply, &QNetworkReply::finished, qu_slot(onDownloaded));
		_reply->deleteLater();
		_reply = nullptr;
	}
	_ready = false;
	if (_data)
	{
		delete _data;
		_data = nullptr;
		delete _array;
		_array = nullptr;
	}
}

bool AudioBuffer::isReady() const
{
	return _ready;
}

bool AudioBuffer::isEmpty() const
{
	return _data == nullptr;
}

QBuffer* AudioBuffer::GetAudio() const
{
	return _data;
}

void AudioBuffer::onDownloaded()
{
	if (_reply->error() == QNetworkReply::NoError)
	{
		_array = new QByteArray(_reply->readAll());
		_reply->deleteLater();
		if (_array->size() && _array->operator[](0) != '<')
		{
			_data = new QBuffer(_array);
			if (_autoplay)
			{
				_player->setSourceDevice(_data);
				_player->play();
			}
		}
		else
		{
			if(_autoplay)
				_player->setSourceDevice(nullptr);
			delete _array;
			_array = nullptr;
		}
	}
	else
	{
		if(_autoplay)
			_player->setSourceDevice(nullptr);
		_reply->deleteLater();
	}
	_reply = nullptr;
	_ready = true;
}

QMediaPlayer* AudioBuffer::_player = nullptr;
