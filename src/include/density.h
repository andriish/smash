/*
 *
 *    Copyright (c) 2013-2017
 *      SMASH Team
 *
 *    GNU General Public License (GPLv3 or later)
 *
 */
#ifndef SRC_INCLUDE_DENSITY_H_
#define SRC_INCLUDE_DENSITY_H_

#include <iostream>
#include <utility>
#include <vector>

#include "energymomentumtensor.h"
#include "experimentparameters.h"
#include "forwarddeclarations.h"
#include "fourvector.h"
#include "lattice.h"
#include "particledata.h"
#include "particles.h"
#include "pdgcode.h"
#include "threevector.h"

namespace smash {

/**
 * Allows to choose which kind of density to calculate.
 * The baryon density is necessary for the Skyrme potential.
 * For the symmetry potential one needs to know the isospin density.
 */
enum class DensityType {
  None = 0,
  Hadron = 1,
  Baryon = 2,
  BaryonicIsospin = 3,
  Pion = 4,
};

std::ostream &operator<<(std::ostream &os, DensityType dt);

/** 
 * Get the factor that determines how much a particle contributes to the
 * density type that is computed. E.g. positive pion contributes with
 * factor 1 to total particle density and with factor 0 to baryon density.
 * Proton contributes with factor 1 to baryon density, anti-proton - with
 * factor -1 to baryon density, and so on.
 *
 * \param[in] type type of the particle to be tested
 * \param[in] dens_type The density type
 * \return The corresponding factor (0 if the particle doesn't
 *         contribute at all).
 */
double density_factor(const ParticleType &type, DensityType dens_type);

/**
 * Norm of the Gaussian smearing function
 *
 * \param[in] two_sigma_sqr \f$2 \sigma^2 \f$ [fm\f$^2\f$],
 *            \f$ \sigma \f$ - width of gaussian smearing
 * \return \f$ (2 \pi \sigma^2)^{3/2}\f$ [fm\f$^3\f$]
 */
inline double smearing_factor_norm(const double two_sigma_sqr) {
  const double tmp = two_sigma_sqr * M_PI;
  return tmp * std::sqrt(tmp);
}

/**
 * Norm of the gradient of the Gaussian smearing factor
 *
 * \param[in] two_sigma_sqr \f$2 \sigma^2 \f$ [fm\f$^2\f$],
 *            \f$ \sigma \f$ - width of gaussian smearing
 * \return \f$ (2 \pi \sigma^2)^{3/2} \cdot  * (2 \sigma^2) \f$ [fm\f$^5\f$]
 */
inline double smearing_factor_grad_norm(const double two_sigma_sqr) {
  const double tmp = two_sigma_sqr * M_PI;
  return tmp * std::sqrt(tmp) * 0.5 * two_sigma_sqr;
}

/**
 * Gaussians used for smearing are cut at radius \f$r_{cut} = a \sigma \f$
 * for calculation speed-up. In the limit of \f$a \to \infty \f$ smearing
 * factor is normalized to 1:
 * \f[ \frac{4 \pi}{(2 \pi \sigma^2)^{3/2}}
 *     \int_0^{\infty} e^{-r^2/2 \sigma^2} r^2 dr = 1 \f]
 * However, for finite \f$ a\f$ integral is less than one:
 * \f[ g(a) \equiv \frac{4 \pi}{(2 \pi \sigma^2)^{3/2}}
 *    \int_0^{a \sigma} e^{-r^2/2 \sigma^2} r^2 dr =
 *    -\sqrt{\frac{2}{\pi}} a e^{-a^2/2} + Erf[a/\sqrt{2}]
 * \f] This \f$ g(a) \f$ is typically close to 1. For example,
 * for \f$r_{cut} = 3 \sigma \f$, and thus \f$ a=3 \f$, g(3) = 0.9707;
 * g(4) = 0.9987. The aim of this function is to compensate for this factor.
 *
 * \param[in] rcut_in_sigma \f$ a = r_{cut} / \sigma\f$
 * \return \f$ g(a) \f$
 */
inline double smearing_factor_rcut_correction(const double rcut_in_sigma) {
  const double x = rcut_in_sigma / std::sqrt(2.0);
  return -2.0 / std::sqrt(M_PI) * x * std::exp(-x * x) + std::erf(x);
}

/** 
 * A class to precalculate and store parameters relevant for density
 * calculation. It has to be initialized only once per SMASH run.
 */
class DensityParameters {
 public:
  /**
   * Constructor of DensityParameters.
   *
   * \param[in]: par Struct containing the Gaussian smearing width
   *             \f$\sigma\f$, the cutoff factor \f$a\f$ where the
   *             cutoff radius \f$r_{\rm cut}=a\sigma\f$, and the
   *             testparticle number.
   */
  DensityParameters(const ExperimentParameters &par)  // NOLINT
      : sig_(par.gaussian_sigma),
        r_cut_(par.gauss_cutoff_in_sigma * par.gaussian_sigma),
        ntest_(par.testparticles) {
    r_cut_sqr_ = r_cut_ * r_cut_;
    const double two_sig_sqr = 2 * sig_ * sig_;
    two_sig_sqr_inv_ = 1. / two_sig_sqr;
    const double norm1 = smearing_factor_norm(two_sig_sqr);
    const double norm2 = smearing_factor_grad_norm(two_sig_sqr);
    const double corr_factor =
        smearing_factor_rcut_correction(par.gauss_cutoff_in_sigma);
    norm_factor_sf_ = 1. / (norm1 * ntest_ * corr_factor);
    norm_factor_sf_grad_ = 1. / (norm2 * ntest_ * corr_factor);
  }
  /// \return Testparticle number
  int ntest() const { return ntest_; }
  /// \return Cut-off radius [fm]
  double r_cut() const { return r_cut_; }
  /// \return Squared cut-off radius [fm\f$^2\f$]
  double r_cut_sqr() const { return r_cut_sqr_; }
  /// \return \f$ (2 \sigma^2)^{-1} \f$ [fm\f$^{-2}\f$]
  double two_sig_sqr_inv() const { return two_sig_sqr_inv_; }
  /** 
   * \return Normalization for smearing factor. Unnormalized smearing factor
   *         \f$ sf(\vec{r}) \f$ has to be multiplied by this to have
   *         \f$ \int d^3r \, sf(\vec{r}) = 1 \f$.
   */
  double norm_factor_sf() const { return norm_factor_sf_; }
  /// \return Normalization for the gradient of the smearing factor
  double norm_factor_sf_grad() const { return norm_factor_sf_grad_; }

