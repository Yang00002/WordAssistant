#pragma once
#include <QAbstractButton>
#include <QButtonGroup>
#include <QDateTime>
#include <QFile>
#include <QFrame>
#include <QLayout>
#include <QShortcut>

#include "betterc.hpp"

#define _mgs(margin,space) { .mgright = (margin),.mgtop = (margin),.mgleft = (margin),.mgbottom = (margin),.hspace = (space),.vspace = (space) }
#define _widmgs _mgs(11,6)
#define _widmg _mgs(11,0)
#define _wids _mgs(0,6)
#define qu_child(parent)
#define qu_slot(slotname) this,&std::remove_pointer<decltype(this)>::type::slotname
#define qu_signal(sender,signalname) sender,&std::remove_pointer<decltype(sender)>::type::signalname


class QStackedLayout;

namespace qu
{
	typedef QFrame::Shape shape;
	typedef QFrame::Shadow shadow;
	typedef QSizePolicy::Policy policy;
	typedef Qt::AlignmentFlag align;
	typedef Qt::Orientation orientation;

	struct layoutinit
	{
		int mgright = 0;
		int mgtop = 0;
		int mgleft = 0;
		int mgbottom = 0;
		int hspace = 0;
		int vspace = 0;
	};

	struct frameinit
	{
		shape shape = shape::NoFrame;
		shadow shadow = shadow::Plain;
	};

	struct fontinit
	{
		int pointsize = -1;
		bool bold = false;
	};

	struct policyinit
	{
		policy hpolicy = policy::Minimum;
		policy vpolicy = policy::Minimum;
	};
}

namespace qu
{
	void retainSize(QWidget* w);
	QVBoxLayout* newvlayout(const struct layoutinit& init = {});
	QHBoxLayout* newhlayout(const struct layoutinit& init = {});
	QGridLayout* newglayout(const struct layoutinit& init = {});
	QStackedLayout* newslayout();
	QFrame* newframe(struct frameinit init = {});

	inline QWidget* newwidget()
	{
		return new QWidget;
	}

	QFont newfont(struct fontinit init = {});

	QSpacerItem* newspace(struct policyinit init = {});
}

namespace bq
{
	typedef QIODeviceBase::OpenModeFlag mode;
	typedef Qt::SplitBehaviorFlags split;
	bool openFile(const QString& path, QFile& file, QIODevice::OpenMode mode);
	void copy(const QString& text);

	template <typename T>
	class errorarray
	{
	public:
		errorarray(const errorarray& other) = delete;
		errorarray& operator=(const errorarray& other) = delete;

	private:
		T** _data = nullptr;
		int _size = 0;
		int _tag = 0;

		void expand(const int& size)
		{
			int nextsize = log2ceil(size > 1 ? size : 2);
			if (nextsize == 0)
				exit(-1);
			T** tmp = _data;
			_data = new T*[nextsize];
			for (int i = 0; i < _size; i++)
			{
				_data[i] = tmp[i];
			}
			for (int i = _size; i < nextsize; i++)
			{
				_data[i] = nullptr;
			}
			delete tmp;
			_size = nextsize;
		}

		void free()
		{
			if (_data)
			{
				for (int i = 0; i < _size; i++)
				{
					if (_data[i])
						delete _data[i];
				}
				delete[] _data;
			}
		}

	public:
		errorarray() = default;

		errorarray(errorarray&& ar) noexcept
		{
			_size = ar._size;
			_data = ar._data;
			_tag = ar._tag;
			ar._data = nullptr;
		}

		~errorarray()
		{
			free();
		}

		errorarray& operator=(errorarray&& ar) noexcept
		{
			if (this != &ar)
			{
				free();
				_size = ar._size;
				ar._size = 0;
				_data = ar._data;
				ar._data = nullptr;
				_tag = ar._tag;
				ar._tag = 0;
			}
			return *this;
		}

		errorarray* resize(const int& size)
		{
			if (size > _size)
				expand(size);
			return this;
		}

		T& operator[](const int& index)
		{
			if (index >= _size)
				expand(index + 1);
			if (_data[index] == nullptr)
				_data[index] = new T();
			return *(_data[index]);
		}

		T& at(const int& index)
		{
			if (index >= _size)
				expand(index + 1);
			if (_data[index] == nullptr)
				_data[index] = new T();
			return *(_data[index]);
		}

		const T& get(const int& index) const
		{
			static T v = T();
			if (index >= _size)
				return v;
			if (_data[index] == nullptr)
				return v;
			return *(_data[index]);
		}

		void set(const int& index, const T& item)
		{
			if (index >= _size)
				expand(index + 1);
			if (_data[index] == nullptr)
				_data[index] = new T(item);
			else
				*(_data[index]) = item;
		}

		void eat(const int& index, T&& item)
		{
			if (index >= _size)
				expand(index + 1);
			if (_data[index] == nullptr)
				_data[index] = new T(std::move(item));
			else
				*(_data[index]) = std::move(item);
		}

