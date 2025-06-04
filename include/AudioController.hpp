#pragma once
#include <QObject>


class QBuffer;
class QNetworkReply;
class QMediaPlayer;
class QAudioOutput;

class AudioBuffer : public QObject
{
	Q_OBJECT

	QBuffer* _data = nullptr;
	QByteArray* _array = nullptr;
	QNetworkReply* _reply = nullptr;
	bool _ready = false;
	bool _autoplay = false;
	static QMediaPlayer* _player;

public:
	AudioBuffer(const AudioBuffer&) = delete;
	AudioBuffer(AudioBuffer&&) = delete;
	AudioBuffer& operator=(const AudioBuffer&) = delete;
	AudioBuffer& operator=(AudioBuffer&&) = delete;

	static void setPlayer(QMediaPlayer* player)
	{
		_player = player;
	}

	AudioBuffer(const QString& word, bool autoplay = false);
	~AudioBuffer() override;
	[[nodiscard]] bool isReady() const;
	[[nodiscard]] bool isEmpty() const;
	[[nodiscard]] QBuffer* GetAudio() const;

private slots:
	void onDownloaded();
};

class AudioController : public QObject
{
	Q_OBJECT

private:
	QCache<QString, AudioBuffer>* _audiocache = nullptr;
	QMediaPlayer* _player = nullptr;
	QAudioOutput* _output = nullptr;
	QString _focusword;

public:
	AudioController(const AudioController&) = delete;
	AudioController(AudioController&&) = delete;
	AudioController& operator=(const AudioController&) = delete;
	AudioController& operator=(AudioController&&) = delete;
	AudioController(QObject* parent = nullptr);
	~AudioController() override;
	//need to check for not contain first
	void addWord(const QString& word) const;
	[[nodiscard]] bool contain(const QString& word) const;
	void setFocusWord(const QString& word);

public slots:
	void changeVolume(int v) const;
	void play() const;
};
