#include "include/scatteractionsfinderphoton.h"

#include <algorithm>

#include "include/scatteractionphoton.h"
#include "include/configuration.h"
#include "include/constants.h"
#include "include/cxx14compat.h"
#include "include/experimentparameters.h"
#include "include/logging.h"
#include "include/macros.h"
#include "include/particles.h"

namespace Smash{
  
  ScatterActionPtr ScatterActionsFinderPhoton::construct_scatter_action(const ParticleData &data_a, const ParticleData &data_b, float time_until_collision) const override {
    return make_unique<ScatterActionPhoton>(data_a, data_b, time_until_collision);
  }
  
}