 private:
  /// Gaussian smearing width [fm]
  const double sig_;
  /// Cut-off radius [fm]
  const double r_cut_;
  /// Squared cut-off radius [fm\f$^2\f$]
  double r_cut_sqr_;
  /// \f$ (2 \sigma^2)^{-1} \f$ [fm\f$^{-2}\f$]
  double two_sig_sqr_inv_;
  /// Normalization for smearing factor 
  double norm_factor_sf_;
  /// Normalization for the gradient of the smearing factor
  double norm_factor_sf_grad_;
  /// Testparticle number
  const int ntest_;
};

/**
 * Implements gaussian smearing for any quantity.
 * Computes smearing factor taking Lorentz contraction into account.
 * Integral of unnormalized smearing factor over space should be
 *  \f$ (2 \pi \sigma^2)^{3/2} \f$. Division over norm is splitted
 *  for efficiency: it is not nice to recalculate the same constant
 *  norm at every call.
 *
 * \param[in] r vector from the particle to the point of interest [fm]
 * \param[in] p particle 4-momentum to account for Lorentz contraction [GeV]
 * \param[in] m_inv particle mass, \f$ (E^2 - p^2)^{-1/2} \f$ [GeV]
 * \param[in] dens_par object containing precomputed parameters for
 *            density calculation.
 * \param[in] compute_gradient option, true - compute gradient, false - no
 * \return (smearing factor, the gradient of the smearing factor or a zero
 *         three vector)
 */
std::pair<double, ThreeVector> unnormalized_smearing_factor(
    const ThreeVector &r, const FourVector &p, const double m_inv,
    const DensityParameters &dens_par, const bool compute_gradient = false);

/**
 * Calculates Eckart rest frame density and optionally its gradient.
 * \f[j^{\mu} = (\sqrt{2\pi} \sigma )^{-3} \sum_{i=1}^N C_i u^{\mu}_i
 * exp \left(- \frac{(\vec r -\vec r_i + \frac{\gamma_i^2}{1 + \gamma_i}
 * \vec \beta_i (\vec \beta_i, \vec r - \vec r_i))^2}{2\sigma^2} \right)\f]
 * \f[ \rho^{Eckart} = \sqrt{j^{\mu} j_{\mu}} \f]
 * Here \f$ C_i \f$ is a corresponding value of "charge". If baryon
 * current option is selected then \f$ C_i \f$ is 1 for baryons,
 * -1 for antibaryons and 0 otherwise. For proton/neutron
 * current \f$ C_i = 1\f$ for proton/neutron and 0 otherwise.
 *
 * For gradient:
 * \f[ \frac{d\rho_{Eck}}{d \vec r} = \frac{\frac{dj^{\mu}}{d \vec r}
 * j_{\mu}}{\sqrt{j^{\mu}j_{\mu}}} \f]
 *
 * To avoid the problems with Eckart frame definition, densities for
 * positive and negative charges, \f$\rho_+ \f$ and \f$ \rho_-\f$,
 * are computed separately and result is \f$\rho_+ - \rho_-\f$.
 *
 * \param[in] r Arbitrary space point where 4-current is calculated [fm]
 * \param[in] plist List of all particles to be used in \f$j^{\mu}\f$
 *            calculation. If the distance between particle and calculation
 *            point r, \f$ |r-r_i| > r_{cut} \f$ then particle input
 *            to density will be ignored.
 *
 * Next three values are taken from ExperimentalParameters structure:
 *
 * \param[in] par Set of parameters packed in one structure.
 *            From them the cutting radius r_cut \f$ r_{cut} / \sigma \f$,
 *            number of test-particles ntest and the gaussian width
 *            gs_sigma are needed.
 * \param[in] dens_type type of four-currect to be calculated:
 *            baryon, proton or neutron options are currently available
 * \param[in] compute_gradient true - compute gradient, false - no
 * \fpPrecision Density gradient is returned as double, because it is
 *   ThreeVector and ThreeVector currently comes only as double.
 *   Density itself is double for uniformity: if gradient is double,
 *   density should also be.
 * \return (density in the local Eckart frame [fm\$f^{-3}\$f], 
 *          the gradient of the density or a 0 3-vector)
 */
std::pair<double, ThreeVector> rho_eckart(const ThreeVector &r,
                                          const ParticleList &plist,
                                          const DensityParameters &par,
                                          DensityType dens_type,
                                          bool compute_gradient);
/// convenience overload of the above
std::pair<double, ThreeVector> rho_eckart(const ThreeVector &r,
                                          const Particles &plist,
                                          const DensityParameters &par,
                                          DensityType dens_type,
                                          bool compute_gradient);

/** 
 * A class for time-efficient (time-memory trade-off) calculation of density
 * on the lattice. It holds two FourVectors - positive and negative
 * summands of 4-current, and the density itself. Four-currents are
 * additive by particles, density is not. That is why such structure is
 * used. It is efficient to calculate additive jmu's in one loop over
 * particles and then calculate density from jmu's. Splitting into
 * positive and negative parts of jmu is necessary to avoid
 * problems with the definition of Eckart rest frame.
 *
 * Intended usage of the class:
 * -# Add particles from some list using add_particle(...). This sets
 *    jmu_pos and jmu_neg
 * -# Compute density from jmus using compute_density(...).
 * -# Get jmus and density whenever necessary via density(),
 *    jmu_pos(), jmu_neg()
 */
class DensityOnLattice {
 public:
  /// Default constructor
  DensityOnLattice()
      : jmu_pos_(FourVector()), jmu_neg_(FourVector()), density_(0.0) {}

