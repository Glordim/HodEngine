#pragma once

#include <string>
#include <vector>
#include <QString>
#include <QVector>
#include <QPair>

#include "../UID.h"
#include "../Signal.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Content
{
public:

	using AddDependencySignal = Signal<Content*>;
	using RemoveDependencySignal = Signal<Content*>;

public:

								Content();
								Content(const Content& copy) = delete;
								Content(Content&&) = delete;
	virtual						~Content() = default;

	void						operator=(const Content& right) = delete;
	void						operator=(Content&& right) = delete;

public:

	const UID&					GetUID() const;
	const QString&				GetName() const;
	const QString&				GetAssetPath() const;
	const QString&				GetPath() const;
	bool						IsDirty() const;
	bool						IsValid() const;

	void						AddDependency(Content& dependency);
	void						RemoveDependency(const UID& uid);
	const QVector<Content*>&	GetAllDependencies() const;

	void						RegisterAddDependency(typename AddDependencySignal::Slot& slot);
	void						UnRegisterAddDependency(typename AddDependencySignal::Slot& slot);
	void						RegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot);
	void						UnRegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot);

	void						SetName(const QString& name);
	void						SetAssetPath(const QString& assetPath);

public:

	virtual bool				Serialize() = 0;
	virtual bool				Deserialize() = 0;

protected:

	void						SetDirty();// Bool ?

protected:

	UID							_uid;
	QString						_name;
	QString						_assetPath;
	QString						_path;
	bool						_isDirty;
	bool						_isValid;
	QVector<Content*>			_dependencies;

private:

	AddDependencySignal			_addDependencySignal;
	RemoveDependencySignal		_removeDependencySignal;
};

#include "Content.inl"