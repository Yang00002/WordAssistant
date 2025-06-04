#include "../include/WAwindow.hpp"

#include <QApplication>
#include <QLabel>
#include <QLCDNumber>
#include <QLineEdit>
#include <Qmessagebox>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QStackedLayout>
#include <QString>
#include <Qstylefactory>
#include <QTextEdit>
#include <Qtimer>

#include "../include/WAProgressbar.hpp"
#include "../include/WAserver.hpp"
#include "../include/WASettings.hpp"

namespace
{
	void bupdEmptyMessage()
	{
		QMessageBox::information(nullptr, QObject::tr("Over"), QObject::tr("UpdateWordEmpty"), QMessageBox::Ok);
	}

	void bupdSuccessMessage()
	{
		QMessageBox::information(nullptr, QObject::tr("Over"), QObject::tr("UpdateWordSuccess"), QMessageBox::Ok);
	}

	void bnewSuccessMessage()
	{
		QMessageBox::information(nullptr, QObject::tr("Over"), QObject::tr("AddWordSuccess"), QMessageBox::Ok);
	}

	void brmbEmptyMessage()
	{
		QMessageBox::information(nullptr, QObject::tr("Over"), QObject::tr("RememberWordEmpty"), QMessageBox::Ok);
	}

	void brmbSuccessMessage()
	{
		QMessageBox::information(nullptr, QObject::tr("Over"), QObject::tr("RememberWordSuccess"), QMessageBox::Ok);
	}
}


