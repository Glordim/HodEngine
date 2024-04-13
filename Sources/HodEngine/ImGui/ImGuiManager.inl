namespace hod::imgui
{
	/// @brief 
	/// @tparam Window_ 
	/// @return 
	template<typename Window_>
	Window_* ImGuiManager::OpenWindow()
	{
		Window_* window = new Window_();
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
}
