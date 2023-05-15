
namespace hod
{
	namespace core
	{
		template<typename T>
		void Document::Node::SetValues(const std::span<const T>& values)
		{
			// TODO ensure no child

			for (const T& value : values)
			{
				Node& child = AddChild("");
				child.SetValue(value);
			}
		}
	}
}
