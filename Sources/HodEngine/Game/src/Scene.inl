
namespace hod
{
	namespace game
	{
		template<typename T>
		T* Scene::SpawnActor(const std::string& name)
		{
			T* actor = new T(name, this);

			_actorList.push_back(actor);

			return actor;
		}
	}
}
