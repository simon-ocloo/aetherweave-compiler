#include <cstdio>
#include <cstdlib>

#include "compiler/import/configuration/ConfigurationImporter.h"
#include "compiler/orchestrator/Orchestrator.h"


int main(int argc, char** argv)
{
    if (argc == 2) {
        auto configuration = aetherweave::ConfigurationImporter::from_file(argv[1]);
        if (configuration.has_value() == false) {
            return EXIT_FAILURE;
        }

        aetherweave::Orchestrator orchestrator;
        if (orchestrator.run(configuration.value()) == false) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else {
        std::fprintf(stderr, "Usage: aw-compile <config.json>\n");
        return EXIT_FAILURE;
    }
}
