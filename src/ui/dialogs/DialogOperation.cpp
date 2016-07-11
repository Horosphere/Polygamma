#include "DialogOperation.hpp"

pg::DialogOperation::DialogOperation(QWidget* parent): QDialog(parent),
	buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok))
{
	buttonBox->setOrientation(Qt::Horizontal);

	connect(buttonBox, &QDialogButtonBox::accepted,
			this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected,
			this, &QDialog::reject);
}
