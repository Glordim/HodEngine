#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>

#include <algorithm>

namespace hod::inline editor
{
	namespace
	{
		/// @brief Delay without new change before the paths of RequestRefresh are refreshed: a save writes several files
		constexpr std::chrono::milliseconds RefreshDebounce(300);

		/// @brief The importer extensions that are text (mergeable): left out of the binary extensions of SetupWorkspace
		constexpr std::string_view TextExtensions[] = {"slang", "hlsl", "glsl", "json", "txt", "xml", "csv", "md", "yaml", "yml", "ini"};

		bool IsSameOrUnder(const Path& path, const Path& parent)
		{
			const String& pathString = path.GetString();
			const String& parentString = parent.GetString();
			if (parentString.Empty() || pathString.StartsWith(parentString) == false)
			{
				return false;
			}
			return pathString.Size() == parentString.Size() || Path::IsSeparator(parentString.Back()) || Path::IsSeparator(pathString[parentString.Size()]);
		}

		bool HasFlags(const VersionControlStatus& status)
		{
			return status._outOfDate || status._lockedByMe || status._lockedByOther || status._otherUsers.Empty() == false;
		}

		VersionControlProvider::Result NotConnected()
		{
			return VersionControlProvider::Result::Failure("No version control connected");
		}
	}

	_SingletonConstructor(VersionControl) {}

	/// @brief
	VersionControl::~VersionControl()
	{
		OnProjectClosed();
	}

	/// @brief
	/// @param project
	void VersionControl::OnProjectOpened(const Project& project)
	{
		OnProjectClosed();

		_projectOpened = true;
		_projectDir = project.GetProjectPath().ParentPath();
		_userSettingsPath = _projectDir / UserSettingsFileName;
		_projectFolders = {project.GetAssetDirPath(), project.GetSourceDirPath(), project.GetProjectSettingsDirPath(), project.GetCodeDirPath(), project.GetProjectPath()};
		_rootFolders = _projectFolders;

		LoadUserSettings();
		Connect();
	}

	/// @brief
	void VersionControl::OnProjectClosed()
	{
		if (_projectOpened == false)
		{
			return;
		}
		Disconnect();
		_projectOpened = false;
		_projectDir.Clear();
		_userSettingsPath.Clear();
		_projectFolders.Clear();
		_rootFolders.Clear();
	}

	/// @brief
	/// @param editorFocused
	void VersionControl::Update(bool editorFocused)
	{
		if (editorFocused && _editorFocused == false)
		{
			RequestRefresh(); // Back from a terminal, a git client...
		}
		_editorFocused = editorFocused;

		Vector<Completion> completions;
		{
			std::scoped_lock lock(_queueLock);
			completions = std::move(_completions);
			_completions.Clear();
		}
		for (Completion& completion : completions)
		{
			if (completion._result._success == false)
			{
				OUTPUT_ERROR("Version control, {}: {}", completion._name, completion._result._error);
			}
			if (completion._callback != nullptr)
			{
				completion._callback(completion._result);
			}
		}
	}

	/// @brief
	/// @param provider
	/// @return
	bool VersionControl::RegisterProvider(VersionControlProvider& provider)
	{
		for (const VersionControlProvider* registered : _providers)
		{
			if (registered->GetName() == provider.GetName())
			{
				return false;
			}
		}
		_providers.PushBack(&provider);

		if (_projectOpened && _provider == nullptr)
		{
			Connect(); // Its module reloaded, or a provider the user chose that wasn't there
		}
		return true;
	}

	/// @brief
	/// @param provider
	void VersionControl::UnregisterProvider(VersionControlProvider& provider)
	{
		if (_provider == &provider)
		{
			Disconnect();
		}
		for (uint32_t index = 0; index < _providers.Size(); ++index)
		{
			if (_providers[index] == &provider)
			{
				_providers.Erase(index);
				break;
			}
		}
	}

	/// @brief
	/// @return
	const Vector<VersionControlProvider*>& VersionControl::GetProviders() const
	{
		return _providers;
	}

	/// @brief
	/// @param choice
	void VersionControl::SetProviderChoice(std::string_view choice)
	{
		_providerChoice = choice;
		SaveUserSettings();
		if (_projectOpened)
		{
			Connect();
		}
	}

	/// @brief
	/// @param provider
	void VersionControl::CreateWorkspace(std::string_view provider)
	{
		if (_projectOpened == false)
		{
			return;
		}
		_providerChoice = provider;
		_createWorkspace = true; // Taken into account by Connect
		SaveUserSettings();
		Connect();
	}

	/// @brief
	/// @return
	std::string_view VersionControl::GetProviderChoice() const
	{
		return _providerChoice;
	}

	/// @brief
	/// @return
	const VersionControlProvider* VersionControl::GetConnectedProvider() const
	{
		return _connected ? _provider : nullptr;
	}

	/// @brief
	/// @return
	bool VersionControl::IsConnected() const
	{
		return _connected;
	}

	/// @brief
	/// @return
	String VersionControl::GetConnectionError() const
	{
		std::scoped_lock lock(_connectionErrorLock);
		return _connectionError;
	}

	/// @brief
	/// @return
	uint32_t VersionControl::GetCapabilities() const
	{
		return _connected ? _capabilities.load() : VersionControlProvider::Capability::None;
	}

	/// @brief
	/// @param capability
	/// @return
	bool VersionControl::HasCapability(uint32_t capability) const
	{
		return (GetCapabilities() & capability) == capability;
	}

