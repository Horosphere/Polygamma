#ifndef _POLYGAMMA_CORE_BUFFER_HPP__
#define _POLYGAMMA_CORE_BUFFER_HPP__

namespace pg
{

/**
 * This class uses a case pattern. E.g. if getType() == Singular, the instance
 * must be an instance of BufferSingular.
 * @brief Each instance of Buffer corresponds to a file or editable content.
 */
class Buffer
{
public:
	virtual ~Buffer();
	enum Type
	{
		Singular // Single-file audio/video streams
	};

	virtual Type getType() = 0;

};

} // namespace pg

#endif // !_POLYGAMMA_CORE_BUFFER_HPP__