WAwindow::WAwindow(QApplication* app, QWidget* parent)
	: QWidget(parent)
{
	auto font24bold = qu::newfont({.pointsize = 24, .bold = true});
	auto font16bold = qu::newfont({.pointsize = 16, .bold = true});
	auto font12 = qu::newfont({.pointsize = 12});

	_app = app;

	this->setWindowIcon(QIcon("./WordAssistant.ico"));
	this->setMinimumSize(this->size());
	auto layout = qu::newhlayout(_widmgs);
	this->setLayout(layout);
	//左侧按钮
	qu_child(layout) {
		auto [frame0, layout0] = qu::pair(
			qu::newframe({.shape = qu::shape::Box}),
			qu::newvlayout(_widmgs));
		layout->addWidget(frame0);
		qu_child(layout0) {
			auto [frame00, layout00] = qu::pair(
				qu::newframe({.shape = qu::shape::Box, .shadow = qu::shadow::Raised}),
				qu::newvlayout(_widmg)
			);
			layout0->addWidget(frame00);
			frame00->setMaximumHeight(500);
			qu_child(layout00) {
				_brmb = qu::newbutton<QRadioButton>(font16bold, tr("_brmb"));
				layout00->addWidget(_brmb);
			}
			qu_child(layout00) {
				_bupd = qu::newbutton<QRadioButton>(font16bold, tr("_bupd"));
				layout00->addWidget(_bupd);
			}
			qu_child(layout00) {
				_bnew = qu::newbutton<QRadioButton>(font16bold, tr("_bnew"));
				layout00->addWidget(_bnew);
			}
			qu_child(layout00) {
				_bsta = qu::newbutton<QRadioButton>(font16bold, tr("_bsta"));
				layout00->addWidget(_bsta);
			}
			qu_child(layout00) {
				_btmp = qu::newbutton<QRadioButton>(font16bold, tr("_btmp"));
				layout00->addWidget(_btmp);
			}
			_bg = qu::newbuttongroup(_brmb, _bupd, _bnew, _bsta, _btmp);
			qu::newshortcut(frame00, Qt::Key_Down, Qt::WidgetWithChildrenShortcut, qu_slot(onBgDown));
			qu::newshortcut(frame00, Qt::Key_Up, Qt::WidgetWithChildrenShortcut, qu_slot(onBgUp));
		}
	}
	//右侧主页面
	qu_child(layout)
	{
		_sp = qu::newslayout();
		layout->addLayout(_sp);
		//空页面
		qu_child(_sp) {
			auto page0 = qu::newwidget();
			_sp->addWidget(page0);
		}
		//添加页面
		qu_child(_sp) {
			auto [page1, layout11] = qu::pair(
				qu::newwidget(),
				qu::newvlayout(_widmgs));
			_sp->addWidget(page1);
			qu_child(layout11) {
				_txtedit = new QTextEdit();
				layout11->addWidget(_txtedit);
				_txtedit->setFont(font12);
				_txtedit->setAcceptRichText(false);
				qu::newshortcut(_txtedit, QKeySequence("Ctrl+S"), Qt::ShortcutContext::WidgetShortcut,
				                qu_slot(txtButtonPushed));
			}
			qu_child(layout11) {
				auto layout111 = qu::newhlayout();
				layout11->addLayout(layout111);
				qu_child(layout111)layout111->addSpacerItem(qu::newspace());
				qu_child(layout111) {
					_txtbutton = qu::newbutton<QPushButton>(font16bold);
					layout111->addWidget(_txtbutton);
					_txtbutton->setMinimumWidth(100);
				}
				qu_child(layout111)layout111->addSpacerItem(qu::newspace());
				qu::setstretch(layout111, 5, 1, 5);
			}
		}
		//单词页面
		qu_child(_sp) {
			auto [page2, layout12] = qu::pair(
				qu::newwidget(),
				qu::newvlayout(_widmg)
			);
			_sp->addWidget(page2);
			//时间条
			qu_child(layout12) {
				auto layout120 = qu::newhlayout(_wids);
				layout12->addLayout(layout120);
				qu_child(layout120) {
					_lcd = new QLCDNumber;
					layout120->addWidget(_lcd);
					_lcd->setFixedWidth(32);
					_lcd->setFixedHeight(24);
					_lcd->setFont(font12);
					_lcd->setSmallDecimalPoint(false);
					_lcd->setDigitCount(2);
					_lcd->setSegmentStyle(QLCDNumber::Filled);
					qu::retainSize(_lcd);
				}
				qu_child(layout120) {
					_timep = new QProgressBar;
					layout120->addWidget(_timep);
					_timep->setTextVisible(false);
					qu::retainSize(_timep);
				}
			}
			//主界面
			qu_child(layout12) {
				auto layout121 = qu::newhlayout();
				layout12->addLayout(layout121);
				//声音条
				qu_child(layout121) {
					auto layout1210 = qu::newvlayout();
					layout121->addLayout(layout1210);
					layout1210->addSpacerItem(qu::newspace({.vpolicy = qu::policy::Expanding}));
					qu_child(layout1210) {
						auto [frame12100, layout12100] = qu::pair(
							qu::newframe({.shape = qu::shape::Box, .shadow = qu::shadow::Plain}), qu::newvlayout({
								.mgright = 0, .mgtop = 11, .mgleft = 0, .mgbottom = 11
							}));
						layout1210->addWidget(frame12100, 0);
						_aframe = frame12100;
						frame12100->setSizePolicy(qu::policy::Preferred, qu::policy::Expanding);
						frame12100->setMaximumSize(40, 240);
						qu::retainSize(frame12100);
						qu_child(layout12100) {
							bc_new(_aslider);
							layout12100->addWidget(_aslider, 0, qu::align::AlignHCenter);
							_aslider->setOrientation(qu::orientation::Vertical);
							_aslider->setMaximum(100);
							_aslider->setValue(100);
						}
						qu_child(layout12100) {
							bc_new(_abutton);
							layout12100->addWidget(_abutton, 0, qu::align::AlignHCenter);
							_abutton->setFixedSize(32, 32);
							_abutton->setIcon(*WA::vico);
						}
					}
				}
				qu_child(layout121) {
					auto layout1211 = qu::newvlayout({.vspace = 15});
					layout121->addLayout(layout1211);
					qu_child(layout1211) {
						_label = new QLabel;
						layout1211->addWidget(_label);
						_label->setFont(font24bold);
						_label->setAlignment(qu::align::AlignCenter);
						_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
						qu::retainSize(_label);
					}
					qu_child(layout1211) {
						auto layout12111 = qu::newhlayout();
						layout1211->addLayout(layout12111);
						qu_child(layout12111)layout12111->addSpacerItem(
							qu::newspace({.hpolicy = qu::policy::Preferred}));
						qu_child(layout12111) {
							_ledit = new QLineEdit;
							layout12111->addWidget(_ledit);
							_ledit->setSizePolicy(qu::policy::Preferred, qu::policy::Preferred);
							_ledit->setMinimumSize(132, 48);
							_ledit->setMaximumHeight(72);
							_ledit->setFont(font12);
							qu::retainSize(_ledit);
						}
						qu_child(layout12111)layout12111->addSpacerItem(
							qu::newspace({.hpolicy = qu::policy::Preferred}));
						qu::setstretch(layout12111, 1, 1, 1);
					}
					qu_child(layout1211) {
						auto layout12112 = qu::newglayout();
						layout1211->addLayout(layout12112);
						qu_child(layout12112) layout12112->addItem(qu::newspace(), 0, 1);
						qu_child(layout12112) layout12112->addItem(qu::newspace(), 1, 0);
						qu_child(layout12112) {
							bc_setp(_b1, qu::newbutton)(font16bold);
							layout12112->addWidget(_b1, 1, 1);
							_b1->setSizePolicy(qu::policy::Preferred, qu::policy::Preferred);
							qu::retainSize(_b1);
						}
						qu_child(layout12112) layout12112->addItem(qu::newspace(), 1, 2);
						qu_child(layout12112) layout12112->addItem(qu::newspace(), 2, 1);
						qu_child(layout12112) {
							bc_setp(_b3, qu::newbutton)(font16bold);
							layout12112->addWidget(_b3, 3, 1);
							_b3->setSizePolicy(qu::policy::Preferred, qu::policy::Preferred);
							_b3->setText(tr("reversechoice"));
							qu::retainSize(_b3);
						}
						qu_child(layout12112) {
							bc_setp(_b2, qu::newbutton)(font16bold);
							layout12112->addWidget(_b2, 1, 3);
							_b2->setSizePolicy(qu::policy::Preferred, qu::policy::Preferred);
							qu::retainSize(_b2);
						}
						qu_child(layout12112) {
							bc_setp(_b4, qu::newbutton)(font16bold);
							layout12112->addWidget(_b4, 3, 3);
							_b4->setSizePolicy(qu::policy::Preferred, qu::policy::Preferred);
							qu::retainSize(_b4);
						}
						qu_child(layout12112) layout12112->addItem(qu::newspace(), 1, 4);
						qu_child(layout12112) layout12112->addItem(qu::newspace(), 4, 1);
						qu::setrowstretch(layout12112, 1, 2, 1, 2, 1);
						qu::setcolstretch(layout12112, 2, 3, 4, 3, 2);
					}
					qu::setstretch(layout1211, 3, 1, 2);
				}
			}
			//进度条
			qu_child(layout12) {
				auto layout122 = qu::newvlayout();
				layout12->addLayout(layout122);
				qu_child(layout122) {
					bc_new(_pnum);
					layout122->addWidget(_pnum, 0, qu::align::AlignHCenter | qu::align::AlignBottom);
					_pnum->setFont(font12);
					qu::retainSize(_pnum);
				}
				qu_child(layout122) {
					bc_new(_pbar);
					layout122->addWidget(_pbar);
					_pbar->setFixedHeight(20);
					qu::retainSize(_pbar);
				}
			}
			qu::newshortcut(page2, Qt::Key_PageDown, Qt::WidgetWithChildrenShortcut, qu_slot(onShortcutPageDown));
			qu::newshortcut(page2, Qt::Key_PageUp, Qt::WidgetWithChildrenShortcut, qu_slot(onShortcutPageUp));
			qu::newshortcut(page2, Qt::Key_Left, Qt::WidgetWithChildrenShortcut, qu_slot(onBPLeft));
			qu::newshortcut(page2, Qt::Key_Right, Qt::WidgetWithChildrenShortcut, qu_slot(onBPRight));
			qu::newshortcut(page2, Qt::Key_Up, Qt::WidgetWithChildrenShortcut, qu_slot(onBPUp));
			qu::newshortcut(page2, Qt::Key_Down, Qt::WidgetWithChildrenShortcut, qu_slot(onBPDown));
			qu::newshortcut(page2, Qt::Key_Backspace, Qt::WidgetWithChildrenShortcut,qu_slot(tryClickB2));
			qu::newshortcut(page2, Qt::CTRL | Qt::Key_Return, Qt::WidgetWithChildrenShortcut, qu_slot(tryClickB3));
			qu::newshortcut(page2, Qt::CTRL | Qt::Key_Backspace, Qt::WidgetWithChildrenShortcut, qu_slot(tryClickB4));
			qu::newshortcut(page2, Qt::CTRL | Qt::Key_P, Qt::WidgetWithChildrenShortcut, qu_slot(tryClickAudioButton));
		}
		_sp->setCurrentIndex(0);
	}
	qu::setstretch(layout, 1, 4);

	WA::controller = new AudioController(this);
	WA::controller->changeVolume(100);

	_timer = new QTimer(this);
	_timer->setInterval(1000);

	_wordcount = rmblist.getcount();
	_workindex = rmblist.getindex();
	_moveindex = rmblist.getmoveindex();

	connect(_timer, &QTimer::timeout, this, &WAwindow::tick);
	connect(_bg, &QButtonGroup::buttonToggled, this, &WAwindow::onBgToggled);
	connect(_txtbutton, &QPushButton::clicked, this, &WAwindow::txtButtonPushed);
	connect(_b1, &QPushButton::clicked, this, &WAwindow::B1Pushed);
	connect(_b2, &QPushButton::clicked, this, &WAwindow::B2Pushed);
	connect(_b3, &QPushButton::clicked, this, &WAwindow::B3Pushed);
	connect(_b4, &QPushButton::clicked, this, &WAwindow::B4Pushed);
	connect(_ledit, &QLineEdit::returnPressed, this, &WAwindow::onLineSubmit);
	connect(_aslider, &QSlider::valueChanged, WA::controller, &AudioController::changeVolume);
	connect(_abutton, &QPushButton::clicked, WA::controller, &AudioController::play);
	connect(app, &QApplication::aboutToQuit, this, &WAwindow::handleQuit);

	this->setFocus();

	this->setWindowState(Qt::WindowMaximized);
	this->show();
}

