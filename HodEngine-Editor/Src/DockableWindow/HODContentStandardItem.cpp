#include "HODContentStandardItem.h"

HODContentStandardItem::HODContentStandardItem(Content* content) : HODStandardItem(content->GetName())
{
	_content = content;
}

Content* HODContentStandardItem::GetContent()
{
	return _content;
}

const void HODContentStandardItem::SetContent(Content* content)
{
	_content = content;
}
