#include "interfaces/shared.hpp"
#include "sinfourmis.h"

#include <cstddef>
#include <dlfcn.h>
#include <iostream>

bool SharedInterface::load(std::string_view path) {
    gpath = path;

    ghandle = dlopen(path.data(), RTLD_LAZY);
    if (!ghandle) {
        std::cerr << "Error loading shared object " << path.data() << std::endl;
		std::cerr << dlerror() << std::endl;
        return false;
    }

    reine_fn = (reine_retour(*)(fourmi_etat *, const size_t, const reine_etat *,
                                const salle *))dlsym(ghandle, "reine_activation");
    if (!reine_fn) {
        std::cerr << "Error loading reine_activation: " << dlerror() << std::endl;
        return false;
    }
    return true;
}

SharedInterface::~SharedInterface() {
    dlclose(ghandle);
}

reine_retour SharedInterface::reine_activation(fourmi_etat fourmis[], const size_t nb_fourmis,
                                               const reine_etat *etat, const salle *salle) {
    return reine_fn(fourmis, nb_fourmis, etat, salle);
}

fourmi_retour SharedInterface::fourmi_activation(fourmi_etat *fourmi, const salle *salle) {
    // each time fourmi_activation is called, we load the corresponding library to prevent
    // persistent data modifications
    void *handle = dlopen(gpath.data(), RTLD_LAZY);

    fourmi_retour (*fourmi_fn)(fourmi_etat *, const struct salle *) =
        (fourmi_retour(*)(fourmi_etat *, const struct salle *))dlsym(handle, "fourmi_activation");
    if (!fourmi_fn) {
        std::cerr << "Error loading fourmi_activation: " << dlerror() << " from " << gpath.data()
                  << std::endl;
        exit(1);
    }

    fourmi_retour result = fourmi_fn(fourmi, salle);

    dlclose(handle);
    return result;
}
