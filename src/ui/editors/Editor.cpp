#include "Editor.hpp"

#include <QCloseEvent>

namespace pg
{

Editor::Editor(Kernel* const kernel, Buffer const* const buffer,
               QWidget* parent): QDialog(parent),
	kernel(kernel)
{
	setWindowTitle(QString::fromStdString(buffer->getTitle()));
	setFocusPolicy(Qt::StrongFocus);
	//setAllowedAreas(Qt::AllDockWidgetAreas);
}
Editor::~Editor()
{
}

void Editor::update(Buffer::Update update)
{
	Buffer const* const buffer = this->getBuffer();
	if (buffer->isDirty())
		this->setWindowTitle("+ " + QString::fromStdString(buffer->getTitle()));
	else
		setWindowTitle(QString::fromStdString(buffer->getTitle()));
	this->repaint();
}

void Editor::closeEvent(QCloseEvent* event)
{
	Q_EMIT destroy();
	QDialog::closeEvent(event);
}

} // namespace pg
