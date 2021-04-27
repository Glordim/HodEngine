#pragma once

#include "HODStandardItem.h"
#include "Contents.h"


//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class HODContentStandardItem : public HODStandardItem
{
	CONTENT_META_TYPE(HODContentStandardItem)

public:

	using Type = uint64_t;

public:

				HODContentStandardItem() = default;
				HODContentStandardItem(Content* content);
				~HODContentStandardItem() = default;

	void		operator=(const Content&) = delete;
	void		operator=(Content&&) = delete;

public:

	Content*	GetContent(); const
	void		SetContent(Content* content);

private:

	Content*	_content;
};