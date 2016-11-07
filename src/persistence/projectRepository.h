#include <phi.h>
#include "persistenceApi.h"

#include <domain/project.h>
#include <domain/repositories/iProjectRepository.h>

namespace demon
{
    class projectRepository : public iProjectRepository
    {
    public:
        PERSISTENCE_API projectRepository();
        PERSISTENCE_API ~projectRepository();

        PERSISTENCE_API virtual project* load(phi::string path) override;
        PERSISTENCE_API virtual void save(project* project, phi::string path) override;
    };
}