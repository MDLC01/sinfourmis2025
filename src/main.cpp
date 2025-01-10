#include "argparse/argparse.hpp"
#include "game/game.hpp"
#include "interfaces/dummy.hpp"
#ifdef USING_PYTHON
#include "interfaces/python.hpp"
#endif
#include "interfaces/shared.hpp"
#include "map/map.hpp"
#include <filesystem>
#include <iostream>
#include <random>

std::filesystem::path check_path(const std::string &value) {
    if (value.empty()) {
        throw std::runtime_error("Output folder cannot be empty");
    }
    std::filesystem::path path(value);
    if (!std::filesystem::exists(path)) {
        return path;
    } else if (value == "output.json") {
        int i = 0;
        while (std::filesystem::exists(path)) {
            path = std::filesystem::path("output" + std::to_string(i) + ".json");
            i++;
        }
        return path;
    }
    std::cerr << "Ouput file already exists" << std::endl;
    exit(1);
    return path;
}

int main(int argc, char **argv) {
    argparse::ArgumentParser program("sinfourmis");
    program.add_description("The 2025 sinfourmis simulator");
    program.add_epilog("MIT Licence - Copyright (c) 2025 - sinfonie");
    program.add_argument("map").help("The map file to use").required().metavar("MAP");
    auto &group = program.add_mutually_exclusive_group(true);

    std::random_device rd;

    group.add_argument("-c", "--check")
        .help("Check the map file for errors and exit")
        .default_value(false)
        .implicit_value(true);

    group.add_argument("-t", "--team").help("Team files to use").nargs(1).append().metavar("TEAM");

    program.add_argument("-o", "--output")
        .help("The combat output file used to store the results")
        .default_value("output.json")
        .metavar("OUTPUT");

    program.add_group("Simulation options:");
    program.add_argument("-d", "--duration")
        .help("The duration of the simulation in turns")
        .default_value(1000)
        .nargs(1)
        .scan<'i', int>()
        .action([](const std::string &value) {
            int duration = std::stoi(value);
            if (duration <= 0) {
                throw std::runtime_error("Duration must be a positive integer");
            }
            return duration;
        })
        .metavar("DURATION");
    program.add_argument("-s", "--seed")
        .help("The seed to use for the random number generator, default to a random value")
        .default_value((unsigned int)rd())
        .nargs(1)
        .scan<'i', unsigned int>()
        .action([](const std::string &value) { return (unsigned int)std::stoul(value); })
        .metavar("SEED");
    program.add_argument("-f", "--flush")
        .help("Flush the animation at each turn")
        .default_value(false)
        .flag();
    program.add_argument("--debug")
        .help("Do the animation step by step to debug")
        .default_value(false)
        .flag();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        return 1;
    }

    Map map;
    if (!map.load(program.get<std::string>("map"))) {
        return 2;
    }
    if (program["check"] == true) {
        std::cout << "Map is valid" << std::endl;
        return 0;
    }

    auto teams_opt = program.present<std::vector<std::string>>("team");
    if (!teams_opt.has_value()) {
        std::cerr << "No team files provided" << std::endl;
        return 1;
    }
    const std::vector<std::string> &teams = teams_opt.value();
    if (teams.size() != map.get_team_count()) {
        std::cerr << "Invalid number of teams : " << teams.size() << " instead of "
                  << map.get_team_count() << std::endl;
        return 1;
    }

    Game &game = Game::getInstance();
    game.set_map(std::move(map));
    int team_id = 0;
    for (const std::string &team : teams) {
        Interface *interface;
        if (team == "dummy") {
            interface = new Dummy();
        } else if (!team.compare(team.length() - 3, 3, ".so")) {
            // the team file is a shared object, we use the corresponding interface
            std::cout << "Loading " << team << " using the shared object interface" << std::endl;
            interface = new SharedInterface();
        } else {
#ifdef USING_PYTHON
            // We assume the team file is a python package
            std::cout << "Loading " << team << " using the python interface" << std::endl;
            interface = new PythonInterface();
#else
            std::cerr << "Unknown team file format" << std::endl;
            return 1;
#endif
        }
        if (!interface->load(team)) {
            std::cerr << "Failed to load team file" << std::endl;
            return 3;
        }
        game.add_interface(team_id, interface);
        team_id++;
    }

    int duration = program.get<int>("duration");
    int seed = program.get<unsigned int>("seed");
    auto path = check_path(program.get<std::string>("output"));
    auto flush = program.get<bool>("flush");
    auto debug = program.get<bool>("debug");

    game.run(duration, seed, flush, debug, std::move(path));

    return 0;
}
