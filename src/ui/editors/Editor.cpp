#include "Editor.hpp"

pg::Editor::Editor(Kernel* const kernel, QWidget *parent): Panel(parent),
	kernel(kernel)
{
	setFocusPolicy(Qt::StrongFocus);

}
pg::Editor::~Editor()
{
}

void pg::Editor::closeEvent(QCloseEvent* event)
{
	kernel->eraseBuffer(getBuffer());
	Panel::closeEvent(event);
}
