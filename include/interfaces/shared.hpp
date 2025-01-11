#pragma once

#include "interface.hpp"
#include <string_view>
#include <string>

/// Generic interface for interfacing with shared objects
class SharedInterface : public Interface {
  public:
    SharedInterface() = default;
    ~SharedInterface() override;

    bool load(std::string_view path) override;
    reine_retour reine_activation(fourmi_etat fourmis[], const size_t nb_fourmis,
                                  const reine_etat *etat, const salle *salle) override;
    fourmi_retour fourmi_activation(fourmi_etat *fourmi, const salle *salle) override;

  private:
    std::string gpath;
    void *ghandle;

    reine_retour (*reine_fn)(fourmi_etat *, const size_t, const reine_etat *, const salle *);
};
