#include "DialogScriptArgs.hpp"

pg::DialogScriptArgs::DialogScriptArgs(QWidget* parent): QDialog(parent),
	buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok))
{
	buttonBox->setOrientation(Qt::Horizontal);

	connect(buttonBox, &QDialogButtonBox::accepted,
			this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected,
			this, &QDialog::reject);
}