void WAwindow::onShortcutPageUp()
{
	switch (_currentPage)
	{
		case Page::RmbMove:
			{
				if (_moveindex != nullptr)
				{
					rmblist.pre();
					changePage(Page::RmbMove);
				}
				break;
			}
		case Page::TmpMove:
			{
				if (_moveindex != nullptr)
				{
					rmblist.pre();
					changePage(Page::TmpMove);
				}
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::onShortcutPageDown()
{
	switch (_currentPage)
	{
		case Page::RmbMove:
			{
				if ((*_moveindex) + 1 != (*_workindex))
				{
					rmblist.next();
					changePage(Page::RmbMove);
				}
				break;
			}
		case Page::TmpMove:
			{
				if ((*_moveindex) + 1 != (*_workindex))
				{
					rmblist.next();
					changePage(Page::TmpMove);
				}
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::onBgShortcutToggled() const
{
	if (const auto widget = QApplication::focusWidget(); widget == _brmb)
	{
		_brmb->setChecked(true);
	}
	else if (widget == _bupd)
	{
		_bupd->setChecked(true);
	}
	else if (widget == _bnew)
	{
		_bnew->setChecked(true);
	}
	else if (widget == _bsta)
	{
		_bsta->setChecked(true);
	}
	else if (widget == _btmp)
	{
		_btmp->setChecked(true);
	}
}

void WAwindow::onbupdOn()
{
	changePage(Page::None);
	auto res = UpdateBaselineFromFile();
	if (res == EMPTY)
		bupdEmptyMessage();
	else if (res == NOERROR)
		bupdSuccessMessage();
	setButtonUnchecked(_bupd);
}

void WAwindow::onbnewOn()
{
	changePage(Page::New);
}

void WAwindow::onbrmbOn()
{
	changePage(Page::None);
	if (UpdateBaselineFromFile() == FERROR)
	{
		setButtonUnchecked(_brmb);
		return;
	}
	if (LoadWorkLineFromFile() == FERROR)
	{
		setButtonUnchecked(_brmb);
		return;
	}
	if (LoadBaseLineFromFile() == FERROR)
	{
		setButtonUnchecked(_brmb);
		return;
	}
	if ((*_wordcount) != 0)
	{
		_pbar->setMaximal(*_wordcount);
		changePage(Page::RmbWork);
	}
	else
	{
		brmbEmptyMessage();
		setButtonUnchecked(_brmb);
		return;
	}
}

void WAwindow::onbnewOff() const
{
	switch (_currentPage)
	{
		case Page::New:
			{
				LogAccidentQuit(_txtedit->toPlainText(), AccidentQuitNew);
				_txtedit->clear();
				break;
			}
		case Page::None:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbMove:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpMove:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::onbrmbOff() const
{
	switch (_currentPage)
	{
		case Page::RmbErr:
			{
				_txtedit->clear();
				break;
			}
		case Page::RmbMove:
		case Page::RmbWorkE:
		case Page::RmbWorkW:
		case Page::RmbWork:
			{
				_timer->stop();
				rmblist.onAccidentQuit();
				break;
			}
		case Page::None:
		case Page::New:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpMove:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::onbtmpOff() const
{
	switch (_currentPage)
	{
		case Page::TmpNew:
		case Page::TmpErr:
			{
				_txtedit->clear();
				break;
			}
		case Page::TmpMove:
		case Page::TmpWorkE:
		case Page::TmpWorkW:
		case Page::TmpWork:
			{
				_timer->stop();
				rmblist.clear();
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbMove:
		case Page::RmbErr:
			break;
	}
}

void WAwindow::onbtmpOn()
{
	changePage(Page::TmpNew);
}

void WAwindow::onbstaOn()
{
	changePage(Page::None);
	int base = 0;
	int work = 0;
	if (GetLineCount(work, base) == FERROR)
		return;
	QMessageBox::information(nullptr, QObject::tr("All %1 work").arg(base + work),
	                         QObject::tr("Base %1 Work %2, Left %3.").arg(base).arg(work).arg(WA::Settings->_wordcount),
	                         QMessageBox::Ok);
	setButtonUnchecked(_bsta);
}

void WAwindow::setButtonUnchecked(QRadioButton*& button) const
{
	_bg->setExclusive(false);
	button->setChecked(false);
	_bg->setExclusive(true);
}

void WAwindow::changePage(Page page, void* data)
{
	static bool played = false;
	switch (page)
	{
		case Page::None:
			{
				if (_currentPage != Page::None)
				{
					switch (_currentPage)
					{
						case Page::New:
							_bnew->setFocus();
							break;
						case Page::RmbWork:
						case Page::RmbWorkW:
						case Page::RmbWorkE:
						case Page::RmbMove:
						case Page::RmbErr:
							_brmb->setFocus();
							break;
						case Page::TmpNew:
						case Page::TmpWork:
						case Page::TmpWorkW:
						case Page::TmpWorkE:
						case Page::TmpMove:
						case Page::TmpErr:
							_btmp->setFocus();
							break;
						case Page::None:
							break;
					}
					_sp->setCurrentIndex(0);
				}
				break;
			}
		case Page::New:
			{
				if (data == nullptr)
					_txtedit->clear();
				else
					_txtedit->setText(*static_cast<QString*>(data));
				_txtbutton->setText(tr("Add"));
				if (_sp->currentIndex() != 1)
				{
					_txtedit->show();
					_txtbutton->show();
					_sp->setCurrentIndex(1);
				}
				_txtedit->setFocus();
				break;
			}
		case Page::RmbWork:
			{
				played = false;
				auto& line = *rmblist.getworkline();
				bool type = line.gettype();
				int time = line.getworkwaittime();
				QString show = line.getworkshown();
				QString word = line.getword();
				if (type)
				{
					_aframe->hide();
				}
				else
				{
					WA::controller->setFocusWord(word);
					WA::controller->play();
					_aslider->show();
					_abutton->show();
					_aframe->show();
				}
				_label->setText(show);
				QPalette color;
				color.setColor(QPalette::WindowText, Qt::black);
				_label->setPalette(color);
				_label->show();
				if (type)
				{
					_currentPage = Page::RmbWorkE;
					_ledit->clear();
					_ledit->show();
					_ledit->setFocus();
					_b1->setText(tr("Submit"));
					_b2->setText(tr("unkwon"));
					_b1->show();
					_b2->show();
				}
				else
				{
					_currentPage = Page::RmbWorkW;
					_ledit->hide();
					_b1->setText(tr("know"));
					_b2->setText(tr("unkwon"));
					_b1->show();
					_b1->setFocus();
					_b2->show();
				}
				_b3->hide();
				_b4->hide();
				_pnum->setText(QString::number(*_workindex) + '/' + QString::number(*_wordcount));
				_pnum->show();
				_pbar->setValue(*_workindex, *_workindex);
				_pbar->show();
				resetTimer(time);
				_timep->show();
				_lcd->show();
				if (_sp->currentIndex() != 2)
					_sp->setCurrentIndex(2);
				_timer->start();
				return;
			}
		case Page::RmbMove:
			{
				auto& line = *rmblist.getmoveline();
				bool type = line.gettype(), righttype = line.isright(), ext = line.isext();
				QString show = line.getdoneshown(), word = line.getword();
				WA::controller->setFocusWord(word);
				_aslider->show();
				_abutton->show();
				_aframe->show();
				if ((played == false) && type && ((*_moveindex) + 1 == (*_workindex)))
				{
					WA::controller->play();
					played = true;
				}
				_lcd->hide();
				_timep->hide();
				_label->setText(show);
				QPalette color;
				if (ext)
					color.setColor(QPalette::WindowText, Qt::blue);
				else if (righttype)
					color.setColor(QPalette::WindowText, Qt::green);
				else
					color.setColor(QPalette::WindowText, Qt::red);
				_label->setPalette(color);
				_label->show();
				if (type && (righttype == false) && ((*_moveindex) + 1 == (*_workindex)))
				{
					_ledit->show();
				}
				else
					_ledit->hide();
				if ((*_moveindex) + 1 == (*_wordcount))
				{
					_b1->setText(tr("done"));
				}
				else
				{
					_b1->setText(tr("nextone"));
				}
				_b1->show();
				_b1->setFocus();
				if ((*_moveindex) != 0)
				{
					_b2->setText(tr("preone"));
					_b2->show();
				}
				else
					_b2->hide();
				_b3->show();
				if (ext)_b4->setText(tr("putdown"));
				else _b4->setText(tr("extract"));
				_b4->show();
				_pnum->setText(
					QString::number(*_moveindex) + '/' + QString::number(*_workindex) + '/' + QString::number(
						*_wordcount));
				_pnum->show();
				_pbar->setValue(*_workindex, *_moveindex);
				_pbar->show();
				if (_sp->currentIndex() != 2)
					_sp->setCurrentIndex(2);
				break;
			}
		case Page::TmpErr:
		case Page::RmbErr:
			{
				if (data == nullptr)
					_txtedit->clear();
				else
					_txtedit->setText(*static_cast<QString*>(data));
				_txtbutton->setText(tr("Ok"));
				if (_sp->currentIndex() != 1)
				{
					_txtedit->show();
					_txtbutton->show();
					_sp->setCurrentIndex(1);
				}
				_txtedit->setFocus();
				break;
			}
		case(Page::TmpNew):
			{
				if (data == nullptr)
					_txtedit->clear();
				else
					_txtedit->setText(*static_cast<QString*>(data));
				_txtbutton->setText(tr("Add"));
				if (_sp->currentIndex() != 1)
				{
					_txtedit->show();
					_txtbutton->show();
					_sp->setCurrentIndex(1);
				}
				_txtedit->setFocus();
				break;
			}
		case(Page::TmpWork):
			{
				played = false;
				auto& line = *rmblist.getworkline();
				played = false;
				bool type = line.gettype();
				int time = line.getworkwaittime();
				QString show = line.getworkshown();
				QString word = line.getword();
				if (type)
				{
					_aframe->hide();
				}
				else
				{
					WA::controller->setFocusWord(word);
					WA::controller->play();
					_aslider->show();
					_abutton->show();
					_aframe->show();
				}
				_label->setText(show);
				QPalette color;
				color.setColor(QPalette::WindowText, Qt::black);
				_label->setPalette(color);
				_label->show();
				if (type)
				{
					_currentPage = Page::TmpWorkE;
					_ledit->clear();
					_ledit->show();
					_ledit->setFocus();
					_b1->setText(tr("Submit"));
					_b2->setText(tr("unkwon"));
					_b1->show();
					_b2->show();
				}
				else
				{
					_currentPage = Page::TmpWorkW;
					_ledit->hide();
					_b1->setText(tr("know"));
					_b2->setText(tr("unkwon"));
					_b1->show();
					_b1->setFocus();
					_b2->show();
				}
				_b3->hide();
				_b4->hide();
				_pnum->setText(QString::number(*_workindex) + '/' + QString::number(*_wordcount));
				_pnum->show();
				_pbar->setValue(*_workindex, *_workindex);
				_pbar->show();
				resetTimer(time);
				_timep->show();
				_lcd->show();
				if (_sp->currentIndex() != 2)
					_sp->setCurrentIndex(2);
				_timer->start();
				return;
			}
		case(Page::TmpMove):
			{
				auto& line = *rmblist.getmoveline();
				bool type = line.gettype(), righttype = line.isright();
				QString show = line.getdoneshown(), word = line.getword();
				WA::controller->setFocusWord(word);
				_aslider->show();
				_abutton->show();
				_aframe->show();
				if ((played == false) && type && ((*_moveindex) + 1 == (*_workindex)))
				{
					WA::controller->play();
					played = true;
				}
				_lcd->hide();
				_timep->hide();
				_label->setText(show);
				QPalette color;
				if (righttype)
					color.setColor(QPalette::WindowText, Qt::green);
				else
					color.setColor(QPalette::WindowText, Qt::red);
				_label->setPalette(color);
				_label->show();
				if (type && (righttype == false) && ((*_moveindex) + 1 == (*_workindex)))
				{
					_ledit->show();
				}
				else
					_ledit->hide();
				if ((*_moveindex) + 1 == (*_wordcount))
				{
					_b1->setText(tr("done"));
				}
				else
				{
					_b1->setText(tr("nextone"));
				}
				_b1->show();
				_b1->setFocus();
				if ((*_moveindex) != 0)
				{
					_b2->setText(tr("preone"));
					_b2->show();
				}
				else
					_b2->hide();
				_b3->show();
				_b4->hide();
				_pnum->setText(
					QString::number(*_moveindex) + '/' + QString::number(*_workindex) + '/' + QString::number(
						*_wordcount));
				_pnum->show();
				_pbar->setValue(*_workindex, *_moveindex);
				_pbar->show();
				if (_sp->currentIndex() != 2)
					_sp->setCurrentIndex(2);
				break;
			}
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
			break;
	}
	_currentPage = page;
}

void WAwindow::resetTimer(int maxTime)
{
	_timer->stop();
	_time = maxTime;
	_maxtime = maxTime;
	_timep->setValue(100);
	_lcd->display(_time);
}

void WAwindow::onTimerEnd()
{
	switch (_currentPage)
	{
		case Page::RmbWorkW:
			{
				rmblist.submit(false);
				changePage(Page::RmbMove);
				break;
			}
		case Page::RmbWorkE:
			{
				rmblist.submit(_ledit->text());
				changePage(Page::RmbMove);
				break;
			}
		case Page::TmpWorkW:
			{
				rmblist.submit(false);
				changePage(Page::TmpMove);
				break;
			}
		case Page::TmpWorkE:
			{
				rmblist.submit(_ledit->text());
				changePage(Page::TmpMove);
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbMove:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpMove:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::txtButtonPushed()
{
	switch (_currentPage)
	{
		case Page::New:
			{
				auto res = AddLineToFile(_txtedit->toPlainText());
				if (res.tag() == FERROR)
				{
					changePage(Page::None);
					setButtonUnchecked(_bnew);
				}
				else if (res.tag() == NOERROR)
				{
					bnewSuccessMessage();
					changePage(Page::None);
					setButtonUnchecked(_bnew);
				}
				else
				{
					QStringList l;
					QString str;
					for (int i = res.getBegin(); i != -1; i = res.next(i))
					{
						l.emplace_back(ErrorMessage(i));
						l.append(std::move(res[i]));
					}
					str = l.join('\n');
					changePage(Page::New, &str);
				}
				break;
			}
		case Page::RmbErr:
			{
				changePage(Page::None);
				setButtonUnchecked(_brmb);
				break;
			}
		case Page::TmpNew:
			{
				auto res = AddTmplineToTmp(_txtedit->toPlainText());
				if (res.tag() == NOERROR)
				{
					if ((*_wordcount) == 0)
					{
						brmbEmptyMessage();
						changePage(Page::None);
						setButtonUnchecked(_btmp);
						break;
					}
					_pbar->setMaximal((*_wordcount));
					changePage(Page::TmpWork);
				}
				else
				{
					QStringList l;
					QString str;
					for (int i = res.getBegin(); i != -1; i = res.next(i))
					{
						l.emplace_back(ErrorMessage(i));
						l.append(std::move(res[i]));
					}
					str = l.join('\n');
					changePage(Page::TmpNew, &str);
				}
				break;
			}
		case Page::TmpErr:
			{
				changePage(Page::None);
				setButtonUnchecked(_btmp);
				break;
			}
		case Page::None:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbMove:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpMove:
			break;
	}
}

void WAwindow::B1Pushed()
{
	switch (_currentPage)
	{
		case Page::RmbWorkW:
			{
				rmblist.submit(true);
				changePage(Page::RmbMove);
				break;
			}
		case Page::RmbWorkE:
			{
				rmblist.submit(_ledit->text());
				changePage(Page::RmbMove);
				break;
			}
		case Page::RmbMove:
			{
				auto i = (*_moveindex) + 1;
				if (i == (*_wordcount))
				{
					ERL res = rmblist.onEnd();
					if (res.tag() == FERROR)
					{
						changePage(Page::None);
						setButtonUnchecked(_brmb);
					}
					else if (res.tag() == NOERROR)
					{
						brmbSuccessMessage();
						changePage(Page::None);
						setButtonUnchecked(_brmb);
					}
					else
					{
						QStringList l;
						for (int index = res.getBegin(); index != -1; index = res.next(index))
						{
							l.emplace_back(ErrorMessage(index));
							l.append(std::move(res[index]));
						}
						QString str = l.join('\n');
						l.clear();
						changePage(Page::RmbErr, &str);
					}
				}
				else if (i == (*_workindex))
				{
					changePage(Page::RmbWork);
				}
				else
				{
					rmblist.next();
					changePage(Page::RmbMove);
				}
				break;
			}
		case Page::TmpWorkW:
			{
				rmblist.submit(true);
				changePage(Page::TmpMove);
				break;
			}
		case Page::TmpWorkE:
			{
				rmblist.submit(_ledit->text());
				changePage(Page::TmpMove);
				break;
			}
		case Page::TmpMove:
			{
				auto i = (*_moveindex) + 1;
				if (i == (*_wordcount))
				{
					ERL res = rmblist.onTmpEnd();
					if (res.tag() == FERROR)
					{
						changePage(Page::None);
						setButtonUnchecked(_btmp);
					}
					else if (res.tag() == NOERROR)
					{
						brmbSuccessMessage();
						changePage(Page::None);
						setButtonUnchecked(_btmp);
					}
					else
					{
						QStringList l;
						for (int index = res.getBegin(); index != -1; index = res.next(index))
						{
							l.emplace_back(ErrorMessage(index));
							l.append(std::move(res[index]));
						}
						QString str = l.join('\n');
						l.clear();
						changePage(Page::TmpErr, &str);
					}
				}
				else if (i == (*_workindex))
				{
					changePage(Page::TmpWork);
				}
				else
				{
					rmblist.next();
					changePage(Page::TmpMove);
				}
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::B2Pushed()
{
	switch (_currentPage)
	{
		case Page::RmbWorkW:
		case Page::RmbWorkE:
			{
				rmblist.submit(false);
				changePage(Page::RmbMove);
				break;
			}
		case Page::RmbMove:
			{
				rmblist.pre();
				changePage(Page::RmbMove);
				break;
			}
		case Page::TmpWorkW:
		case Page::TmpWorkE:
			{
				rmblist.submit(false);
				changePage(Page::TmpMove);
				break;
			}
		case Page::TmpMove:
			{
				rmblist.pre();
				changePage(Page::TmpMove);
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::B3Pushed()
{
	switch (_currentPage)
	{
		case Page::RmbMove:
			{
				auto& i = *rmblist.getmoveline();
				if (i.isright())
				{
					i.submit(false);
					changePage(Page::RmbMove);
				}
				else
				{
					i.submit(true);
					changePage(Page::RmbMove);
				}
				break;
			}
		case Page::TmpMove:
			{
				auto& i = *rmblist.getmoveline();
				if (i.isright())
				{
					i.submit(false);
					changePage(Page::TmpMove);
				}
				else
				{
					i.submit(true);
					changePage(Page::TmpMove);
				}
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::B4Pushed() const
{
	switch (_currentPage)
	{
		case Page::RmbMove:
		case Page::TmpMove:
			{
				auto& i = *rmblist.getmoveline();
				if (i.isext())
				{
					i.put();
					_b4->setText(tr("extract"));
					QPalette color;
					if (i.isright())
						color.setColor(QPalette::WindowText, Qt::green);
					else
						color.setColor(QPalette::WindowText, Qt::red);
					_label->setPalette(color);
				}
				else
				{
					i.extract();
					_b4->setText(tr("putdown"));
					QPalette color;
					color.setColor(QPalette::WindowText, Qt::blue);
					_label->setPalette(color);
				}
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbWorkE:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::onBPLeft() const
{
	auto widget = QApplication::focusWidget();
	if (widget == _b4)
	{
		if (_b3->isVisible())
		{
			_b3->setFocus();
		}
		else
		{
			_b1->setFocus();
		}
	}
	else if ((widget != _b1) && (widget != _b3))
	{
		_b1->setFocus();
	}
}

void WAwindow::onBPRight() const
{
	auto widget = QApplication::focusWidget();
	if (widget == _b1)
	{
		if (_b2->isVisible())
		{
			_b2->setFocus();
		}
		else if (_b4->isVisible())
		{
			_b4->setFocus();
		}
	}
	else if (widget == _b3)
	{
		if (_b4->isVisible())
		{
			_b4->setFocus();
		}
		else if (_b2->isVisible())
		{
			_b2->setFocus();
		}
	}
	else if ((widget != _b2) && (widget != _b4))
	{
		_b1->setFocus();
	}
}

void WAwindow::onBPUp() const
{
	auto widget = QApplication::focusWidget();
	if ((widget == _b1) || (widget == _b2))
	{
		if (_ledit->isVisible())
		{
			_ledit->setFocus();
		}
	}
	else if (widget == _b4)
	{
		if (_b2->isVisible())
		{
			_b2->setFocus();
		}
		else
			_b1->setFocus();
	}
	else if (widget != _ledit)
	{
		_b1->setFocus();
	}
}

void WAwindow::onBPDown() const
{
	auto widget = QApplication::focusWidget();
	if (widget == _b1)
	{
		if (_b3->isVisible())
		{
			_b3->setFocus();
		}
		else if (_b4->isVisible())
		{
			_b4->setFocus();
		}
	}
	else if (widget == _b2)
	{
		if (_b4->isVisible())
		{
			_b4->setFocus();
		}
		else if (_b3->isVisible())
		{
			_b3->setFocus();
		}
	}
	else if ((widget != _b3) && (widget != _b4))
	{
		_b1->setFocus();
	}
}

void WAwindow::tick()
{
	_time--;
	_timep->setValue(100 * _time / _maxtime);
	_lcd->display(_time);
	if (_time == 0)
	{
		_timer->stop();
		onTimerEnd();
	}
}

void WAwindow::tryClickAudioButton() const
{
	if (_abutton->isVisible())
	{
		WA::controller->play();
	}
}

void WAwindow::tryClickB2() const
{
	if (_b2->isVisible())
	{
		_b2->click();
	}
}

void WAwindow::tryClickB3() const
{
	if (_b3->isVisible())
	{
		_b3->click();
	}
}

void WAwindow::tryClickB4() const
{
	if (_b4->isVisible())
	{
		_b4->click();
	}
}

void WAwindow::onLineSubmit()
{
	switch (_currentPage)
	{
		case Page::RmbWorkE:
			{
				rmblist.submit(_ledit->text());
				changePage(Page::RmbMove);
				break;
			}
		case Page::TmpWorkE:
			{
				rmblist.submit(_ledit->text());
				changePage(Page::TmpMove);
				break;
			}
		case Page::RmbMove:
		case Page::TmpMove:
			{
				_b1->setFocus();
				break;
			}
		case Page::None:
		case Page::New:
		case Page::RmbWork:
		case Page::RmbWorkW:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::handleQuit() const
{
	switch (_currentPage)
	{
		case Page::New:
			{
				LogAccidentQuit(_txtedit->toPlainText(), AccidentQuitNew);
				break;
			}
		case Page::RmbMove:
		case Page::RmbWorkE:
		case Page::RmbWorkW:
		case Page::RmbWork:
			{
				_timer->stop();
				rmblist.onAccidentQuit();
				break;
			}
		case Page::None:
		case Page::RmbErr:
		case Page::TmpNew:
		case Page::TmpWork:
		case Page::TmpWorkW:
		case Page::TmpWorkE:
		case Page::TmpMove:
		case Page::TmpErr:
			break;
	}
}

void WAwindow::onBgDown() const
{
	auto widget = QApplication::focusWidget();
	if (widget == _brmb)
	{
		_bupd->setFocus();
	}
	else if (widget == _bupd)
	{
		_bnew->setFocus();
	}
	else if (widget == _bnew)
	{
		_bsta->setFocus();
	}
	else if (widget == _bsta)
	{
		_btmp->setFocus();
	}
	else
	{
		_brmb->setFocus();
	}
}

void WAwindow::onBgUp() const
{
	auto widget = QApplication::focusWidget();
	if (widget == _brmb)
	{
		_btmp->setFocus();
	}
	else if (widget == _bnew)
	{
		_bupd->setFocus();
	}
	else if (widget == _bsta)
	{
		_bnew->setFocus();
	}
	else if (widget == _btmp)
	{
		_bsta->setFocus();
	}
	else
	{
		_brmb->setFocus();
	}
}

void WAwindow::onBgToggled(const QAbstractButton* button, bool isOn)
{
	if (isOn)
	{
		if (button == _bupd)
			onbupdOn();
		else if (button == _bnew)
			onbnewOn();
		else if (button == _brmb)
			onbrmbOn();
		else if (button == _btmp)
			onbtmpOn();
		else if (button == _bsta)
			onbstaOn();
	}
	else
	{
		if (button == _bnew)
			onbnewOff();
		else if (button == _brmb)
			onbrmbOff();
		else if (button == _btmp)
			onbtmpOff();
	}
}
