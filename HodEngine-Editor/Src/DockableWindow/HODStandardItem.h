#pragma once

#include <QStandardItem>
#include <Qstring>
#include "Contents.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class HODStandardItem : public QStandardItem
{
public:

	using Type = uint64_t;

public:

						HODStandardItem() = default;
						HODStandardItem(const QString& text);
						~HODStandardItem() = default;

	void				operator=(const Content&) = delete;
	void				operator=(Content&&) = delete;

public:

	virtual Type		GetType() const = 0;
	virtual const char*	GetTypeName() const = 0;
};