
namespace hod
{
	template<typename T, size_t Size>
	void Document::Node::SetValues(const std::span<T, Size>& values)
	{
		// TODO ensure no child
		_type = Type::Array;

		for (const T& value : values)
		{
			Node& child = AddChild("");
			child.SetValue(value);
		}
	}
}
