#pragma once

#include <string>
#include <vector>
#include <QString>
#include <QVector>
#include <QPair>
#include <QJsonObject>

#include "../UID.h"
#include "../Signal.h"
#include "Hash.h"

#define BASE_META_TYPE(__ClassName__)										\
public:																		\
																			\
using Type = uint64_t;														\
																			\
virtual Type		GetType() const = 0;									\
virtual const char*	GetTypeName() const = 0;								\
																			\
private:																	\
																			\

#define CONTENT_META_TYPE(__ClassName__)									\
public:																		\
																			\
using Type = uint64_t;														\
																			\
static constexpr Type _type = Hash::CompilationTimeFnv64(#__ClassName__);	\
Type GetType() const override												\
{																			\
	return Hash::CompilationTimeFnv64(#__ClassName__);						\
}																			\
																			\
const char* GetTypeName() const override									\
{																			\
																			\
	return #__ClassName__;													\
}																			\
																			\
static const char* GetTypeNameStatic()										\
{																			\
	return #__ClassName__;													\
}																			\
																			\
private:																	\
																			\

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Content
{
	BASE_META_TYPE(Content);

public:

	using Type = uint64_t;
	using AddDependencySignal = Signal<Content*>;
	using RemoveDependencySignal = Signal<Content*>;

public:

								Content();
								Content(const Content&) = delete;
								Content(Content&&) = delete;
	virtual						~Content() = default;

	void						operator=(const Content&) = delete;
	void						operator=(Content&&) = delete;

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
	void						SetUid(const UID& uid);

	void						RemoveFile();
	void						SetPath(const QString& filepath);
	bool						SaveAtPath(const QString& filepath);

	bool						SerializeHeader(QJsonObject& header);
	bool						DeserializeHeader(const QJsonObject& header);

public:

	virtual bool				Serialize(QJsonObject& data) = 0;
	virtual bool				Deserialize(const QJsonObject& data) = 0;

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
