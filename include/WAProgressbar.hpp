#pragma once

#include <QWidget>
#include <QPainter>

class WAProgressbar : public QWidget
{
	Q_OBJECT

	int _maximal = 100;
	int _value = 0;
	int _pointer = 0;
	QPen penframe;
	QPen penp;
	QBrush brushback;
	QBrush brushfill;

public:
	WAProgressbar(const WAProgressbar&) = delete;
	WAProgressbar(WAProgressbar&&) = delete;
	WAProgressbar& operator=(const WAProgressbar&) = delete;
	WAProgressbar& operator=(WAProgressbar&&) = delete;
	WAProgressbar(QWidget* parent = nullptr);
	~WAProgressbar() override;
	void paintEvent(QPaintEvent* event) override;
	void setMaximal(const int& m);
	void setValue(const int& v);
	void setValue(const int& v, const int& p);
	void setPointer(const int& p);
};
