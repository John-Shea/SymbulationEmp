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


  void Process(emp::WorldPosition pos){
    if (GetDead()) {
      return; // If the host is dead, no further processing.
    }

    bool host_cycle = true;
    if (HasSym()) {
      host_cycle = random->P(0.5); 
    }

    // If the host is processing, run the host's CPU step.
    if (host_cycle) {
      GetCPU().RunCPUStep(pos, sgp_config->CYCLES_PER_UPDATE());
    }

    // Process symbionts if the host isn't processing.
    if (HasSym() && !host_cycle) {
      emp::vector<emp::Ptr<Organism>> &syms = GetSymbionts();
      for (size_t j = 0; j < syms.size(); j++) {
        emp::Ptr<Organism> curSym = syms[j];
        if (GetDead()) {
          return; // Stop processing if the host is dead.
        }

        // Process the symbiont if it's not dead.
        emp::WorldPosition sym_pos = emp::WorldPosition(j + 1, pos.GetIndex());
        if (!curSym->GetDead()) {
          curSym->Process(sym_pos); // Call the symbiont's Process method.

            // TODO: ask 
          // Check if the symbiont performed a task that matches the host's recent task.
          if (SymbiontPerformedTask(curSym)) {
            // If the operation matches the host's recent operation, steal resources.
            double resources_stolen = StealResources(curSym->GetIntVal());
            if (resources_stolen > 0) {

                /// ask about infinity resources 
              // Steal resources from the host and the world. The resources are split.
              double world_resources = my_world->PullResources(resources_stolen);
              double stolen_from_host = resources_stolen / 2;
              double stolen_from_world = resources_stolen / 2;
              this->AddPoints(stolen_from_world); 
              curSym->AddPoints(stolen_from_host); 
            }
          } else {
            // If the operation doesn't match the host's recent operation, distribute resources equally.
            double resources = curSym->GetIntVal(); // Symbiont earns resources.
            double half = resources / 2;
            this->AddPoints(half); // Half goes to the host.
            curSym->AddPoints(half); // Half goes to the symbiont.
          }
        }

        // If the symbiont dies during processing, remove it from the list.
        if (curSym->GetDead()) {
          syms.erase(syms.begin() + j);
          curSym.Delete();
        }
      } // For each sym in syms
    } // If the host has symbionts
  }
  // TODO: Helper function to check if the symbiont performed a task that matches the host's recent task
  bool SymbiontPerformedTask(emp::Ptr<Organism> curSym) {


    return false; 
  }
};

}

#endif
