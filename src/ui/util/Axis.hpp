#ifndef _POLYGAMMA_UI_GRAPHICS_AXIS_HPP__
#define _POLYGAMMA_UI_GRAPHICS_AXIS_HPP__

#include <set>
#include <vector>
#include <cassert>
#include <functional>

#include <QPen>
#include <QWidget>

#include "../../math/Interval.hpp"

namespace pg
{

/**
 * @warning This widget does not adjust its width/height automatically.
 * @brief Ticked axis for graphs and plots
 */
class Axis: public QWidget
{
	Q_OBJECT

	/**
	 * Relative position of labels w.r.t. ticks
	 */
	Q_PROPERTY(Qt::Edge orientation READ getOrientation WRITE setOrientation)
	/**
	 * Base for a logarithm plot. If it is 0 then a linear plot is produced.
	 * Negative values result in the axis being inverted. The absolute value of
	 * logBase must be greater than 1.
	 */
	Q_PROPERTY(double logBase READ getLogBase WRITE setLogBase)
	/**
	 * Number of unlabeled ticks between labeled ones
	 */
	Q_PROPERTY(int nTicksMinor READ getNTicksMinor WRITE setNTicksMinor)
	/**
	 * QPen used to draw the tick
	 */
	Q_PROPERTY(QPen penTick READ getPenTick WRITE setPenTick)
	/**
	 * QPen used to draw minor ticks
	 */
	Q_PROPERTY(QPen penTickMinor READ getPenTickMinor WRITE setPenTickMinor)
	/**
	 * QPen used to draw labels
	 */
	Q_PROPERTY(QPen penText READ getPenText WRITE setPenText)
	/**
	 * Length of ticks
	 */
	Q_PROPERTY(int tickLength READ getTickLength WRITE setTickLength)
	/**
	 * Length of minor ticks.
	 */
	Q_PROPERTY(int tickLengthMinor READ getTickLengthMinor WRITE setTickLengthMinor)
public:
	explicit Axis(QWidget* parent = 0);

	void setOrientation(Qt::Edge o) noexcept
	{
		orientation = o;
	}
	Qt::Edge getOrientation() const noexcept
	{
		return orientation;
	}
	// logBase = 0 is equivalent to linear
	void setLogBase(double d) noexcept
	{
		assert(d > 1.0 || d < -1.0 || d == 0.0);
		invLogBase = d == 0.0 ? 0.0 : 1.0 / d;
	}
	double getLogBase() const noexcept
	{
		return invLogBase == 0.0 ? 0.0 : 1.0 / invLogBase;
	}
	void setNTicksMinor(int i) noexcept
	{
		nTicksMinor = i;
	}
	int getNTicksMinor() const noexcept
	{
		return nTicksMinor;
	}
	void setPenTick(QPen p) noexcept
	{
		penTick = p;
	}
	QPen getPenTick() const noexcept
	{
		return penTick;
	}
	void setPenTickMinor(QPen p) noexcept
	{
		penTickMinor = p;
	}
	QPen getPenTickMinor() const noexcept
	{
		return penTickMinor;
	}
	void setPenText(QPen p) noexcept
	{
		penText = p;
	}
	QPen getPenText() const noexcept
	{
		return penText;
	}
	void setTickLength(int l) noexcept
	{
		tickLength = l;
	}
	int getTickLength() const noexcept
	{
		return tickLength;
	}
	void setTickLengthMinor(int l) noexcept
	{
		tickLengthMinor = l;
	}
	int getTickLengthMinor() const noexcept
	{
		return tickLengthMinor;
	}

	/**
	 * \ref nSegments segments will be drawn between the position raster0 and
	 * raster1. Emits a recalculationComplete() signal.
	 * @param raster0 Position of marker 0
	 * @param raster1 Position of marker 1
	 * @param nSegments Number of segments. a.k.a. number of ticks - 1.
	 */
	void setMarks(int raster0, int raster1, int nSegments);
	/**
	 * @param Registers a callback that turns the index of the label into labels
	 */
	void setLabelingFunction(std::function<QString(int)>) noexcept;

	/**
	 * @brief Gets the effective length (in px) of this axis. Returns width()
	 *  if horizontal and height() if vertical
	 */
	int effectiveLength() const noexcept;

	std::vector<std::pair<int, QString>> ticks;
	std::vector<int> ticksMinor;

Q_SIGNALS:
	void recalculationComplete();

protected:
	virtual void paintEvent(QPaintEvent*) final;

private:
	Qt::Edge orientation; // Position of labels w.r.t. ticks
	double invLogBase;
	int nTicksMinor;
	QPen penTick;
	QPen penTickMinor;
	QPen penText;
	int tickLength;
	int tickLengthMinor;

	// Marks
	int raster0, raster1;
	int nSegments;
	std::function<QString(int)> labelingFunction;

};

class AxisInterval: public Axis
{
	Q_OBJECT

	/**
	 * Minimum width inbetween ticks
	 */
	Q_PROPERTY(int minTickWidth READ getMinTickWidth WRITE setMinTickWidth)
public:
	explicit AxisInterval(QWidget* parent = 0);

	int getMinTickWidth() const noexcept
	{
		return minTickWidth;
	}
	void setMinTickWidth(int i) noexcept
	{
		minTickWidth = i;
	}

	/**
	 * Provide a set of moduli to hint the distance (in interval) between ticks.
	 * If the moduli are too far apart or empty, a binary system is used.
	 */
	void setModuli(std::vector<long> moduli) noexcept;
	// Arguments: 1. Multiplier 2.Modulus
	void setIntervalLabelingFunction(std::function<QString(long, long)>) noexcept;
	void setIntervalLabelingFunction(std::function<std::string(long, long)>) noexcept;

	Interval<long> interval;

public Q_SLOTS:
	/**
	 * Update the interval. Calls recalculate()
	 */
	void onIntervalChanged(Interval<long>) noexcept;
	/**
	 * @brief Recalculate the position of ticks. This is automatically called by
	 * \ref onIntervalChanged(Interval<long>) and \ref resizeEvent(QResizeEvent*)
	 */
	void recalculate();


protected:
	virtual void resizeEvent(QResizeEvent*);

private:
	int minTickWidth;

	std::vector<long> moduli;

	// Calculated
	long modulus;
	long tLow;


};


// Implementations

// Axis

inline void
Axis::setLabelingFunction(std::function<QString(int)> f) noexcept
{
	labelingFunction = f;
}
inline int
Axis::effectiveLength() const noexcept
{
	return  getOrientation() == Qt::TopEdge ||
	        getOrientation() == Qt::BottomEdge ? width() : height();
}

// AxisInterval

inline void
AxisInterval::setModuli(std::vector<long> m) noexcept
{
	moduli = std::move(m);
}
inline void
AxisInterval::setIntervalLabelingFunction(std::function<QString(long, long)> f) noexcept
{
	auto lambda = [this, f](int index) -> QString
	{
		return f(this->tLow + index, modulus);
	};
	setLabelingFunction(lambda);
}
inline void
AxisInterval::setIntervalLabelingFunction(std::function<std::string(long, long)> f) noexcept
{
	auto lambda = [this, f](int index) -> QString
	{
		return QString::fromStdString(f(this->tLow + index, this->modulus));
	};
	setLabelingFunction(lambda);
}
inline void
AxisInterval::onIntervalChanged(Interval<long> i) noexcept
{
	interval = i;
	recalculate();
}

} // namespace pg

#endif // !_POLYGAMMA_UI_GRAPHICS_AXIS_HPP__
