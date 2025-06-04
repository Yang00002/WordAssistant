#pragma once
#include <QWidget>

class QFrame;
class QLineEdit;
class QLabel;
class QPushButton;
class QButtonGroup;
class QStackedLayout;
class WAProgressbar;
class QRadioButton;
class QTextEdit;
class QLCDNumber;
class QProgressBar;
class QSlider;
class QAbstractButton;

class WAwindow final : public QWidget
{
	Q_OBJECT

public:
	WAwindow(const WAwindow&) = delete;
	WAwindow(WAwindow&&) = delete;
	WAwindow& operator=(const WAwindow&) = delete;
	WAwindow& operator=(WAwindow&&) = delete;
	WAwindow(QApplication* app, QWidget* parent = nullptr);
	~WAwindow() override = default;

private:
	QApplication* _app = nullptr;
	QStackedLayout* _sp = nullptr;
	QRadioButton* _brmb = nullptr;
	QRadioButton* _bupd = nullptr;
	QRadioButton* _bnew = nullptr;
	QRadioButton* _bsta = nullptr;
	QRadioButton* _btmp = nullptr;
	QButtonGroup* _bg = nullptr;
	QTextEdit* _txtedit = nullptr;
	QPushButton* _txtbutton = nullptr;
	QLCDNumber* _lcd = nullptr;
	QProgressBar* _timep = nullptr;
	QLabel* _label = nullptr;
	QLineEdit* _ledit = nullptr;
	QPushButton* _b1 = nullptr;
	QPushButton* _b2 = nullptr;
	QPushButton* _b3 = nullptr;
	QPushButton* _b4 = nullptr;
	QLabel* _pnum = nullptr;
	QTimer* _timer = nullptr;
	WAProgressbar* _pbar = nullptr;
	QPushButton* _abutton = nullptr;
	QSlider* _aslider = nullptr;
	QFrame* _aframe = nullptr;
	int _time = 0;
	int _maxtime = 0;
	const int* _wordcount = nullptr;
	const int* _workindex = nullptr;
	const int* _moveindex = nullptr;

	enum class Page: uint8_t
	{
		None,
		New,
		RmbWork,
		RmbWorkW,
		RmbWorkE,
		RmbMove,
		RmbErr,
		TmpNew,
		TmpWork,
		TmpWorkW,
		TmpWorkE,
		TmpMove,
		TmpErr
	} _currentPage{Page::None};

	void onbupdOn();
	void onbnewOn();
	void onbrmbOn();
	void onbnewOff() const;
	void onbrmbOff() const;
	void onbtmpOff() const;
	void onbtmpOn();
	void onbstaOn();
	void setButtonUnchecked(QRadioButton*& button) const;
	void changePage(Page page, void* data = nullptr);
	void resetTimer(int maxTime);
	void onTimerEnd();

private slots:
	void onShortcutPageUp();
	void onShortcutPageDown();
	void onBgShortcutToggled() const;
	void onBgDown() const;
	void onBgUp() const;
	void onBgToggled(const QAbstractButton* button, bool isOn);
	void txtButtonPushed();
	void B1Pushed();
	void B2Pushed();
	void B3Pushed();
	void B4Pushed() const;
	void onBPLeft() const;
	void onBPRight() const;
	void onBPUp() const;
	void onBPDown() const;
	void tick();
	void tryClickAudioButton() const;
	void tryClickB2() const;
	void tryClickB3() const;
	void tryClickB4() const;
	void onLineSubmit();
	void handleQuit() const;
};
