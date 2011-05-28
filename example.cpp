#include "fusion_memory_manager.hpp"
#include <iostream>

class Foo
{
public:
	Foo(const char * name) : name(name)
	{
		std::cout << name << " is born" << std::endl;
	}

	const char * Name() const
	{
		return name.c_str();
	}

	~Foo()
	{
		std::cout << name << " is gone" << std::endl;
	}

private:
	std::string name;
};

// Give a boost::mpl::vector as a template parameter
FusionMemoryManager::Manager<boost::mpl::vector<Foo> > manager;

int main()
{
	Foo * foo = new Foo("Fred");

	// Add foo->Name() as key
	manager.Mark(foo->Name(), foo);

	// Giving back the same key. This will delete the foo object, invoking 
	// the destructor.
	manager.Free(foo->Name());

    // Example output:
    // Fred is born
    // Fred is gone

	return 0;
}
