#ifndef _POLYGAMMA_UI_EDITORS_EDITOR_HPP__
#define _POLYGAMMA_UI_EDITORS_EDITOR_HPP__

#include <QString>
#include <QMainWindow>

#include "../../core/Buffer.hpp"
#include "../../core/Kernel.hpp"
#include "../../core/Script.hpp"

namespace pg
{

/**
 * Each editor corresponds to a Buffer. The process of converting a buffer to
 * an editor is done in the MainWindow
 * @brief Base class for all editors.
 */
class Editor: public QMainWindow
{
	Q_OBJECT
public:
	explicit Editor(Kernel* const kernel, Buffer const* const buffer,
	                QWidget* parent = 0);
	virtual ~Editor();

	virtual bool saveAs(QString* const error) = 0;
	virtual Buffer const* getBuffer() const noexcept = 0;

Q_SIGNALS:
	void execute(Script);
	/**
	 * @brief Called in the Kernel thread. Should only be used to relay signal to
	 *  repaint()
	 */
	void graphicsUpdate();

	/**
	 * Called in the Kernel thread. Editor intercepts this signal and calls
	 * delete this
	 */
	void bufferDestoyed();

protected:
	virtual void closeEvent(QCloseEvent*) override;

	Kernel* const kernel;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_EDITORS_EDITOR_HPP__