  /**
   * Adds particle to 4-current: \f$j^{\mu} += p^{\mu}/p^0 \cdot factor \f$
   * Two private class members jmu_pos_ and jmu_neg_ indicating the 4-current
   * of the positively and negatively charged particles are updated by this
   * function
   *
   * \param[in] part Particle would be added to the current density 
   *            on the lattice.
   * \param[in] factor Factor can in principle be any scalar multiplier. 
   *            Physics-wise it accounts for smearing on the lattice and for
   *            particle contribution to given density type (e.g. anti-proton
   *            contributes with factor -1 to baryon density, proton - with
   *            factor 1).
   */
  void add_particle(const ParticleData &part, double factor) {
    if (factor > 0.0) {
      jmu_pos_ += FourVector(factor, part.velocity() * factor);
    } else {
      jmu_neg_ += FourVector(factor, part.velocity() * factor);
    }
  }

  /**
   * Compute the net density on the local lattice, and assign it
   * to the private class member density_
   *
   * \param[in] norm_factor Normalization factor
   */
  void compute_density(const double norm_factor = 1.0) {
    density_ = (jmu_pos_.abs() - jmu_neg_.abs()) * norm_factor;
  }
  /// \return Net density on the lattice
  double density() const { return density_; }
  /// \return Current density of the positively charged particle
  FourVector jmu_pos() const { return jmu_pos_; }
  /// \return Current density of the negatively charged particle
  FourVector jmu_neg() const { return jmu_neg_; }