	/// @brief
	/// @param capability
	/// @return
	String VersionControl::GetCapabilityHint(uint32_t capability) const
	{
		if (_connected == false)
		{
			return "No version control connected";
		}
		if (HasCapability(capability))
		{
			return String();
		}
		std::scoped_lock lock(_connectionErrorLock);
		for (uint32_t bit = 0; bit < VersionControlProvider::Capability::Count; ++bit)
		{
			if (capability == (1u << bit))
			{
				return _capabilityHints[bit];
			}
		}
		return String();
	}

	/// @brief
	/// @param path
	/// @return
	VersionControlStatus VersionControl::GetStatus(const Path& path) const
	{
		if (_connected == false)
		{
			return VersionControlStatus();
		}
		std::scoped_lock lock(_statusLock);
		return GetCachedStatus(path);
	}

	/// @brief
	/// @param path
	/// @return
	VersionControlStatus VersionControl::GetCachedStatus(const Path& path) const
	{
		VersionControlStatus status;
		if (_hasStatuses == false)
		{
			return status;
		}
		if (IsExcluded(path))
		{
			status._state = VersionControlState::Ignored;
			return status;
		}
		if (IsUnderRootFolders(path) == false)
		{
			return status;
		}
		auto it = _statuses.find(path);
		if (it != _statuses.end())
		{
			return it->second;
		}
		status._state = VersionControlState::Unchanged;
		return status;
	}

	/// @brief
	/// @param asset
	/// @return
	VersionControlStatus VersionControl::GetAssetStatus(const Asset& asset) const
	{
		if (_connected == false)
		{
			return VersionControlStatus();
		}
		const Vector<Path>   files = GetAssetFiles(asset);
		std::scoped_lock     lock(_statusLock);
		VersionControlStatus status;
		for (const Path& file : files)
		{
			status = VersionControlStatus::Combine(status, GetCachedStatus(file));
		}
		return status;
	}

	/// @brief
	/// @param folder
	/// @return
	VersionControlStatus VersionControl::GetFolderStatus(const Path& folder) const
	{
		if (_connected == false)
		{
			return VersionControlStatus();
		}
		std::scoped_lock     lock(_statusLock);
		VersionControlStatus status;
		if (_hasStatuses == false || IsUnderRootFolders(folder) == false)
		{
			return status;
		}
		auto it = _folderStatuses.find(folder);
		if (it != _folderStatuses.end())
		{
			return it->second;
		}
		status._state = VersionControlState::Unchanged;
		return status;
	}

	/// @brief
	/// @return
	uint32_t VersionControl::GetStatusVersion() const
	{
		return _statusVersion;
	}

	/// @brief
	/// @return
	const Vector<Path>& VersionControl::GetProjectFolders() const
	{
		return _projectFolders;
	}

	/// @brief
	/// @return
	uint32_t VersionControl::GetOutOfDateCount() const
	{
		return _connected ? _outOfDateCount.load() : 0;
	}

	/// @brief
	/// @return
	uint32_t VersionControl::GetPendingPushCount() const
	{
		return _connected ? _pendingPushCount.load() : 0;
	}

	/// @brief
	/// @return
	Vector<VersionControl::Change> VersionControl::GetChanges() const
	{
		Vector<Change> changes;
		if (_connected == false)
		{
			return changes;
		}
		{
			std::scoped_lock lock(_statusLock);
			for (const auto& [path, status] : _statuses)
			{
				switch (status._state)
				{
					case VersionControlState::Untracked:
					case VersionControlState::Added:
					case VersionControlState::Modified:
					case VersionControlState::Deleted:
					case VersionControlState::Renamed:
					case VersionControlState::Conflicted: changes.PushBack({path, status}); break;
					case VersionControlState::Unknown:
					case VersionControlState::Ignored:
					case VersionControlState::Unchanged: break;
				}
			}
		}
		std::sort(changes.begin(), changes.end(), [](const Change& left, const Change& right) { return left._path < right._path; });
		return changes;
	}

	/// @brief
	/// @return
	bool VersionControl::IsBusy() const
	{
		if (_busy)
		{
			return true;
		}
		std::scoped_lock lock(_queueLock);
		return _queue.Empty() == false || _connectRequested || _pendingFullRefresh || _pendingRefreshPaths.Empty() == false;
	}

	/// @brief The sources out of the project (imported from elsewhere) left out
	/// @param asset
	/// @return
	Vector<Path> VersionControl::GetAssetFiles(const Asset& asset)
	{
		Vector<Path> files;
		files.PushBack(asset.GetPath());

		const Project* project = Project::GetInstance();
		const Path     projectDir = project != nullptr ? project->GetProjectPath().ParentPath() : Path();
		for (const AssetContainer::SourceInfo& source : asset.GetSources())
		{
			if (projectDir.Empty() == false && IsSameOrUnder(source._path, projectDir))
			{
				files.PushBack(source._path);
			}
		}
		return files;
	}

	/// @brief
	void VersionControl::RequestRefresh()
	{
		if (_connected == false)
		{
			return;
		}
		{
			std::scoped_lock lock(_queueLock);
			_pendingFullRefresh = true;
		}
		_queueCondition.notify_all();
	}

