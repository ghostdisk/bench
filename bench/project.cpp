#include <bench/project.hpp>
#include <bench/core/file.hpp>

namespace bench {

Project* Project::Load(String path, FileCreateDisposition create_dispoisiton) {
	Project* project = new Project();

	if (project->ini.Load(path, true, create_dispoisiton)) {
		return project;
	}
	else {
		delete project;
		return nullptr;
	}
}

String Project::GetName() {
	return ini.GetString("name");
}

void Project::SetName(String name) {
	ini.SetString("name", name);
	ini.Save();
}

}
