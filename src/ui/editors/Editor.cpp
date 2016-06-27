#include "Editor.hpp"

pg::Editor::Editor(Kernel* const kernel, Buffer* buffer,
                   QWidget* parent): Panel(parent),
	kernel(kernel)
{
	setFocusPolicy(Qt::StrongFocus);
	buffer->registerUpdateListener([this]()
	{
		Q_EMIT this->graphicsUpdate();
	});
	connect(this, &Editor::graphicsUpdate,
	        this, [this]()
	{
		this->repaint();
	});
}
pg::Editor::~Editor()
{
}

void pg::Editor::closeEvent(QCloseEvent* event)
{
	kernel->eraseBuffer(getBuffer());
	Panel::closeEvent(event);
}