	/// @brief
	/// @param paths
	void VersionControl::RequestRefresh(ArrayView<Path> paths)
	{
		if (_connected == false)
		{
			return;
		}
		{
			std::scoped_lock lock(_queueLock);
			for (const Path& path : paths)
			{
				if (IsUnderRootFolders(path) && IsExcluded(path) == false)
				{
					_pendingRefreshPaths.PushBack(path);
				}
			}
			_pendingRefreshTime = Clock::now() + RefreshDebounce;
		}
		_queueCondition.notify_all();
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::Add(Vector<Path> paths, Callback callback)
	{
		EnqueueAsync(
			"Add", FilterPaths(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Add(operationPaths); }, std::move(callback));
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::Revert(Vector<Path> paths, Callback callback)
	{
		paths = FilterPaths(paths);
		{
			std::scoped_lock lock(_editCheckoutsLock);
			for (const Path& path : paths)
			{
				std::erase_if(_editCheckouts, [&path](const Path& editCheckout) { return IsSameOrUnder(editCheckout, path); });
			}
		}
		EnqueueAsync(
			"Revert", std::move(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Revert(operationPaths); }, std::move(callback));
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::Checkout(Vector<Path> paths, Callback callback)
	{
		paths = FilterPaths(paths);
		{
			std::scoped_lock lock(_editCheckoutsLock);
			for (const Path& path : paths)
			{
				std::erase_if(_editCheckouts, [&path](const Path& editCheckout) { return IsSameOrUnder(editCheckout, path); }); // The user's now
			}
		}
		EnqueueAsync(
			"Checkout", std::move(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Checkout(operationPaths); },
			std::move(callback));
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::Lock(Vector<Path> paths, Callback callback)
	{
		EnqueueAsync(
			"Lock", FilterPaths(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Lock(operationPaths); }, std::move(callback));
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::Unlock(Vector<Path> paths, Callback callback)
	{
		EnqueueAsync(
			"Unlock", FilterPaths(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Unlock(operationPaths); },
			std::move(callback));
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::GetLatest(Vector<Path> paths, Callback callback)
	{
		EnqueueAsync(
			"Get latest", FilterPaths(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.GetLatest(operationPaths); },
			std::move(callback));
	}

	/// @brief
	/// @param callback
	void VersionControl::UpdateWorkspace(Callback callback)
	{
		EnqueueAsync(
			"Pull", _rootFolders,
			[this](VersionControlProvider& provider, const Vector<Path>& operationPaths)
			{
				Result result = provider.UpdateWorkspace(operationPaths);
				{
					std::scoped_lock lock(_queueLock);
					_pendingFullRefresh = true; // Even failed: part of it may have been done
				}
				return result;
			},
			std::move(callback), false);
	}

	/// @brief
	/// @param paths
	/// @param message
	/// @param push
	/// @param callback
	void VersionControl::Submit(Vector<Path> paths, String message, bool push, Callback callback)
	{
		if (std::string_view(message).find_first_not_of(" \t\r\n") == std::string_view::npos)
		{
			PostCompletion("Submit", Result::Failure("A message is needed"), std::move(callback));
			return;
		}
		paths = FilterPaths(paths);
		{
			std::scoped_lock lock(_statusLock);
			const uint32_t   count = paths.Size();
			for (uint32_t index = 0; index < count; ++index)
			{
				const VersionControlStatus status = GetCachedStatus(paths[index]);
				if (status._state == VersionControlState::Renamed && status._originalPath.Empty() == false)
				{
					paths.PushBack(status._originalPath);
				}
			}
		}

		EnqueueAsync(
			push ? "Submit and push" : "Submit", std::move(paths),
			[this, message = std::move(message), push](VersionControlProvider& provider, const Vector<Path>& operationPaths)
			{
				Result result = provider.Submit(operationPaths, message);
				if (result._success && push)
				{
					result = provider.Push();
					std::scoped_lock lock(_queueLock);
					_pendingFullRefresh = true; // Locks released, out of date files...
				}
				return result;
			},
			std::move(callback));
	}

	/// @brief
	/// @param callback
	void VersionControl::Push(Callback callback)
	{
		EnqueueAsync(
			"Push", _rootFolders,
			[this](VersionControlProvider& provider, const Vector<Path>& operationPaths)
			{
				(void)operationPaths;
				Result           result = provider.Push();
				std::scoped_lock lock(_queueLock);
				_pendingFullRefresh = true;
				return result;
			},
			std::move(callback), false);
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::RevertUnchanged(Vector<Path> paths, Callback callback)
	{
		EnqueueAsync(
			"Revert unchanged", FilterPaths(paths), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.RevertUnchanged(operationPaths); },
			std::move(callback));
	}

	/// @brief
	/// @param path
	void VersionControl::OpenDiff(const Path& path)
	{
		EnqueueAsync("Diff", {path}, [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.OpenDiff(operationPaths[0]); }, nullptr, false);
	}

	/// @brief
	/// @param path
	void VersionControl::OpenHistory(const Path& path)
	{
		EnqueueAsync(
			"History", {path}, [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.OpenHistory(operationPaths[0]); }, nullptr, false);
	}

	/// @brief
	/// @param paths
	/// @param callback
	void VersionControl::PrepareEdit(Vector<Path> paths, Callback callback)
	{
		paths = FilterPaths(paths);

		String lockedByOthers;
		if (_checkoutOnEdit == false || HasCapability(VersionControlProvider::Capability::Checkout) == false ||
		    (GetFilesToCheckout(paths, lockedByOthers).Empty() && lockedByOthers.Empty()))
		{
			if (callback != nullptr)
			{
				PostCompletion("Checkout for edit", Result(), std::move(callback));
			}
			return;
		}

		EnqueueAsync(
			"Checkout for edit", std::move(paths),
			[this](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return CheckoutToWrite(provider, operationPaths, true); }, std::move(callback));
	}

	/// @brief
	/// @param paths
	void VersionControl::ReleaseEdit(Vector<Path> paths)
	{
		Vector<Path> released;
		{
			std::scoped_lock lock(_editCheckoutsLock);
			for (const Path& path : paths)
			{
				if (_editCheckouts.erase(path) > 0)
				{
					released.PushBack(path);
				}
			}
		}
		if (released.Empty() == false && _connected)
		{
			EnqueueAsync(
				"Release unchanged", std::move(released),
				[](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.RevertUnchanged(operationPaths); }, nullptr);
		}
	}

	/// @brief
	/// @param paths
	/// @param error
	/// @return
	bool VersionControl::PrepareWrite(ArrayView<Path> paths, String* error)
	{
		if (HasCapability(VersionControlProvider::Capability::Checkout) == false)
		{
			return true;
		}

		Vector<Path> filtered = FilterPaths(paths);
		String       lockedByOthers;
		if (GetFilesToCheckout(filtered, lockedByOthers).Empty() && lockedByOthers.Empty())
		{
			return true; // Nothing to do, as far as the cache knows: no need to wait for the worker
		}

		Result result = RunSynchronous("Checkout to write", std::move(filtered),
		                               [this](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return CheckoutToWrite(provider, operationPaths, false); });
		if (result._success == false && error != nullptr)
		{
			*error = result._error;
		}
		return result._success;
	}

	/// @brief
	/// @param from
	/// @param to
	/// @param error
	/// @return
	VersionControl::Delegation VersionControl::Move(const Path& from, const Path& to, String* error)
	{
		if (HasCapability(VersionControlProvider::Capability::TrackMoves) == false || IsUnderRootFolders(from) == false || IsUnderRootFolders(to) == false)
		{
			return Delegation::NotHandled;
		}
		const VersionControlState state = GetStatus(from)._state;
		if (state == VersionControlState::Unknown || state == VersionControlState::Untracked || state == VersionControlState::Ignored)
		{
			return Delegation::NotHandled;
		}

		Result result = RunSynchronous("Move", {from, to},
		                               [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Move(operationPaths[0], operationPaths[1]); });
		if (result._success == false && error != nullptr)
		{
			*error = result._error;
		}
		return result._success ? Delegation::Done : Delegation::Failed;
	}

	/// @brief The tracked ones through the provider, the others deleted on disk
	/// @param paths
	/// @param error
	/// @return
	VersionControl::Delegation VersionControl::Delete(ArrayView<Path> paths, String* error)
	{
		if (HasCapability(VersionControlProvider::Capability::TrackMoves) == false)
		{
			return Delegation::NotHandled;
		}

		Vector<Path> tracked;
		Vector<Path> untracked;
		for (const Path& path : paths)
		{
			const VersionControlState state = IsUnderRootFolders(path) ? GetStatus(path)._state : VersionControlState::Unknown;
			if (state == VersionControlState::Unknown || state == VersionControlState::Untracked || state == VersionControlState::Ignored)
			{
				untracked.PushBack(path);
			}
			else
			{
				tracked.PushBack(path);
			}
		}
		if (tracked.Empty())
		{
			return Delegation::NotHandled;
		}

		Result result =
			RunSynchronous("Delete", std::move(tracked), [](VersionControlProvider& provider, const Vector<Path>& operationPaths) { return provider.Delete(operationPaths); });
		for (const Path& path : untracked)
		{
			FileSystem::GetInstance()->RemoveAll(path);
		}
		if (result._success == false && error != nullptr)
		{
			*error = result._error;
		}
		return result._success ? Delegation::Done : Delegation::Failed;
	}

	/// @brief
	/// @param paths
	void VersionControl::OnFilesCreated(ArrayView<Path> paths)
	{
		if (_autoAdd == false || _connected == false)
		{
			return;
		}
		Vector<Path> filtered = FilterPaths(paths);
		if (filtered.Empty() == false)
		{
			Add(std::move(filtered));
		}
	}

	/// @brief
	/// @param autoAdd
	void VersionControl::SetAutoAdd(bool autoAdd)
	{
		_autoAdd = autoAdd;
		SaveUserSettings();
	}

	/// @brief
	/// @return
	bool VersionControl::GetAutoAdd() const
	{
		return _autoAdd;
	}

	/// @brief
	/// @param checkoutOnEdit
	void VersionControl::SetCheckoutOnEdit(bool checkoutOnEdit)
	{
		_checkoutOnEdit = checkoutOnEdit;
		SaveUserSettings();
	}

	/// @brief
	/// @return
	bool VersionControl::GetCheckoutOnEdit() const
	{
		return _checkoutOnEdit;
	}

	/// @brief Main thread
	void VersionControl::Connect()
	{
		Disconnect();
		if (_projectOpened == false || _providerChoice == "None")
		{
			return;
		}

		VersionControlProvider* chosen = nullptr;
		if (_providerChoice == "Auto")
		{
			for (VersionControlProvider* provider : _providers)
			{
				if (provider->Detect(_projectDir))
				{
					chosen = provider;
					break;
				}
			}
			if (chosen == nullptr)
			{
				return; // Not versioned (or by a system no module offers): nothing to say
			}
		}
		else
		{
			for (VersionControlProvider* provider : _providers)
			{
				if (provider->GetName() == std::string_view(_providerChoice))
				{
					chosen = provider;
					break;
				}
			}
			if (chosen == nullptr)
			{
				std::scoped_lock lock(_connectionErrorLock);
				_connectionError = String::Format("%s isn't available (its editor module isn't loaded)", _providerChoice.CStr());
				return;
			}
			if (_createWorkspace == false && chosen->CanCreateWorkspace() && chosen->Detect(_projectDir) == false)
			{
				// Its Detect is reliable (a .git up the folders): its own error wouldn't say it as clearly. VersionControlGui offers the creation
				std::scoped_lock lock(_connectionErrorLock);
				_connectionError = String::Format("The project isn't in a %s repository", _providerChoice.CStr());
				return;
			}
		}

		_provider = chosen;
		_createWorkspaceOnConnect = false;
		if (_createWorkspace)
		{
			// Taken into account once, whatever its result: an error says why, the user can do it by hand then
			_createWorkspace = false;
			SaveUserSettings();
			_createWorkspaceOnConnect = _provider->Detect(_projectDir) == false;
		}
		_workspaceSetup = MakeWorkspaceSetup();
		_rootFolders = _projectFolders;
		for (Path& workspaceFile : _provider->GetWorkspaceFiles(_projectDir))
		{
			_rootFolders.PushBack(std::move(workspaceFile));
		}
		{
			std::scoped_lock lock(_queueLock);
			_connectRequested = true;
		}
		StartWorker();
		_queueCondition.notify_all();
	}

	/// @brief Main thread
	void VersionControl::Disconnect()
	{
		StopWorker();
		if (_provider != nullptr)
		{
			_provider->Disconnect();
			_provider = nullptr;
		}
		_connected = false;
		_capabilities = VersionControlProvider::Capability::None;
		_pendingPushCount = 0;
		{
			std::scoped_lock lock(_connectionErrorLock);
			_connectionError.Clear();
		}
		{
			std::scoped_lock lock(_statusLock);
			_hasStatuses = false;
			_statuses.clear();
			_folderStatuses.clear();
		}
		_outOfDateCount = 0;
		++_statusVersion;
		{
			std::scoped_lock lock(_editCheckoutsLock);
			_editCheckouts.clear();
		}
	}

	/// @brief
	void VersionControl::StartWorker()
	{
		if (_workerRunning)
		{
			return;
		}
		{
			std::scoped_lock lock(_queueLock);
			_stopWorker = false;
		}
		_workerRunning = true;
		_worker.Start(&VersionControl::WorkerEntry, this, Thread::Priority::Low, "VersionControl");
	}

	/// @brief
	void VersionControl::StopWorker()
	{
		if (_workerRunning == false)
		{
			return;
		}
		{
			std::scoped_lock lock(_queueLock);
			_stopWorker = true;
		}
		_queueCondition.notify_all();
		_worker.Join();
		_workerRunning = false;

		std::scoped_lock lock(_queueLock);
		_queue.Clear();
		_completions.Clear(); // Their callbacks may be of a module unloading
		_connectRequested = false;
		_pendingFullRefresh = false;
		_pendingRefreshPaths.Clear();
		_lastRefreshError.Clear();
	}

	/// @brief
	/// @param operation
	void VersionControl::Enqueue(Operation operation)
	{
		{
			std::scoped_lock lock(_queueLock);
			_queue.PushBack(std::move(operation));
		}
		_queueCondition.notify_all();
	}

	/// @brief
	/// @param name
	/// @param paths
	/// @param execute
	/// @param callback
	/// @param refreshPaths
	void VersionControl::EnqueueAsync(std::string_view name, Vector<Path> paths, Execute execute, Callback callback, bool refreshPaths)
	{
		if (_connected == false)
		{
			PostCompletion(name, NotConnected(), std::move(callback));
			return;
		}
		if (paths.Empty())
		{
			PostCompletion(name, Result(), std::move(callback));
			return;
		}

		Operation operation;
		operation._name = name;
		operation._paths = std::move(paths);
		operation._execute = std::move(execute);
		operation._callback = std::move(callback);
		operation._refreshPaths = refreshPaths;
		Enqueue(std::move(operation));
	}

	/// @brief
	/// @param name
	/// @param paths
	/// @param execute
	/// @return
	VersionControl::Result VersionControl::RunSynchronous(std::string_view name, Vector<Path> paths, Execute execute)
	{
		if (_connected == false)
		{
			return NotConnected();
		}

		std::shared_ptr<SyncState> sync = std::make_shared<SyncState>();
		Operation                  operation;
		operation._name = name;
		operation._paths = std::move(paths);
		operation._execute = std::move(execute);
		operation._sync = sync;
		Enqueue(std::move(operation));

		std::unique_lock lock(_queueLock);
		_syncCondition.wait(lock, [&sync]() { return sync->_done; });
		return sync->_result;
	}

	/// @brief
	/// @param name
	/// @param result
	/// @param callback
	void VersionControl::PostCompletion(std::string_view name, const Result& result, Callback callback)
	{
		std::scoped_lock lock(_queueLock);
		Completion&      completion = _completions.EmplaceBack();
		completion._name = name;
		completion._result = result;
		completion._callback = std::move(callback);
	}

	/// @brief Worker
	/// @param provider
	/// @param paths
	/// @param editCheckouts
	/// @return
	VersionControl::Result VersionControl::CheckoutToWrite(VersionControlProvider& provider, const Vector<Path>& paths, bool editCheckouts)
	{
		String             lockedByOthers;
		const Vector<Path> toCheckout = GetFilesToCheckout(paths, lockedByOthers);
		if (lockedByOthers.Empty() == false)
		{
			return Result::Failure(lockedByOthers);
		}
		if (toCheckout.Empty())
		{
			return Result();
		}

		Result result = provider.Checkout(toCheckout);
		if (result._success && editCheckouts)
		{
			std::scoped_lock lock(_editCheckoutsLock);
			for (const Path& path : toCheckout)
			{
				_editCheckouts.insert(path);
			}
		}
		return result;
	}

	/// @brief
	/// @param parameter
	/// @return
	int VersionControl::WorkerEntry(void* parameter)
	{
		static_cast<VersionControl*>(parameter)->WorkerLoop();
		return 0;
	}

	/// @brief The operations first (the user waits for them), then the refreshes. Stopping: the operations queued are run, not the
	/// refreshes
	void VersionControl::WorkerLoop()
	{
		std::unique_lock lock(_queueLock);
		while (true)
		{
			if (_connectRequested)
			{
				_connectRequested = false;
				_busy = true;
				lock.unlock();
				WorkerConnect();
				lock.lock();
				continue;
			}

			if (_queue.Empty() == false)
			{
				Operation operation = std::move(_queue[0]);
				_queue.Erase(_queue.Begin());
				_busy = true;
				lock.unlock();
				WorkerRun(operation);
				lock.lock();
				continue;
			}

			if (_stopWorker)
			{
				break;
			}

			if (_connected && _pendingFullRefresh)
			{
				_pendingFullRefresh = false;
				_pendingRefreshPaths.Clear(); // Included
				_busy = true;
				lock.unlock();
				WorkerRefresh(true, Vector<Path>());
				lock.lock();
				continue;
			}

			if (_connected && _pendingRefreshPaths.Empty() == false)
			{
				if (Clock::now() >= _pendingRefreshTime)
				{
					Vector<Path> paths = std::move(_pendingRefreshPaths);
					_pendingRefreshPaths.Clear();
					_busy = true;
					lock.unlock();
					WorkerRefresh(false, std::move(paths));
					lock.lock();
					continue;
				}
				_busy = false;
				_queueCondition.wait_until(lock, _pendingRefreshTime);
				continue;
			}

			_busy = false;
			_queueCondition.wait(lock);
		}
		_busy = false;
	}

	/// @brief
	void VersionControl::WorkerConnect()
	{
		bool workspaceCreated = false;
		if (_createWorkspaceOnConnect)
		{
			_createWorkspaceOnConnect = false;
			Result created = _provider->CreateWorkspace(_projectDir);
			if (created._success == false)
			{
				OUTPUT_ERROR("Version control: can't create the {} workspace of the project: {}", _provider->GetName(), created._error);
			}
			else
			{
				OUTPUT_MESSAGE("Version control: {} workspace created in {}", _provider->GetName(), _projectDir);
				workspaceCreated = true;
			}
		}

		Result result = _provider->Connect(_projectDir);
		if (result._success == false)
		{
			OUTPUT_WARNING("Version control: can't connect to {}: {}", _provider->GetName(), result._error);
			std::scoped_lock lock(_connectionErrorLock);
			_connectionError = result._error;
			return;
		}

		Result setup = _provider->SetupWorkspace(_projectDir, _workspaceSetup);
		if (setup._success == false)
		{
			OUTPUT_WARNING("Version control: can't set the workspace up: {}", setup._error);
		}

		if (workspaceCreated)
		{
			// What describes the workspace and the project, added right away: the project file and the provider's files SetupWorkspace
			// just created (.gitignore, .gitattributes). The files of the root folders, not the folders' content: the user's choice
			Vector<Path> files;
			for (const Path& rootFolder : _rootFolders)
			{
				if (FileSystem::GetInstance()->IsRegularFile(rootFolder))
				{
					files.PushBack(rootFolder);
				}
			}
			if (files.Empty() == false)
			{
				Result added = _provider->Add(files);
				if (added._success == false)
				{
					OUTPUT_WARNING("Version control: can't add the files of the new workspace: {}", added._error);
				}
			}
		}

		UpdateCapabilities();
		{
			std::scoped_lock lock(_connectionErrorLock);
			_connectionError.Clear();
		}
		_connected = true;
		OUTPUT_MESSAGE("Version control: {} connected", _provider->GetName());

		WorkerRefresh(true, Vector<Path>());
	}

	/// @brief
	/// @param fullRefresh
	/// @param paths
	void VersionControl::WorkerRefresh(bool fullRefresh, Vector<Path> paths)
	{
		if (fullRefresh == false)
		{
			std::sort(paths.begin(), paths.end());
			const uint32_t uniqueCount = static_cast<uint32_t>(std::unique(paths.begin(), paths.end()) - paths.begin());
			paths.Erase(uniqueCount, paths.Size() - uniqueCount);
		}

		VersionControlProvider::StatusMap statuses;
		Result                            result = fullRefresh ? _provider->QueryStatus(_rootFolders, statuses) : _provider->QueryFileStatus(paths, statuses);
		if (fullRefresh)
		{
			UpdateCapabilities(); // An upstream branch set since...
			_pendingPushCount = _provider->GetPendingPushCount();
		}

		if (result._success == false)
		{
			if (result._error != _lastRefreshError)
			{
				OUTPUT_WARNING("Version control: can't refresh the statuses: {}", result._error);
				_lastRefreshError = result._error;
			}
			return;
		}
		_lastRefreshError.Clear();
		ApplyStatuses(std::move(statuses), fullRefresh, paths);
	}

	/// @brief
	/// @param operation
	void VersionControl::WorkerRun(Operation& operation)
	{
		TaskTracker* taskTracker = (operation._sync == nullptr && Editor::GetInstance() != nullptr) ? &Editor::GetInstance()->GetTaskTracker() : nullptr;
		uint64_t     taskId = 0;
		if (taskTracker != nullptr)
		{
			const String description = operation._paths.Size() == 1 ? operation._paths[0].GetString() : String::Format("%u files", operation._paths.Size());
			taskId = taskTracker->CreateTask(operation._name, description);
			taskTracker->UpdateTaskStatus(taskId, TaskStatus::Running);
		}

		Result result = (_connected && _provider != nullptr) ? operation._execute(*_provider, operation._paths) : NotConnected();

		if (_connected)
		{
			_pendingPushCount = _provider->GetPendingPushCount();
		}
		if (_connected && operation._refreshPaths)
		{
			VersionControlProvider::StatusMap statuses;
			if (_provider->QueryFileStatus(operation._paths, statuses)._success)
			{
				ApplyStatuses(std::move(statuses), false, operation._paths);
			}
		}

		if (taskTracker != nullptr)
		{
			taskTracker->UpdateTaskStatus(taskId, result._success ? TaskStatus::Succeeded : TaskStatus::Failed);
		}

		if (operation._sync != nullptr)
		{
			{
				std::scoped_lock lock(_queueLock);
				operation._sync->_result = std::move(result);
				operation._sync->_done = true;
			}
			_syncCondition.notify_all();
		}
		else
		{
			PostCompletion(operation._name, result, std::move(operation._callback));
		}
	}

	/// @brief The capabilities and the hints of the missing ones
	void VersionControl::UpdateCapabilities()
	{
		const uint32_t capabilities = _provider->GetCapabilities();
		_capabilities = capabilities;
		std::scoped_lock lock(_connectionErrorLock);
		for (uint32_t bit = 0; bit < VersionControlProvider::Capability::Count; ++bit)
		{
			const uint32_t capability = 1u << bit;
			_capabilityHints[bit] = (capabilities & capability) != 0 ? String() : _provider->GetCapabilityHint(capability);
		}
	}

	/// @brief
	/// @param statuses
	/// @param fullRefresh
	/// @param refreshedPaths
	void VersionControl::ApplyStatuses(VersionControlProvider::StatusMap&& statuses, bool fullRefresh, ArrayView<Path> refreshedPaths)
	{
		std::scoped_lock lock(_statusLock);
		if (fullRefresh)
		{
			_statuses.clear();
		}
		else
		{
			std::erase_if(_statuses,
			              [&refreshedPaths](const auto& entry)
			              {
							  for (const Path& refreshedPath : refreshedPaths)
							  {
								  if (IsSameOrUnder(entry.first, refreshedPath))
								  {
									  return true;
								  }
							  }
							  return false;
						  });
		}

		for (auto& [path, status] : statuses)
		{
			if (IsExcluded(path) || (status._state == VersionControlState::Unchanged && HasFlags(status) == false))
			{
				continue;
			}
			_statuses[path] = std::move(status);
		}

		if (fullRefresh)
		{
			_hasStatuses = true;
		}
		RebuildFolderStatuses();
		++_statusVersion;
	}

	/// @brief Each status combined into its folders, up to its root folder. The ignored files don't make their folder ignored
	void VersionControl::RebuildFolderStatuses()
	{
		_folderStatuses.clear();
		uint32_t outOfDateCount = 0;
		for (const auto& [path, status] : _statuses)
		{
			outOfDateCount += status._outOfDate ? 1 : 0;

			VersionControlStatus propagated = status;
			if (propagated._state == VersionControlState::Ignored)
			{
				propagated._state = VersionControlState::Unchanged;
			}

			Path folder = path.ParentPath();
			while (IsUnderRootFolders(folder))
			{
				VersionControlStatus& folderStatus = _folderStatuses[folder];
				if (folderStatus._state == VersionControlState::Unknown)
				{
					folderStatus._state = VersionControlState::Unchanged;
				}
				folderStatus = VersionControlStatus::Combine(folderStatus, propagated);

				Path parent = folder.ParentPath();
				if (parent == folder)
				{
					break;
				}
				folder = parent;
			}
		}
		_outOfDateCount = outOfDateCount;
	}

	/// @brief
	/// @param paths
	/// @return
	Vector<Path> VersionControl::FilterPaths(ArrayView<Path> paths) const
	{
		Vector<Path> filtered;
		filtered.Reserve(paths.Size());
		for (const Path& path : paths)
		{
			if (IsUnderRootFolders(path) && IsExcluded(path) == false)
			{
				filtered.PushBack(path);
			}
		}
		return filtered;
	}

	/// @brief
	/// @param path
	/// @return
	bool VersionControl::IsUnderRootFolders(const Path& path) const
	{
		for (const Path& rootFolder : _rootFolders)
		{
			if (IsSameOrUnder(path, rootFolder))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param path
	/// @return
	bool VersionControl::IsExcluded(const Path& path) const
	{
		return path == _userSettingsPath;
	}

	/// @brief Unknown ones (not refreshed yet) included: the provider tells
	/// @param paths
	/// @param lockedByOthers
	/// @return
	Vector<Path> VersionControl::GetFilesToCheckout(ArrayView<Path> paths, String& lockedByOthers) const
	{
		Vector<Path>     files;
		std::scoped_lock lock(_statusLock);
		for (const Path& path : paths)
		{
			if (FileSystem::GetInstance()->IsRegularFile(path) == false)
			{
				continue; // A new file, or a folder
			}

			const VersionControlStatus status = GetCachedStatus(path);
			if (status._lockedByOther)
			{
				lockedByOthers += lockedByOthers.Empty() ? String() : String("\n");
				lockedByOthers += String::Format("%s is locked by %s", path.GetString().CStr(), status._otherUsers.Empty() ? "someone else" : status._otherUsers.CStr());
				continue;
			}
			if (status._lockedByMe)
			{
				continue;
			}
			if (status._state == VersionControlState::Unchanged || status._state == VersionControlState::Unknown)
			{
				files.PushBack(path);
			}
		}
		return files;
	}

	/// @brief Main thread, from the project and the registered asset types and importers
	/// @return
	VersionControlProvider::WorkspaceSetup VersionControl::MakeWorkspaceSetup() const
	{
		VersionControlProvider::WorkspaceSetup setup;
		setup._ignoredFolders = {_projectDir / "Cache", _projectDir / "Build"};
		setup._ignoredFiles = {_userSettingsPath}; // Per user (the CMakeLists.txt are the project's, versioned, see Project::GenerateProjectCMakeLists)
		setup._ignoredNames = {".DS_Store"};       // macOS's Finder, in every folder it opens

		Vector<String> extensions = AssetDatabase::GetInstance()->GetAssetExtensions();
		for (String& extension : AssetDatabase::GetInstance()->GetImporterExtensions())
		{
			bool text = false;
			for (std::string_view textExtension : TextExtensions)
			{
				text |= std::string_view(extension) == textExtension;
			}
			if (text == false)
			{
				extensions.PushBack(std::move(extension));
			}
		}
		for (String& extension : extensions)
		{
			bool known = false;
			for (const String& added : setup._binaryExtensions)
			{
				known |= added == extension;
			}
			if (known == false)
			{
				setup._binaryExtensions.PushBack(std::move(extension));
			}
		}
		return setup;
	}

	/// @brief The "VersionControl" object of UserSettings.json (the file may hold other settings)
	void VersionControl::LoadUserSettings()
	{
		_providerChoice = "Auto";
		_autoAdd = false;
		_checkoutOnEdit = true;
		_createWorkspace = false;

		if (FileSystem::GetInstance()->IsRegularFile(_userSettingsPath) == false)
		{
			return;
		}

		Document           document;
		DocumentReaderJson reader;
		if (reader.Read(document, _userSettingsPath) == false)
		{
			OUTPUT_WARNING("Version control: can't read {}, default settings used", _userSettingsPath);
			return;
		}

		const DocumentNode* settings = document.GetRootNode().GetChild("VersionControl");
		if (settings == nullptr)
		{
			return;
		}
		if (const DocumentNode* provider = settings->GetChild("provider"); provider != nullptr && provider->GetString().Empty() == false)
		{
			_providerChoice = provider->GetString();
		}
		if (const DocumentNode* autoAdd = settings->GetChild("autoAdd"); autoAdd != nullptr)
		{
			_autoAdd = autoAdd->GetBool();
		}
		if (const DocumentNode* checkoutOnEdit = settings->GetChild("checkoutOnEdit"); checkoutOnEdit != nullptr)
		{
			_checkoutOnEdit = checkoutOnEdit->GetBool();
		}
		if (const DocumentNode* createWorkspace = settings->GetChild("createWorkspace"); createWorkspace != nullptr)
		{
			_createWorkspace = createWorkspace->GetBool();
		}
	}

	/// @brief The other settings of the file kept
	void VersionControl::SaveUserSettings() const
	{
		if (_userSettingsPath.Empty())
		{
			return;
		}

		Document document;
		if (FileSystem::GetInstance()->IsRegularFile(_userSettingsPath))
		{
			DocumentReaderJson reader;
			if (reader.Read(document, _userSettingsPath) == false)
			{
				OUTPUT_ERROR("Version control: can't read {}, not overwritten", _userSettingsPath);
				return;
			}
		}

		DocumentNode& settings = document.GetRootNode().GetOrAddChild("VersionControl");
		settings.GetOrAddChild("provider").SetString(_providerChoice);
		settings.GetOrAddChild("autoAdd").SetBool(_autoAdd);
		settings.GetOrAddChild("checkoutOnEdit").SetBool(_checkoutOnEdit);
		if (_createWorkspace || settings.GetChild("createWorkspace") != nullptr)
		{
			settings.GetOrAddChild("createWorkspace").SetBool(_createWorkspace);
		}

		DocumentWriterJson writer;
		if (writer.Write(document, _userSettingsPath) == false)
		{
			OUTPUT_ERROR("Version control: can't write {}", _userSettingsPath);
		}
	}

	/// @brief
	/// @param projectDir
	/// @param provider
	/// @return
	bool VersionControl::RequestWorkspaceCreation(const Path& projectDir, std::string_view provider)
	{
		const Path userSettingsPath = projectDir / UserSettingsFileName;

		Document document;
		if (FileSystem::GetInstance()->IsRegularFile(userSettingsPath))
		{
			DocumentReaderJson reader;
			if (reader.Read(document, userSettingsPath) == false)
			{
				OUTPUT_ERROR("Version control: can't read {}, not overwritten", userSettingsPath);
				return false;
			}
		}

		DocumentNode& settings = document.GetRootNode().GetOrAddChild("VersionControl");
		settings.GetOrAddChild("provider").SetString(provider);
		settings.GetOrAddChild("createWorkspace").SetBool(true);

		DocumentWriterJson writer;
		if (writer.Write(document, userSettingsPath) == false)
		{
			OUTPUT_ERROR("Version control: can't write {}", userSettingsPath);
			return false;
		}
		return true;
	}
}
