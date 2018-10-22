/*
 *
 *    Copyright (c) 2014-2018
 *      SMASH Team
 *
 *    GNU General Public License (GPLv3 or later)
 *
 */

#include "unittest.h"  // This include has to be first

#include "setup.h"

#include <fstream>
#include <map>

#include "../include/smash/collidermodus.h"
#include "../include/smash/configuration.h"
#include "../include/smash/constants.h"
#include "../include/smash/cxx14compat.h"
#include "../include/smash/experiment.h"
#include "../include/smash/modusdefault.h"
#include "../include/smash/nucleus.h"
#include "../include/smash/potentials.h"
#include "../include/smash/propagation.h"
#include "../include/smash/spheremodus.h"

#include <boost/filesystem.hpp>

using namespace smash;

TEST(init_particle_types) {
  ParticleType::create_type_list(
      "# NAME MASS[GEV] WIDTH[GEV] PARITY PDG\n"
      "N+ 0.938 0.0 + 2212\n"
      "N0 0.938 0.0 + 2112\n"
      "π+ 0.138 0.0 - 211\n");
}

static ParticleData create_proton(int id = -1) {
  return ParticleData{ParticleType::find(0x2212), id};
}

// Create nuclear potential profile in XY plane
TEST(nucleus_potential_profile) {
  // Create a nucleus
  Configuration conf = Test::configuration();
  // All interactions off
  conf["Collision_Term"]["Decays"] = "False";
  conf["Collision_Term"]["Collisions"] = "False";
  conf["Collision_Term"]["Sigma"] = 0.0;
  // Fixed target: Copper
  conf["Modi"]["Collider"]["Calculation_Frame"] = "fixed target";
  conf["Modi"]["Collider"]["E_Kin"] = 1.23;
  conf["Modi"]["Collider"]["Projectile"]["Particles"]["211"] = 1;
  conf["Modi"]["Collider"]["Target"]["Particles"]["2212"] = 29;
  conf["Modi"]["Collider"]["Target"]["Particles"]["2112"] = 34;
  conf["Modi"]["Collider"]["Target"]["Automatic"] = "True";

  ExperimentParameters param = smash::Test::default_parameters();
  ColliderModus c(conf["Modi"], param);
  Particles P;
  c.initial_conditions(&P, param);
  ParticleList plist;

  // Create potentials
  conf["Potentials"]["Skyrme"]["Skyrme_A"] = -209.2;
  conf["Potentials"]["Skyrme"]["Skyrme_B"] = 156.4;
  conf["Potentials"]["Skyrme"]["Skyrme_Tau"] = 1.35;
  Potentials pot = Potentials(conf["Potentials"], param);

  // Write potential XY map in a vtk output
  ThreeVector r;
  const int nx = 50, ny = 50;
  const double dx = 0.2, dy = 0.2;
  double pot_value;
  const ParticleType &proton = ParticleType::find(0x2212);

  std::ofstream a_file;
  const double timestep = param.labclock.timestep_duration();
  for (auto it = 0; it < 20; it++) {
    {
      a_file.open(("Nucleus_U_xy.vtk." + std::to_string(it)).c_str(),
                  std::ios::out);
      plist = P.copy_to_vector();
      a_file << "# vtk DataFile Version 2.0\n"
             << "potential\n"
             << "ASCII\n"
             << "DATASET STRUCTURED_POINTS\n"
             << "DIMENSIONS " << 2 * nx + 1 << " " << 2 * ny + 1 << " 1\n"
             << "SPACING 1 1 1\n"
             << "ORIGIN " << -nx << " " << -ny << " 0\n"
             << "POINT_DATA " << (2 * nx + 1) * (2 * ny + 1) << "\n"
             << "SCALARS potential double 1\n"
             << "LOOKUP_TABLE default\n";

      a_file << std::setprecision(8);
      a_file << std::fixed;
      for (auto iy = -ny; iy <= ny; iy++) {
        for (auto ix = -nx; ix <= nx; ix++) {
          r = ThreeVector(ix * dx, iy * dy, 8.0);
          pot_value = pot.potential(r, plist, proton);
          a_file << pot_value << " ";
        }
        a_file << "\n";
      }
    }

    for (auto i = 0; i < 50; i++) {
      const double time_to = 5.0 * it + i * timestep;
      const double dt = propagate_straight_line(&P, time_to, {});
      update_momenta(&P, dt, pot, nullptr, nullptr);
    }
  }
}