 private:
  /// Four-current density of the positively and negativelly charged particle.
  FourVector jmu_pos_, jmu_neg_;
  /// Net density
  double density_;
};

/// Conveniency typedef for lattice of density
typedef RectangularLattice<DensityOnLattice> DensityLattice;

/**
 * Updates the contents on the lattice.
 *
 * \param[out] lat The lattice on which the content will be updated
 * \param[in] update tells if called for update at printout or at timestep
 * \param[in] dens_type density type to be computed on the lattice
 * \param[in] par a structure containing testparticles number and gaussian
 *            smearing parameters.
 * \param[in] particles the particles vector
 */
template <typename /*LatticeType*/ T>
void update_general_lattice(RectangularLattice<T> *lat,
                            const LatticeUpdate update,
                            const DensityType dens_type,
                            const DensityParameters &par,
                            const Particles &particles) {
  // Do not proceed if lattice does not exists/update not required
  if (lat == nullptr || lat->when_update() != update) {
    return;
  }
  lat->reset();
  const double norm_factor = par.norm_factor_sf();
  for (const auto &part : particles) {
    const double dens_factor = density_factor(part.type(), dens_type);
    if (std::abs(dens_factor) < really_small) {
      continue;
    }
    const FourVector p = part.momentum();
    const double m = p.abs();
    if (unlikely(m < really_small)) {
      const auto &log = logger<LogArea::Density>();
      log.warn("Gaussian smearing is undefined for momentum ", p);
      continue;
    }
    const double m_inv = 1.0 / m;

    const ThreeVector pos = part.position().threevec();
    lat->iterate_in_radius(
        pos, par.r_cut(), [&](T &node, int ix, int iy, int iz) {
          const ThreeVector r = lat->cell_center(ix, iy, iz);
          const double sf =
              norm_factor *
              unnormalized_smearing_factor(pos - r, p, m_inv, par).first;
          if (sf > really_small) {
            node.add_particle(part, sf * dens_factor);
          }
        });
  }
}

/**
 * Calculates density on the lattice in an time-efficient way.
 *
 * \param[out] lat The lattice on which the density will be updated
 * \param[in] update tells if called for update at printout or at timestep
 * \param[in] dens_type density type to be computed on the lattice
 * \param[in] par a structure containing testparticles number and gaussian
 *            smearing parameters.
 * \param[in] particles the particles vector
 */
void update_density_lattice(DensityLattice *lat, const LatticeUpdate update,
                            const DensityType dens_type,
                            const DensityParameters &par,
                            const Particles &particles);

/** 
 * Calculates energy-momentum tensor on the lattice in an time-efficient way.
 *
 * \param[out] lat The lattice on which the energy-momentum tensor
 *             will be updated.
 * \param[in] update tells if called for update at printout or at timestep
 * \param[in] dens_type density type to be computed on the lattice
 * \param[in] par a structure containing testparticles number and gaussian
 *            smearing parameters.
 * \param[in] particles the particles vector
 */
void update_Tmn_lattice(RectangularLattice<EnergyMomentumTensor> *lat,
                        const LatticeUpdate update, const DensityType dens_type,
                        const DensityParameters &par,
                        const Particles &particles);

}  // namespace smash

#endif  // SRC_INCLUDE_DENSITY_H_
