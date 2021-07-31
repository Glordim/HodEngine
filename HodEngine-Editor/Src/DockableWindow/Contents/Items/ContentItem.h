#pragma once

#include "../ContentTreeViewItem.h"

class Content;

///
///@brief
///
///
class ContentItem : public ContentTreeViewItem
{
	CONTENT_META_TYPE(ContentItem)

public:

				ContentItem(Content* content);
				ContentItem(const ContentItem&) = delete;
				ContentItem(ContentItem&&) = delete;
				~ContentItem() = default;

	void		operator=(const Content&) = delete;
	void		operator=(Content&&) = delete;

public:

	Content*	GetContent() const;

private:

	Content*	_content;
};