TEST(propagation_in_test_potential) {
  /* Two dummy potentials are created:
   * One has only the time component: U(x) = U_0/(1 + exp(x/d))
   * A particle is propagated through this stationary potential and
   * its momentum and energy are checked against analytically expected
   * from conservation laws.
   * The other gives rise to a constant magnetic field along z-axis.
   * A particle is expected to do a circular motion with a constant
   * speed in the magnetic field. Its final speed is compared with
   * the initial value.*/

  // Create a dummy potential
  class Dummy_Pot : public Potentials {
   public:
    Dummy_Pot(Configuration conf, const ExperimentParameters &param,
              const double U0, const double d, const double B0)
        : Potentials(conf, param), U0_(U0), d_(d), B0_(B0) {}

    std::tuple<ThreeVector, ThreeVector, ThreeVector, ThreeVector> all_forces(
        const ThreeVector &r, const ParticleList &) const override {
      const double tmp = std::exp(r.x1() / d_);
      return std::make_tuple(
          ThreeVector(U0_ / d_ * tmp / ((1.0 + tmp) * (1.0 + tmp)), 0.0, 0.0),
          ThreeVector(0., 0., B0_), ThreeVector(), ThreeVector());
    }

    bool use_skyrme() const override { return true; }
    bool use_symmetry() const override { return true; }

   private:
    const double U0_, d_, B0_;
  };

  // Create spheremodus with arbitrary parameters
  // Do not initialize particles: just artificially put one particle to list
  const double p_mass = 0.938;
  Configuration conf = Test::configuration();
  ExperimentParameters param = smash::Test::default_parameters();

  /* Create two dummy test potentials: one has only the electrical
   * force, the other has only magnetic force. */
  const double U0 = 0.5;
  const double d = 4.0;
  const double B0 = 0.5;
  std::unique_ptr<Dummy_Pot> pot1 =
      make_unique<Dummy_Pot>(conf["Potentials"], param, U0, d, 0.);
  std::unique_ptr<Dummy_Pot> pot2 =
      make_unique<Dummy_Pot>(conf["Potentials"], param, 0., d, B0);

  /* Create two particles: one flies in the pure electrical field, while
   * the other flies in the pure magnetical field. */
  ParticleData part1 = create_proton();
  part1.set_4momentum(p_mass, ThreeVector(2.0, -1.0, 1.0));
  part1.set_4position(FourVector(0.0, -20 * d, 0.0, 0.0));
  Particles P1;
  P1.insert(part1);
  COMPARE(P1.back().id(), 0);

  // This particle is expected to do circular motion with a constant speed.
  ParticleData part2 = create_proton();
  // The speed of the particle is 0.6
  part2.set_4momentum(4., ThreeVector(3., 0., 0.));
  part2.set_4position(FourVector(0.0, 0.0, 2.0, 0.0));
  Particles P2;
  P2.insert(part2);
  COMPARE(P2.back().id(), 0);

  /* Propagate the first particle, until particle1 is at x>>d,
   * where d is parameter of potential */
  const double timestep = param.labclock.timestep_duration();
  double time_to = 0.0;
  while (P1.front().position().x1() < 20 * d) {
    time_to += timestep;
    const double dt1 = propagate_straight_line(&P1, time_to, {});
    update_momenta(&P1, dt1, *pot1, nullptr, nullptr);
  }

  // Propagate the second particle for two periods.
  const double period = hbarc * twopi * 4. / B0;
  time_to = 0.0;
  while (P2.front().position().x0() < 2. * period) {
    time_to += timestep;
    const double dt2 = propagate_straight_line(&P2, time_to, {});
    update_momenta(&P2, dt2, *pot2, nullptr, nullptr);
  }

  // Calculate 4-momentum, expected from conservation laws
  const FourVector pm = part1.momentum();
  FourVector expected_p = FourVector(
      pm.x0() + U0, std::sqrt(pm.x1() * pm.x1() + 2 * pm.x0() * U0 + U0 * U0),
      pm.x2(), pm.x3());

  COMPARE_ABSOLUTE_ERROR(expected_p.x0(), P1.front().momentum().x0(), 1.e-4)
      << "Expected energy " << expected_p.x0() << ", obtained "
      << P1.front().momentum().x0();
  COMPARE_ABSOLUTE_ERROR(expected_p.x1(), P1.front().momentum().x1(), 1.e-4)
      << "Expected px " << expected_p.x1() << ", obtained "
      << P1.front().momentum().x1();
  // y and z components did not have to change at all, so check is precise
  COMPARE(expected_p.x2(), P1.front().momentum().x2());
  COMPARE(expected_p.x3(), P1.front().momentum().x3());

  /* compare the final speed of the second particle with its initial value,
   * the test is passed if the error is within 1 percent. */
  COMPARE_RELATIVE_ERROR(P2.front().momentum().velocity().abs(), 0.6, 0.01)
      << "Expected speed " << 0.6 << ", obtained "
      << P2.front().momentum().velocity().abs();
}