		[[nodiscard]] bool exist(const int& index) const
		{
			if (index >= _size)
				return false;
			if (_data[index] == nullptr)
				return false;
			return true;
		}

		[[nodiscard]] int getBegin() const
		{
			for (int i = 0; i < _size; i++)
			{
				if (_data[i])
					return i;
			}
			return -1;
		}

		[[nodiscard]] static int getEnd()
		{
			return -1;
		}

		[[nodiscard]] int next(const int& index) const
		{
			for (int i = index + 1; i < _size; i++)
			{
				if (_data[i])
					return i;
			}
			return -1;
		}

		operator bool() const
		{
			return _data != nullptr;
		}

		void clear()
		{
			free();
			_size = 0;
			_data = nullptr;
			_tag = 0;
		}

		[[nodiscard]] const int& tag() const
		{
			return _tag;
		}

		void settag(const int& t)
		{
			_tag = t;
		}

		void writelog(const QString& logdir, const QDate& date,
		              std::function<QString(const int&, const T&)> outer) const
		{
			if (_data)
			{
				QString logname = QString::asprintf("/%d-%d.log", date.year(), date.month());
				QString logtime = QString::asprintf("%d-%d-%d", date.year(), date.month(), date.day());
				QFile file(logdir + logname);
				ifn(file.open(bq::mode::WriteOnly | bq::mode::Append))return;
				QTextStream stream(&file);
				stream << logtime << '\n';
				for (int i = 0; i < _size; i++)
				{
					if (_data[i])
						stream << outer(i, *(_data[i])) << '\n';
				}
				file.close();
			}
			return;
		}
	};
}

namespace qu
{
	template <typename Slotname>
	void newshortcut(QWidget* parent, const QKeySequence& key, Slotname&& slot) // NOLINT(cppcoreguidelines-missing-std-forward)
	{
		QShortcut* shortcut = new QShortcut(key, parent);
		shortcut->setContext(Qt::WidgetShortcut);
		QObject::connect(shortcut, &QShortcut::activated, parent, slot);
	}

	template <typename SlotWidgetType, typename Slotname>
	void newshortcut(QWidget* parent, const QKeySequence& key, Qt::ShortcutContext context, const SlotWidgetType& slotwidget, Slotname&& slot) // NOLINT(cppcoreguidelines-missing-std-forward)
	{
		QShortcut* shortcut = new QShortcut(key, parent);
		shortcut->setContext(context);
		QObject::connect(shortcut, &QShortcut::activated, slotwidget, slot);
	}

	template <typename WidgetType, typename LayoutType>
	std::pair<WidgetType*, LayoutType*> pair(WidgetType* widget, LayoutType* layout)
	{
		widget->setLayout(layout);
		return {widget, layout};
	}

	template <typename... G>
	void setstretch(QVBoxLayout* layout, G... stretchlist)
	{
		int index = 0;
		[[maybe_unused]] bool arr[] = {
			([&layout ,&index](auto stretch)-> bool
			{
				if (stretch >= 0)
					layout->setStretch(index, stretch);
				index++;
				return true;
			})(std::forward<G>(stretchlist))...
		};
	}

	template <typename... G>
	void setstretch(QHBoxLayout* layout, G... stretchlist)
	{
		int index = 0;
		[[maybe_unused]] bool arr[] = {
			([&layout ,&index](auto stretch)-> bool
			{
				if (stretch >= 0)
					layout->setStretch(index, stretch);
				index++;
				return true;
			})(std::forward<G>(stretchlist))...
		};
	}

	template <typename... G>
	void setrowstretch(QGridLayout* layout, G... stretchlist)
	{
		int index = 0;
		[[maybe_unused]] bool arr[] = {
			([&layout ,&index](auto stretch)-> bool
			{
				if (stretch >= 0)
					layout->setRowStretch(index, stretch);
				index++;
				return true;
			})(std::forward<G>(stretchlist))...
		};
	}

	template <typename... G>
	void setcolstretch(QGridLayout* layout, G... stretchlist)
	{
		int index = 0;
		[[maybe_unused]] bool arr[] = {
			([&layout ,&index](auto stretch)-> bool
			{
				if (stretch >= 0)
					layout->setColumnStretch(index, stretch);
				index++;
				return true;
			})(std::forward<G>(stretchlist))...
		};
	}

	template <typename... G>
	QButtonGroup* newbuttongroup(G... group)
	{
		QButtonGroup* buttongroup = new QButtonGroup;
		[[maybe_unused]] bool arr[] = {
			([&buttongroup](auto button)-> bool
			{
				buttongroup->addButton(button);
				return true;
			})(std::forward<G>(group))...
		};
		buttongroup->setParent(buttongroup->buttons()[0]->parent());
		return buttongroup;
	}

	template <typename T>
	T* newbutton(const QFont& font, const QString& text = QString())
	{
		auto button = new T;
		button->setFont(font);
		button->setText(text);
		newshortcut(button, Qt::Key_Return, &QAbstractButton::click);
		return button;
	}
}
