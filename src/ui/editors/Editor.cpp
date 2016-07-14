#include "Editor.hpp"

#include <QCloseEvent>

pg::Editor::Editor(Kernel* const kernel, Buffer const* const buffer,
                   QWidget* parent): QDialog(parent),
	kernel(kernel)
{
	setWindowTitle(QString::fromStdString(buffer->getTitle()));
	setFocusPolicy(Qt::StrongFocus);
	//setAllowedAreas(Qt::AllDockWidgetAreas);

	buffer->registerUpdateListener([this](IntervalIndex)
	{
		Q_EMIT this->graphicsUpdate();
	});
	buffer->registerUIUpdateListener([this]()
	{
		Q_EMIT this->graphicsUpdate();
	});
	connect(this, &Editor::graphicsUpdate,
	        this, [this]()
	{
		this->repaint();
		Buffer const* const buffer = this->getBuffer();
		if (buffer->isDirty())
			this->setWindowTitle("+ " + QString::fromStdString(buffer->getTitle()));
		else
			setWindowTitle(QString::fromStdString(buffer->getTitle()));
	}, Qt::QueuedConnection);
}
pg::Editor::~Editor()
{
}

void pg::Editor::closeEvent(QCloseEvent* event)
{
	std::size_t index = kernel->bufferIndex(getBuffer());
	Q_EMIT execute(std::string(PYTHON_KERNEL) + ".eraseBuffer(" +
	               std::to_string(index) + ')');
	event->ignore();
}
