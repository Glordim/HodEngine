namespace hod::imgui
{
	template<typename _Window_>
	_Window_* ImGuiManager::OpenWindow()
	{
		_Window_* window = new _Window_();

		_windows.push_back(window);

		return window;
	}
}
