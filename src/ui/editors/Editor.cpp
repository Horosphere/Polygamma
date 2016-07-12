#include "Editor.hpp"

#include <QCloseEvent>

pg::Editor::Editor(Kernel* const kernel, Buffer const* const buffer,
                   QWidget* parent): QMainWindow(parent),
	kernel(kernel)
{
	setFocusPolicy(Qt::StrongFocus);
	//setAllowedAreas(Qt::AllDockWidgetAreas);

	buffer->registerUpdateListener([this]()
	{
		Q_EMIT this->graphicsUpdate();
	});
	connect(this, &Editor::graphicsUpdate,
	        this, [this]()
	{
		this->repaint();
	});
	buffer->registerDestroyListener([this]()
	{
		Q_EMIT this->bufferDestoyed();
	});
	connect(this, &Editor::bufferDestoyed,
	        this, [this]()
	{
		delete this;
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
