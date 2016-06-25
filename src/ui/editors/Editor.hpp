#ifndef _POLYGAMMA_UI_EDITORS_EDITOR_HPP__
#define _POLYGAMMA_UI_EDITORS_EDITOR_HPP__

#include <QString>

#include "../panels/Panel.hpp"
#include "../../core/Buffer.hpp"
#include "../../core/Kernel.hpp"

namespace pg
{

/**
 * Each editor corresponds to a Buffer. The process of converting a buffer to
 * an editor is done in the MainWindow
 * @brief Base class for all editors.
 */
class Editor: public Panel
{
	Q_OBJECT
public:
	explicit Editor(Kernel* const kernel, QWidget *parent = 0);
	virtual ~Editor();

	virtual bool saveAs(QString* const error) = 0;
	virtual Buffer* getBuffer() = 0;

protected:
	virtual void closeEvent(QCloseEvent*) override;

	Kernel* const kernel;
};

} // namespace pg

#endif // !_POLYGAMMA_UI_EDITORS_EDITOR_HPP__

