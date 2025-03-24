#ifndef NUTRIENTHOST_H
#define NUTRIENTHOST_H

#include "SGPHost.h"

namespace sgpmode {

class NutrientHost : public SGPHost {

public:

  /**
   * Constructs a new NutrientHost as an ancestor organism.
   */
  NutrientHost(emp::Ptr<emp::Random> _random, emp::Ptr<SGPWorld> _world,
    emp::Ptr<SymConfigSGP> _config, double _intval = 0.0,
    emp::vector<emp::Ptr<Organism>> _syms = {},
    emp::vector<emp::Ptr<Organism>> _repro_syms = {},
    double _points = 0.0)
    : SGPHost(_random, _world, _config, _intval, _syms, _repro_syms, _points) {}

  /**
   * Constructs a NutrientHost with a copy of the provided genome.
   */
  NutrientHost(emp::Ptr<emp::Random> _random, emp::Ptr<SGPWorld> _world,
    emp::Ptr<SymConfigSGP> _config, const sgpl::Program<Spec>& genome,
    double _intval = 0.0, emp::vector<emp::Ptr<Organism>> _syms = {},
    emp::vector<emp::Ptr<Organism>> _repro_syms = {},
    double _points = 0.0)
    : SGPHost(_random, _world, _config, genome, _intval, _syms, _repro_syms, _points) {}

  NutrientHost(const NutrientHost& host)
    : SGPHost(host) {}

  /**
   * Input: None.
   *
   * Output: A new host with same properties as this host.
   *
   * Purpose: To avoid creating an organism via constructor in other methods.
   */
  emp::Ptr<Organism> MakeNew() override {
    emp::Ptr<NutrientHost> host_baby = emp::NewPtr<NutrientHost>(
      random, GetWorld(), sgp_config, GetCPU().GetProgram(), GetIntVal());
    return host_baby;
  }

  // Process function that handles the host and symbionts.
  void Process(emp::WorldPosition pos) {
    if (my_world->GetUpdate() % sgp_config->EXTINCTION_FREQUENCY() == 0) {
      double death_chance = sgp_config->BASE_DEATH_CHANCE();
        if (GetDead()) {
            return; // If the host is dead, no further processing.
        }
        GetCPU().RunCPUStep(pos, sgp_config->CYCLES_PER_UPDATE());
        emp::vector<emp::Ptr<Organism>> &hosts = GetHost();
        for (size_t i = 0; i < hosts.size(); i++) {
            if (HasSym() & hosts->IsHost()) {
                if (sgp_config->NUTRIENT_TYPE() == MUTUALIST) death_chance = sgp_config->MUTUALIST_DEATH_CHANCE();
                else if (sgp_config->NUTRIENT_TYPE() == PARASITE) death_chance = sgp_config->PARASITE_DEATH_CHANCE();
                emp::vector<emp::Ptr<Organism>> &syms = GetSymbionts();
                for (size_t j = 0; j < syms.size(); j++) {
                    emp::Ptr<Organism> curSym = syms[j];
                    if (GetDead()) {
                        return;
                    }
                    emp::WorldPosition sym_pos = emp::WorldPosition(j + 1, pos.GetIndex());
                    if (!curSym->GetDead()) {
                        curSym->Process(sym_pos); // Call the symbiont's Process method.
                        // TODO: probably need to clean up host death too
                        if (curSym->GetDead()) {
                            syms.erase(syms.begin() + j);
                            curSym.Delete();
                         }
                    } 
                } 
                } 
        } 
    } 
};
#endif // NUTRIENTHOST_H
