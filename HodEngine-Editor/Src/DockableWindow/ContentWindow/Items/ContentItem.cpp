#include "ContentItem.h"
#include "../../../Contents/Content.h"

#include <QFileIconProvider>

///
///@brief Construct a new Content Item:: Content Item object
///
///@param content 
///
ContentItem::ContentItem(Content* content)
: ContentTreeViewItem()
{
	_content = content;

	setText(_content->GetName());

	QFileIconProvider iconProvider;
	setIcon(iconProvider.icon(QFileIconProvider::File));
}

///
///@brief 
///
///@return Content* 
///
Content* ContentItem::GetContent() const
{
	return _content;
}
