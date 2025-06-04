#include "../include/WAProgressbar.hpp"

WAProgressbar::WAProgressbar(QWidget* parent)
	: QWidget(parent)
{
	penframe.setStyle(Qt::SolidLine);
	penframe.setCapStyle(Qt::SquareCap);
	penframe.setJoinStyle(Qt::RoundJoin);
	penframe.setWidth(2);
	penp = penframe;
	penframe.setColor(Qt::black);
	penp.setColor(Qt::blue);
	brushback.setStyle(Qt::SolidPattern);
	brushback.setColor(Qt::white);
	brushfill.setStyle(Qt::SolidPattern);
	brushfill.setColor(Qt::green);
}

WAProgressbar::~WAProgressbar()
= default;

void WAProgressbar::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)
	QPainter painter(this);
	painter.setViewport(0, 0, width(), height());
	painter.setWindow(-3, -1, 606, 11);
	painter.setRenderHint(QPainter::Antialiasing);

	int barlen = 600 * _value / _maximal;

	painter.fillRect(barlen, 0, 600 - barlen, 9, brushback);
	if (barlen > 0)
	{
		painter.fillRect(0, 0, barlen, 9, brushfill);
	}

	if (_pointer < _maximal)
	{
		int pstart = 600 * _pointer / _maximal;
		int pend = 600 * (_pointer + 1) / _maximal;
		painter.setPen(penp);
		painter.drawLine(pstart, 1, pend, 1);
		painter.drawLine(pstart, 8, pend, 8);
	}

	painter.setPen(penframe);
	painter.drawRect(-1, -1, 602, 11);
}

void WAProgressbar::setMaximal(const int& m)
{
	_maximal = m;
	_pointer = 0;
	_value = 0;
	update();
}

void WAProgressbar::setValue(const int& v)
{
	_value = v;
	update();
}

void WAProgressbar::setValue(const int& v, const int& p)
{
	_value = v;
	_pointer = p;
	update();
}

void WAProgressbar::setPointer(const int& p)
{
	_pointer = p;
	update();
}
