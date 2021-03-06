#ifndef _POLYGAMMA_UI_EDITORS_EDITOR_HPP__
#define _POLYGAMMA_UI_EDITORS_EDITOR_HPP__

#include <QString>
#include <QDialog>

#include "../../core/Buffer.hpp"
#include "../../core/Kernel.hpp"
#include "../../core/Script.hpp"

namespace pg
{

/**
 * Each editor corresponds to a Buffer. The process of converting a buffer to
 * an editor is done in the MainWindow. All editors must be registered in the
 * MainWindow.
 * @warning One buffer cannot correspond to multiple editors at a time.
 * @brief Base class for all editors.
 */
class Editor: public QDialog
{
	Q_OBJECT
public:
	explicit Editor(Kernel* const kernel, Buffer const* const buffer,
	                QWidget* parent = 0);
	virtual ~Editor();

	virtual bool saveAs(QString* const error) = 0;
	virtual Buffer const* getBuffer() const noexcept = 0;
	virtual void update(Buffer::Update);

Q_SIGNALS:
	/**
	 * @warning Do not use this to send close signals
	 */
	void execute(Script);
	void destroy();


protected:
	virtual void closeEvent(QCloseEvent*) final override;

	Kernel* const kernel;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_EDITORS_EDITOR_HPP__

