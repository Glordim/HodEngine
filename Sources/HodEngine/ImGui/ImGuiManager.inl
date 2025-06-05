namespace hod::imgui
{
	/// @brief 
	/// @tparam Window_ 
	/// @return 
	template<typename Window_, typename... Args>
	Window_* ImGuiManager::OpenWindow(Args&&... args)
	{
		Window_* window = new Window_(std::forward<Args>(args)...);
		OpenWindow(window);
		return window;
	}

	/// @brief 
	/// @tparam Window_ 
	/// @return 
	template<typename Window_>
	Window_* ImGuiManager::FindWindow() const
	{
		return static_cast<Window_*>(FindWindow(Window_::GetStaticDescription()));
	}

	/// @brief 
	/// @tparam Window_ 
	/// @return 
	template<typename Window_>
	Vector<Window*> ImGuiManager::FindWindows() const
	{
		return FindWindows(Window_::GetStaticDescription());
	}
}
