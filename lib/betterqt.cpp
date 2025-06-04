#include "betterqt.hpp"

#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QMessageBox>
#include <QStackedLayout>

#include "betterc.hpp"

bool bq::openFile(const QString& path, QFile& file, QIODevice::OpenMode mode)
{
	ifn(file.open(mode))
	{
		int num = mode;
		QStringList modelist;
		ifn(num)
			modelist.emplace_back("NotOpen");
		else
		{
			if ((num & 0x0003) == 0x0003)
				modelist.emplace_back("ReadWrite");
			else if (num & 0x0001)
				modelist.emplace_back("ReadOnly");
			else if (num & 0x0002)
				modelist.emplace_back("WriteOnly");
			if (num & 0x0004)
				modelist.emplace_back("Append");
			if (num & 0x0008)
				modelist.emplace_back("Truncate");
			if (num & 0x0010)
				modelist.emplace_back("Text");
			if (num & 0x0020)
				modelist.emplace_back("Unbuffered");
			if (num & 0x0040)
				modelist.emplace_back("NewOnly");
			if (num & 0x0080)
				modelist.emplace_back("ExistingOnly");
		}
		QMessageBox::critical(nullptr, QObject::tr("Error"),
		                      QObject::tr("File %1 CannotOpenWithMode %2").arg(path).arg(modelist.join('|')),
		                      QMessageBox::Ok);
		return false;
	}
	return true;
}

void bq::copy(const QString& text)
{
	QClipboard* clip = QApplication::clipboard();
	clip->setText(text);
}

void qu::retainSize(QWidget* w)
{
	if (w)
	{
		auto sizepolicy = w->sizePolicy();
		sizepolicy.setRetainSizeWhenHidden(true);
		w->setSizePolicy(sizepolicy);
	}
}

QVBoxLayout* qu::newvlayout(const layoutinit& init)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(init.mgleft, init.mgtop, init.mgright, init.mgbottom);
	layout->setSpacing(init.vspace);
	return layout;
}

QHBoxLayout* qu::newhlayout(const layoutinit& init)
{
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(init.mgleft, init.mgtop, init.mgright, init.mgbottom);
	layout->setSpacing(init.hspace);
	return layout;
}

QGridLayout* qu::newglayout(const layoutinit& init)
{
	QGridLayout* layout = new QGridLayout();
	layout->setContentsMargins(init.mgleft, init.mgtop, init.mgright, init.mgbottom);
	layout->setVerticalSpacing(init.vspace);
	layout->setHorizontalSpacing(init.hspace);
	return layout;
}

QStackedLayout* qu::newslayout()
{
	return new QStackedLayout();
}

QFrame* qu::newframe(frameinit init)
{
	QFrame* frame = new QFrame();
	frame->setFrameShape(init.shape);
	frame->setFrameShadow(init.shadow);
	return frame;
}

QFont qu::newfont(fontinit init)
{
	QFont font;
	if (init.pointsize > 0)
		font.setPointSize(init.pointsize);
	font.setBold(init.bold);
	return font;
}

QSpacerItem* qu::newspace(struct policyinit init)
{
	return new QSpacerItem(0, 0, init.hpolicy, init.vpolicy);
}
