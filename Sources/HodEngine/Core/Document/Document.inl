
namespace hod
{
	template<typename T>
	void Document::Node::SetValues(const std::span<const T>& values)
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
