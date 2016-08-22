/*
 *
 *    Copyright (c) 2013-2014
 *      SMASH Team
 *
 *    GNU General Public License (GPLv3 or later)
 *
 */

#include "include/parametrizations.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "include/average.h"
#include "include/cxx14compat.h"
#include "include/interpolation.h"
#include "include/kinematics.h"
#include "include/lowess.h"
#include "include/pow.h"

// All quantities in this file use they same units as the rest of SMASH.
// That is: GeV for energies and momenta, fm for distances and time, and mb for
// cross sections.

namespace Smash {

/** pp elastic cross section parametrization.
 * Source: \iref{Weil:2013mya}, eq. (44) */
float pp_elastic(double mandelstam_s) {
  double p_lab = plab_from_s(mandelstam_s);
  if (p_lab < 0.435) {
    return 5.12 * nucleon_mass
        / (mandelstam_s - 4 * nucleon_mass * nucleon_mass) + 1.67;
  } else if (p_lab < 0.8) {
    return 23.5 + 1000 * (p_lab - 0.7) * (p_lab - 0.7)
      * (p_lab - 0.7) *(p_lab - 0.7);
  } else if (p_lab < 2.0) {
    return 1250 / (p_lab + 50) - 4 * (p_lab - 1.3) * (p_lab - 1.3);
  } else if (p_lab < 2.776) {
    return 77 / (p_lab + 1.5);
  } else {
    const auto logp = std::log(p_lab);
    return 11.9 + 26.9 * std::pow(p_lab, -1.21) + 0.169 * logp * logp
           - 1.85 * logp;
  }
}

/** pp total cross section parametrization.
 * Sources:
 * low-p: \iref{Cugnon:1996kh}
 * highest-p: \iref{Buss:2011mx}
 */
float pp_total(double mandelstam_s) {
  double p_lab = plab_from_s(mandelstam_s);
  if (p_lab < 0.4) {
    return 34 * std::pow(p_lab / 0.4, -2.104);
  } else if (p_lab < 0.8) {
    return 23.5 + 1000 * (p_lab - 0.7) * (p_lab - 0.7)
      * (p_lab - 0.7) *(p_lab - 0.7);
  } else if (p_lab < 1.5) {
    return 23.5 + 24.6 / (1 + std::exp(-(p_lab - 1.2) / 0.1));
  } else if (p_lab < 5.0) {
    return 41 + 60 * (p_lab - 0.9) * std::exp(-1.2 * p_lab);
  } else {
    const auto logp = std::log(p_lab);
    return 48.0 + 0.522 * logp * logp - 4.51 * logp;
  }
}

/** np elastic cross section parametrization.
 * Source: \iref{Weil:2013mya}, eq. (45) */
float np_elastic(double mandelstam_s) {
  double p_lab = plab_from_s(mandelstam_s);
  if (p_lab < 0.525) {
    return 17.05 * nucleon_mass
        / (mandelstam_s - 4 * nucleon_mass * nucleon_mass) - 6.83;
  } else if (p_lab < 0.8) {
    return 33 + 196 * std::pow(std::abs(p_lab - 0.95), 2.5);
  } else if (p_lab < 2.0) {
    return 31 / std::sqrt(p_lab);
  } else if (p_lab < 2.776) {
    return 77 / (p_lab + 1.5);
  } else {
    const auto logp = std::log(p_lab);
    return 11.9 + 26.9 * std::pow(p_lab, -1.21) + 0.169 * logp * logp
           - 1.85 * logp;
  }
}

/** np total cross section parametrization.
 * Sources:
 * low-p: \iref{Cugnon:1996kh}
 * highest-p: \iref{Buss:2011mx}
 */
float np_total(double mandelstam_s) {
  double p_lab = plab_from_s(mandelstam_s);
  const auto logp = std::log(p_lab);
  if (p_lab < 0.4) {
    return 6.3555 * std::pow(p_lab, -3.2481) * std::exp(-0.377 * logp * logp);
  } else if (p_lab < 1.0) {
    return 33 + 196 * std::pow(std::abs(p_lab - 0.95), 2.5);
  } else if (p_lab < 2.0) {
    return 24.2 + 8.9 * p_lab;
  } else if (p_lab < 5.0) {
    return 42;
  } else {
    return 48.0 + 0.522 * logp * logp - 4.51 * logp;
  }
}

/** ppbar elastic cross section parametrization.
 * Source: \iref{Bass:1998ca} */
float ppbar_elastic(double mandelstam_s) {
  double p_lab = plab_from_s(mandelstam_s);
  if (p_lab < 0.3) {
    return 78.6;
  } else if (p_lab < 5.0) {
    return 31.6 + 18.3 / p_lab - 1.1 / (p_lab * p_lab) - 3.8 * p_lab;
  } else {
    const auto logp = std::log(p_lab);
    return 10.2 + 52.7 * std::pow(p_lab, -1.16) + 0.125 * logp * logp
           - 1.28 * logp;
  }
}

/** ppbar total cross section parametrization.
 * Source: \iref{Bass:1998ca} */
float ppbar_total(double mandelstam_s) {
  double p_lab = plab_from_s(mandelstam_s);
  if (p_lab < 0.3) {
    return 271.6 * std::exp(-1.1 * p_lab * p_lab);
  } else if (p_lab < 5.0) {
    return 75.0 + 43.1 / p_lab + 2.6 / (p_lab * p_lab) - 3.9 * p_lab;
  } else {
    const auto logp = std::log(p_lab);
    return 38.4 + 77.6 * std::pow(p_lab, -0.64) + 0.26 * logp * logp
           - 1.2 * logp;
  }
}

/** K+ p elastic cross section parametrization.
 * sigma(K+n->K+n) = sigma(K+n->K0p) = 0.5 * sigma(K+p->K+p)
 * Source: \iref{Buss:2011mx}, B.3.8 */
float kplusp_elastic(double mandelstam_s) {
  constexpr double a0 = 10.508;  // mb
  constexpr double a1 = -3.716;  // mb/GeV
  constexpr double a2 = 1.845;  // mb/GeV^2
  constexpr double a3 = -0.764;  // GeV^-1
  constexpr double a4 = 0.508;  // GeV^-2

  const double p_lab = plab_from_s(mandelstam_s, kaon_mass, nucleon_mass);
  const double p_lab2 = p_lab*p_lab;

  return (a0 + a1*p_lab + a2*p_lab2) / (1 + a3*p_lab + a4*p_lab2);
}

/** K+ n elastic cross section parametrization.
 * sigma(K+n->K+n) = sigma(K+n->K0p) = 0.5 * sigma(K+p->K+p)
 * Source: \iref{Buss:2011mx}, B.3.8 */
float kplusn_elastic(double mandelstam_s) {
  return 0.5 * kplusp_elastic(mandelstam_s);
}

/// PDG data on K- p elastic cross section: momentum in lab frame.
const std::initializer_list<double> KMINUSP_ELASTIC_P_LAB = {
  0.03000,   0.05000,   0.06287,   0.07000,  0.07044,  0.07300,  0.08730,
  0.09000,   0.11000,   0.11000,   0.11210,  0.11300,  0.12262,  0.13000,
  0.13000,   0.13722,   0.14552,   0.15000,  0.15000,  0.15300,  0.15909,
  0.16269,   0.17000,   0.17000,   0.17470,  0.18768,  0.18916,  0.19000,
  0.19000,   0.19300,   0.20534,   0.21000,  0.21000,  0.21554,  0.22049,
  0.22500,   0.23000,   0.23000,   0.23300,  0.23500,  0.23944,  0.24500,
  0.24623,   0.25000,   0.25500,   0.26154,  0.26500,  0.27000,  0.27500,
  0.27618,   0.28227,   0.28500,   0.29000,  0.29290,  0.29300,  0.29500,
  0.30000,   0.30191,   0.30500,   0.31500,  0.32500,  0.33500,  0.34500,
  0.35000,   0.35012,   0.35500,   0.36500,  0.37500,  0.38500,  0.38700,
  0.38986,   0.39200,   0.39500,   0.40000,  0.40500,  0.41500,  0.42000,
  0.42500,   0.43400,   0.43407,   0.43500,  0.43600,  0.44500,  0.45500,
  0.45500,   0.46500,   0.47500,   0.49500,  0.51300,  0.51300,  0.51400,
  0.52228,   0.53400,   0.55400,   0.57300,  0.59700,  0.61000,  0.61700,
  0.62004,   0.63700,   0.64200,   0.65800,  0.66000,  0.67200,  0.67700,
  0.69900,   0.70000,   0.70300,   0.70800,  0.71900,  0.72500,  0.73000,
  0.74000,   0.74000,   0.74100,   0.75800,  0.75996,  0.76100,  0.76800,
  0.77300,   0.77700,   0.77700,   0.78000,  0.78500,  0.79300,  0.80200,
  0.80600,   0.80600,   0.81000,   0.82000,  0.82000,  0.83300,  0.83800,
  0.83800,   0.84999,   0.85300,   0.85300,  0.85600,  0.86000,  0.87000,
  0.87400,   0.87400,   0.87600,   0.89400,  0.89400,  0.89900,  0.90000,
  0.90400,   0.90400,   0.90500,   0.91600,  0.91600,  0.92200,  0.92500,
  0.93500,   0.93500,   0.93500,   0.94000,  0.94300,  0.94500,  0.95400,
  0.95400,   0.95500,   0.96000,   0.96500,  0.97000,  0.97000,  0.98000,
  0.99000,   0.99100,   0.99100,   1.00500,  1.02000,  1.02200,  1.02200,
  1.04000,   1.04400,   1.04400,   1.04500,  1.06000,  1.06100,  1.06100,
  1.08000,   1.08000,   1.08500,   1.10000,  1.10200,  1.10200,  1.11700,
  1.11700,   1.12500,   1.13400,   1.13400,  1.13800,  1.14000,  1.15000,
  1.15000,   1.15300,   1.15300,   1.16100,  1.16500,  1.17400,  1.17400,
  1.17900,   1.18000,   1.18300,   1.18300,  1.20100,  1.20500,  1.22000,
  1.22600,   1.22600,   1.23300,   1.24500,  1.25300,  1.26000,  1.26000,
  1.26300,   1.26300,   1.27600,   1.28500,  1.29600,  1.30000,  1.31600,
  1.31600,   1.32000,   1.32800,   1.34000,  1.35500,  1.36800,  1.36800,
  1.38000,   1.38300,   1.41500,   1.41500,  1.42300,  1.43300,  1.46200,
  1.46500,   1.48300,   1.51300,   1.51400,  1.53000,  1.53400,  1.54500,
  1.54600,   1.58400,   1.60600,   1.60600,  1.63400,  1.65200,  1.65300,
  1.68000,   1.68400,   1.70500,   1.70500,  1.73400,  1.73900,  1.74100,
  1.78400,   1.80000,   1.80000,   1.81500,  1.84300,  1.84300,  1.88400,
  1.93400,   1.93400,   1.98400,   2.00000,  2.03100,  2.03400,  2.08400,
  2.13400,   2.13500,   2.17500,   2.23400,  2.24000,  2.28400,  2.32500,
  2.33100,   2.37400,   2.41200,   2.51600,  2.66000,  2.66000,  2.98500,
  3.00000,   3.00000,   3.46000,   3.59000,  3.65000,  4.10000,  4.20000,
  4.60000,   5.00000,   5.50000,   6.00000,  7.20020,  9.00010,  10.12000,
  14.30000,  14.30000,  25.20000,  32.10000, 40.10000, 50.00000, 70.00000,
  100.00000, 140.00000, 147.00000, 175.00000
};
/// PDG data on K- p elastic cross section: cross section.
const std::initializer_list<double> KMINUSP_ELASTIC_SIG = {
  313.50, 103.60, 113.00, 44.800, 58.500, 187.00, 92.000, 71.500, 92.800,
  87.290, 82.000, 105.00, 59.400, 40.400, 79.220, 82.000, 49.000, 41.400,
  69.610, 108.00, 53.900, 98.000, 75.760, 32.800, 45.000, 73.000, 66.000,
  59.090, 53.300, 68.000, 37.000, 53.300, 60.490, 48.000, 41.000, 65.410,
  62.900, 55.690, 50.000, 55.740, 41.200, 53.240, 37.000, 51.500, 49.220,
  43.600, 47.710, 58.060, 48.750, 30.000, 44.900, 39.420, 38.270, 47.800,
  48.200, 41.220, 44.500, 44.200, 40.360, 37.020, 40.280, 37.840, 37.260,
  34.000, 33.500, 34.770, 34.210, 36.670, 33.890, 31.900, 34.700, 34.000,
  38.470, 38.900, 32.060, 32.590, 48.400, 31.190, 30.600, 32.800, 26.830,
  25.800, 28.830, 23.800, 30.320, 31.990, 23.100, 21.500, 26.500, 27.600,
  21.700, 35.000, 19.300, 19.100, 17.500, 17.700, 17.660, 18.600, 16.000,
  16.000, 17.230, 16.400, 12.100, 16.220, 15.600, 15.200, 14.200, 15.220,
  13.500, 14.200, 11.500, 14.070, 15.900, 14.000, 11.600, 16.930, 16.700,
  17.300, 15.200, 18.600, 18.300, 18.700, 17.900, 19.370, 20.500, 19.300,
  20.000, 20.450, 20.670, 18.700, 19.300, 19.630, 19.800, 19.530, 22.400,
  19.100, 19.390, 19.780, 19.500, 21.360, 20.100, 20.310, 21.070, 21.600,
  21.660, 22.020, 21.500, 20.900, 20.710, 21.340, 20.850, 20.400, 22.340,
  20.940, 20.120, 20.100, 19.980, 21.800, 21.010, 19.330, 20.640, 20.700,
  20.610, 21.270, 20.340, 20.400, 20.720, 22.400, 21.220, 21.400, 22.150,
  21.750, 20.800, 22.100, 22.150, 23.300, 21.500, 21.460, 22.220, 21.200,
  20.600, 20.560, 18.700, 18.740, 19.830, 18.300, 18.600, 18.770, 17.600,
  17.820, 17.890, 17.000, 17.750, 15.700, 17.000, 18.300, 17.300, 17.200,
  17.230, 15.300, 15.390, 16.500, 16.460, 14.660, 16.800, 15.900, 15.890,
  12.680, 13.890, 15.700, 11.300, 11.810, 13.110, 12.320, 11.870, 15.200,
  14.000, 10.870, 10.870, 11.440, 10.960, 11.570, 12.000, 10.200, 11.200,
  10.260, 9.7400, 14.400, 9.5300, 10.300, 10.300, 16.600, 10.500, 8.8300,
  8.8300, 8.4200, 8.6000, 9.1100, 9.1100, 8.3000, 7.7000, 7.7000, 8.1500,
  8.8000, 8.0600, 8.0600, 8.7000, 8.8600, 8.8600, 9.2000, 8.4000, 8.4000,
  8.1900, 8.9000, 9.0800, 9.0800, 9.5000, 8.5100, 8.5100, 9.0000, 8.1300,
  8.1300, 6.9500, 7.8600, 7.8600, 9.0000, 7.9000, 7.4800, 7.8000, 7.4600,
  7.3100, 7.1000, 7.4000, 7.1000, 7.2600, 7.9000, 6.8700, 6.2000, 7.3000,
  6.5000, 6.7600, 6.6000, 6.3200, 6.2100, 5.7000, 6.0400, 4.9500, 5.0600,
  4.9500, 4.9400, 4.4000, 4.6000, 4.3000, 4.5000, 4.2000, 3.8400, 4.1000,
  3.6200, 4.2300, 3.9500, 3.2400, 2.9600, 3.0100, 2.4600, 2.5600, 2.3300,
  2.5400, 2.5300, 2.5100, 2.5200, 2.7400, 2.5900
};
static std::unique_ptr<InterpolateDataLinear<double>>
    kminusp_elastic_interpolation = nullptr;

/// PDG data on K- p total cross section: momentum in lab frame.
const std::initializer_list<double> KMINUSP_TOT_PLAB = {
    0.245,   0.255,   0.265,   0.275,   0.285,   0.293,   0.293,   0.295,
    0.300,   0.305,   0.315,   0.325,   0.335,   0.345,   0.350,   0.350,
    0.355,   0.360,   0.365,   0.375,   0.385,   0.385,   0.387,   0.390,
    0.392,   0.395,   0.400,   0.405,   0.405,   0.415,   0.419,   0.425,
    0.434,   0.434,   0.435,   0.436,   0.440,   0.445,   0.446,   0.455,
    0.455,   0.464,   0.475,   0.475,   0.483,   0.494,   0.495,   0.506,
    0.506,   0.513,   0.513,   0.514,   0.520,   0.534,   0.534,   0.536,
    0.547,   0.554,   0.561,   0.566,   0.573,   0.574,   0.586,   0.590,
    0.596,   0.597,   0.599,   0.604,   0.612,   0.617,   0.620,   0.623,
    0.625,   0.627,   0.630,   0.637,   0.639,   0.653,   0.655,   0.657,
    0.658,   0.667,   0.677,   0.680,   0.686,   0.687,   0.693,   0.699,
    0.707,   0.708,   0.712,   0.719,   0.720,   0.725,   0.732,   0.736,
    0.740,   0.741,   0.744,   0.752,   0.756,   0.760,   0.761,   0.767,
    0.768,   0.768,   0.773,   0.777,   0.782,   0.793,   0.794,   0.795,
    0.802,   0.806,   0.819,   0.820,   0.822,   0.838,   0.839,   0.840,
    0.848,   0.850,   0.853,   0.860,   0.874,   0.874,   0.881,   0.894,
    0.894,   0.899,   0.900,   0.900,   0.904,   0.916,   0.925,   0.931,
    0.935,   0.936,   0.952,   0.954,   0.965,   0.969,   0.970,   0.975,
    0.978,   0.980,   0.987,   0.991,   0.991,   1.020,   1.022,   1.022,
    1.023,   1.030,   1.043,   1.044,   1.061,   1.067,   1.068,   1.070,
    1.080,   1.080,   1.093,   1.100,   1.101,   1.102,   1.115,   1.117,
    1.119,   1.130,   1.134,   1.144,   1.150,   1.153,   1.169,   1.169,
    1.174,   1.179,   1.183,   1.194,   1.219,   1.220,   1.226,   1.230,
    1.230,   1.244,   1.263,   1.270,   1.274,   1.283,   1.306,   1.316,
    1.320,   1.325,   1.328,   1.336,   1.350,   1.368,   1.370,   1.375,
    1.383,   1.384,   1.406,   1.415,   1.433,   1.434,   1.442,   1.462,
    1.480,   1.483,   1.485,   1.488,   1.509,   1.514,   1.521,   1.534,
    1.546,   1.549,   1.562,   1.583,   1.584,   1.600,   1.606,   1.614,
    1.634,   1.637,   1.653,   1.663,   1.684,   1.687,   1.705,   1.710,
    1.730,   1.734,   1.735,   1.741,   1.763,   1.784,   1.785,   1.800,
    1.805,   1.835,   1.840,   1.843,   1.855,   1.879,   1.884,   1.885,
    1.903,   1.924,   1.929,   1.934,   1.950,   1.953,   1.984,   1.985,
    2.005,   2.031,   2.034,   2.035,   2.052,   2.080,   2.084,   2.104,
    2.124,   2.134,   2.135,   2.159,   2.164,   2.175,   2.206,   2.207,
    2.234,   2.240,   2.250,   2.257,   2.284,   2.301,   2.308,   2.325,
    2.331,   2.350,   2.351,   2.356,   2.374,   2.392,   2.400,   2.404,
    2.423,   2.450,   2.467,   2.480,   2.500,   2.514,   2.550,   2.558,
    2.600,   2.608,   2.630,   2.650,   2.657,   2.660,   2.660,   2.670,
    2.700,   2.700,   2.750,   2.800,   2.850,   2.880,   2.900,   2.950,
    2.970,   2.980,   3.000,   3.000,   3.050,   3.090,   3.100,   3.150,
    3.190,   3.200,   3.250,   3.300,   3.300,   3.500,   3.710,   3.920,
    3.980,   4.000,   4.130,   4.200,   4.250,   4.340,   4.760,   5.180,
    5.500,   6.000,   6.650,   7.000,   7.000,   8.000,   8.500,  10.000,
   10.000,  10.000,  10.000,  10.000,  10.100,  11.500,  12.000,  12.000,
   12.600,  13.300,  13.400,  14.000,  14.500,  15.000,  15.800,  16.000,
   17.500,  18.000,  19.100,  20.000,  20.000,  20.000,  21.000,  21.000,
   23.000,  25.000,  25.000,  25.000,  27.500,  30.000,  30.000,  30.000,
   31.500,  32.000,  33.500,  35.000,  35.000,  35.500,  37.500,  40.000,
   40.000,  40.000,  40.000,  42.500,  45.000,  45.000,  45.000,  47.500,
   50.000,  50.000,  50.000,  50.000,  50.000,  52.500,  55.000,  55.000,
   55.000,  70.000, 100.000, 100.000, 100.000, 120.000, 147.000, 150.000,
  150.000, 170.000, 175.000, 200.000, 200.000, 240.000, 280.000, 310.000
};
/// PDG data on K- p total cross section: cross section.
const std::initializer_list<double> KMINUSP_TOT_SIG = {
  113.80,  98.00,  94.00,  96.70,  75.10,  89.30,  90.70,  82.50,  79.40,
   78.60,  70.90,  76.00,  71.50,  71.30,  70.20,  69.30,  68.80,  69.32,
   70.00,  81.80, 100.19,  88.30,  69.60,  77.80,  80.60,  91.60,  76.20,
   69.80,  76.60,  70.70,  58.61,  66.70,  61.50,  62.80,  54.90,  56.70,
   58.41,  55.70,  50.27,  57.20,  48.60,  46.82,  45.30,  48.45,  44.19,
   43.19,  42.30,  42.70,  44.71,  50.10,  49.50,  40.70,  40.11,  40.50,
   39.55,  42.15,  38.34,  39.30,  37.78,  39.10,  35.60,  36.74,  36.32,
   32.99,  37.80,  36.50,  36.03,  38.59,  35.07,  36.30,  32.40,  34.40,
   34.75,  36.36,  35.40,  32.70,  34.41,  34.31,  34.44,  35.40,  34.90,
   34.38,  33.70,  34.34,  35.33,  34.16,  34.81,  33.10,  34.63,  32.60,
   35.37,  35.40,  34.69,  33.30,  35.26,  35.64,  36.80,  31.90,  37.65,
   36.40,  38.84,  40.10,  38.10,  38.57,  38.50,  39.87,  40.60,  40.70,
   40.31,  40.90,  40.96,  40.53,  40.20,  41.30,  40.67,  39.70,  40.29,
   37.30,  39.86,  36.10,  40.58,  40.60,  38.10,  41.13,  41.75,  41.30,
   42.36,  43.70,  42.70,  43.41,  43.41,  52.00,  42.10,  43.20,  44.86,
   44.10,  42.40,  45.46,  46.24,  42.90,  47.33,  47.16,  43.20,  47.19,
   47.96,  47.10,  45.30,  45.80,  49.79,  51.70,  51.27,  47.70,  52.19,
   48.40,  52.17,  51.90,  50.30,  49.40,  51.46,  49.30,  50.51,  46.90,
   46.13,  43.60,  44.50,  43.50,  44.18,  40.90,  42.23,  43.74,  39.30,
   39.80,  45.00,  40.50,  39.18,  38.67,  42.00,  39.01,  40.70,  36.92,
   35.21,  35.51,  33.40,  35.41,  33.80,  33.39,  32.36,  32.52,  32.11,
   32.50,  31.17,  29.68,  31.03,  30.60,  30.50,  31.12,  31.20,  32.61,
   31.27,  30.90,  31.20,  31.09,  31.63,  29.78,  31.90,  31.83,  32.28,
   31.68,  32.50,  32.90,  33.35,  32.83,  33.47,  29.87,  33.53,  33.80,
   32.16,  34.00,  33.97,  34.17,  34.20,  32.50,  33.19,  34.42,  34.20,
   34.07,  32.79,  34.19,  33.80,  33.84,  33.20,  33.66,  32.50,  33.20,
   33.19,  33.42,  32.77,  32.30,  32.26,  32.94,  31.92,  31.53,  31.19,
   31.32,  30.72,  30.85,  30.40,  30.81,  30.18,  30.40,  30.49,  30.14,
   30.50,  30.46,  30.10,  30.17,  30.37,  30.71,  29.90,  29.99,  29.86,
   29.89,  29.80,  29.70,  29.81,  29.60,  31.77,  29.83,  29.79,  29.70,
   29.87,  30.05,  30.75,  29.79,  29.79,  29.46,  29.50,  29.63,  29.37,
   29.30,  31.89,  29.44,  29.38,  28.81,  28.90,  29.07,  29.10,  28.86,
   28.88,  28.87,  28.50,  26.90,  28.46,  28.37,  28.30,  28.13,  28.21,
   27.88,  30.30,  28.10,  27.96,  30.40,  30.20,  27.41,  28.02,  30.90,
   27.83,  27.72,  27.73,  26.65,  27.58,  27.51,  25.30,  27.76,  24.90,
   27.42,  27.20,  26.25,  27.00,  26.98,  26.75,  26.78,  26.70,  26.73,
   27.00,  27.15,  26.90,  25.95,  25.40,  28.20,  25.14,  25.40,  25.80,
   25.84,  24.74,  24.03,  24.30,  24.00,  23.80,  25.00,  23.50,  23.60,
   24.60,  23.20,  22.90,  22.60,  22.67,  22.50,  22.50,  23.30,  22.90,
   21.60,  21.60,  22.10,  21.50,  21.50,  21.60,  21.60,  21.90,  21.30,
   21.90,  21.00,  21.60,  21.20,  21.02,  22.40,  21.16,  21.09,  20.89,
   20.72,  20.70,  20.81,  20.72,  21.30,  20.84,  20.57,  20.50,  19.90,
   20.55,  20.80,  20.54,  20.43,  20.39,  20.90,  20.59,  20.62,  20.49,
   20.35,  20.60,  20.57,  20.45,  20.45,  21.00,  20.48,  20.30,  20.25,
   20.40,  20.32,  21.50,  20.49,  20.23,  20.38,  20.80,  20.45,  20.41,
   20.59,  21.50,  20.60,  20.57,  20.65,  21.40,  20.79,  20.84,  21.30,
   21.32,  21.45
};
static std::unique_ptr<InterpolateDataLinear<double>>
    kminusp_total_interpolation = nullptr;

/// Center-of-mass energy.
const std::initializer_list<double> KMINUSP_RES_SQRTS = {
  1.4325, 1.4500, 1.4700, 1.4900, 1.5100, 1.5300, 1.5500, 1.5700, 1.5900, 1.6100,
  1.6300, 1.6500, 1.6700, 1.6900, 1.7100, 1.7300, 1.7500, 1.7700, 1.7900, 1.8100,
  1.8300, 1.8500, 1.8700, 1.8900, 1.9100, 1.9300, 1.9500, 1.9700, 1.9900, 2.0100,
  2.0300, 2.0500, 2.0700, 2.0900, 2.1100, 2.1300, 2.1500, 2.1700, 2.1900, 2.2100,
  2.2300, 2.2500, 2.2700, 2.2900, 2.3100, 2.3300, 2.3500, 2.3700, 2.3900, 2.4100,
  2.4300, 2.4500, 2.4700, 2.4900, 2.5100, 2.5300
};
/// Elastic $K^-$ p cross section contributions from decays.
///
/// These need to be subtracted from the interpolation of the PDG data on
/// elastic cross sections. This data was generated using the SMASH analysis
/// suite and should be updated when strange resonances are changed or added.
const std::initializer_list<double> KMINUSP_RES_SIG = {
   0.33367129511,  0.50169807071,  0.81215608591,  1.81425185871,  9.73878541251,
   4.72942298787,  1.64110137713,  1.29907690636,  1.45132667219,  1.71867719084,
   2.39088023526,  3.71927701425,  5.20320581341,  5.39379062402,  5.73765670210,
   7.13462691623,  9.59060875433, 11.52191181500, 14.55085371550, 16.60736427130,
  14.02383890030,  9.82138787521,  7.74700751400,  5.70551632076,  4.37202276878,
   3.06489586700,  2.28183945733,  1.83920912408,  1.40165435359,  1.06008207577,
   0.81583198079,  0.76663983079,  0.61601816015,  0.55005296573,  0.48486316803,
   0.39262267967,  0.36575657252,  0.33427926436,  0.26292819528,  0.26549081727,
   0.22493534086,  0.23026481368,  0.18399816802,  0.16801899889,  0.17045653508,
   0.13612053285,  0.13359129739,  0.11907757417,  0.11595499898,  0.10646106065,
   0.10646488139,  0.08225463854,  0.08293506562,  0.08910192954,  0.07059081341,
   0.06994780581
};
static std::unique_ptr<InterpolateDataSpline> kminusp_elastic_res_interpolation
    = nullptr;

/** K- p elastic cross section parametrization, PDG data.
 *
 * The PDG data is being interpolated using cubic splines. If more than one
 * cross section was given for one p_lab value, the corresponding cross sections
 * are averaged.
 */
static float kminusp_elastic_pdg(double mandelstam_s) {
  if (kminusp_elastic_interpolation == nullptr) {
    std::vector<double> x = KMINUSP_ELASTIC_P_LAB;
    std::vector<double> y = KMINUSP_ELASTIC_SIG;
    std::vector<double> dedup_x;
    std::vector<double> dedup_y;
    std::tie(dedup_x, dedup_y) = dedup_avg(x, y);
    dedup_y = smooth(dedup_x, dedup_y, 0.1, 5);
    kminusp_elastic_interpolation =
        make_unique<InterpolateDataLinear<double>>(dedup_x, dedup_y);
    /*
    // Output interpolation for plotting.
    constexpr int N = 10000;
    const double x_min = 0.1;
    const double x_max = 100;
    std::cout << "\n-------------------\n";
    for (int i = 0; i < N; i++) {
        const double xi = x_min + (x_max - x_min) * (i /
    static_cast<double>(N));
        std::cout << xi << " " << (*kminusp_elastic_interpolation)(xi) << "\n";
    }
    std::cout << "-------------------" << std::endl;
    */
  }
  const double p_lab = plab_from_s(mandelstam_s, kaon_mass, nucleon_mass);
  return (*kminusp_elastic_interpolation)(p_lab);
}

/** K- p elastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.9 */
float kminusp_elastic(double mandelstam_s) {
  const double p_lab = plab_from_s(mandelstam_s, kaon_mass, nucleon_mass);
  double sigma;
  if (std::sqrt(mandelstam_s) < 1.68) {
    // The parametrization here also works for anti-K0 n, Lambda pi0,
    // Sigma+ pi-, Sigma- pi+, Sigma0 pi0 with different parameters a0, a1, a2.
    //
    // The values of the parameters are *not* taken from the source above,
    // they come from a fit to PDG data.
    constexpr double a0 = 186.03567644;  // mb GeV^2
    constexpr double a1 = 0.22002795;  // Gev
    constexpr double a2 = 0.64907116;

    const double p_i = p_lab;
    const double p_f = p_lab;

    const double ratio = a1*a1 / (a1*a1 + p_f*p_f);
    sigma = a0 * p_f / (p_i * mandelstam_s) * std::pow(ratio, a2);
  } else {
    sigma = kminusp_elastic_pdg(mandelstam_s);
  }
  // The elastic contributions from decays still need to be subtracted.
  if (kminusp_elastic_res_interpolation == nullptr) {
    std::vector<double> x = KMINUSP_RES_SQRTS;
    for (auto& i : x) {
      i = plab_from_s(i * i, kaon_mass, nucleon_mass);
    }
    std::vector<double> y = KMINUSP_RES_SIG;
    kminusp_elastic_res_interpolation =
        make_unique<InterpolateDataSpline>(x, y);
  }
  const auto old_sigma = sigma;
  sigma -= (*kminusp_elastic_res_interpolation)(p_lab);
  if (sigma < 0) {
    std::cout << "NEGATIVE SIGMA: sigma=" << sigma
              << ", sqrt(s)=" << std::sqrt(mandelstam_s)
              << ", sig_el_exp=" << old_sigma
              << ", sig_el_res=" << (*kminusp_elastic_res_interpolation)(p_lab)
              << std::endl;
  }
  assert(sigma >= 0);
  return sigma;
}

/** K- n elastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.9 */
float kminusn_elastic(double) { return 4.0; }

/** K0 p elastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.9 */
float k0p_elastic(double mandelstam_s) {
  // by isospin symmetry
  return kplusn_elastic(mandelstam_s);
}

/** K0 n elastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.9 */
float k0n_elastic(double mandelstam_s) {
  // by isospin symmetry
  return kplusp_elastic(mandelstam_s);
}

/** Kbar0 p elastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.9 */
float kbar0p_elastic(double mandelstam_s) {
  // by isospin symmetry
  return kminusn_elastic(mandelstam_s);
}

/** Kbar0 n elastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.9 */
float kbar0n_elastic(double mandelstam_s) {
  // by isospin symmetry
  return kminusp_elastic(mandelstam_s);
}


/// PDG data on K+ N total cross section: momentum in lab frame.
const std::initializer_list<double> KPLUSN_TOTAL_P_LAB = {
    0.770,   0.888,   0.939,   0.970,   0.989,   1.040,   1.091,   1.141,
    1.170,   1.191,   1.242,   1.292,   1.300,   1.342,   1.392,   1.440,
    1.442,   1.492,   1.550,   1.593,   1.600,   1.643,   1.690,   1.693,
    1.700,   1.743,   1.750,   1.793,   1.800,   1.850,   1.893,   1.900,
    1.950,   1.970,   1.993,   2.000,   2.050,   2.093,   2.100,   2.150,
    2.193,   2.200,   2.260,   2.300,   2.350,   2.393,   2.400,   2.450,
    2.500,   2.550,   2.550,   2.600,   2.650,   2.700,   2.750,   2.800,
    2.830,   2.850,   2.900,   2.950,   3.000,   3.050,   3.100,   3.150,
    3.200,   3.250,   3.300,   6.000,   8.000,  10.000,  12.000,  14.000,
   15.000,  16.000,  18.000,  20.000,  20.000,  25.000,  30.000,  35.000,
   35.000,  40.000,  45.000,  50.000,  50.000,  50.000,  55.000,  70.000,
  100.000, 100.000, 120.000, 150.000, 150.000, 170.000, 200.000, 200.000,
  240.000, 280.000, 310.000
};
/// PDG data on K+ N total cross section: cross section.
const std::initializer_list<double> KPLUSN_TOTAL_SIG = {
  15.50, 16.85, 17.60, 17.80, 18.53, 18.91, 20.61, 21.25, 18.20, 20.87, 20.26,
  19.68, 18.50, 19.32, 19.22, 18.10, 19.07, 18.95, 18.91, 18.79, 18.89, 18.67,
  18.50, 18.69, 18.83, 18.88, 18.86, 18.73, 18.53, 18.66, 18.50, 18.69, 18.70,
  18.60, 18.55, 18.79, 18.54, 18.67, 18.49, 18.43, 18.40, 18.40, 17.70, 18.27,
  18.26, 18.63, 18.09, 18.25, 18.11, 17.10, 18.17, 18.09, 18.02, 18.11, 18.06,
  18.01, 17.50, 17.95, 17.85, 17.81, 17.81, 17.83, 17.85, 17.61, 17.61, 17.66,
  17.55, 17.50, 17.60, 17.50, 17.60, 17.50, 17.87, 17.40, 17.60, 17.94, 17.70,
  17.78, 17.69, 18.29, 18.12, 18.15, 18.30, 18.66, 18.56, 18.02, 18.43, 18.60,
  19.04, 18.99, 19.23, 19.63, 19.55, 19.74, 19.72, 19.82, 20.37, 20.61, 20.80,
};
static std::unique_ptr<InterpolateDataLinear<double>>
    kplusp_total_interpolation = nullptr;

/** K+ p inelastic cross section parametrization.
 * Source: \iref{Buss:2011mx}, B.3.8 */
float kplusp_inelastic(double mandelstam_s) {
  if (kplusp_total_interpolation == nullptr) {
    std::vector<double> x = KPLUSN_TOTAL_P_LAB;
    std::vector<double> y = KPLUSN_TOTAL_SIG;
    std::vector<double> dedup_x;
    std::vector<double> dedup_y;
    std::tie(dedup_x, dedup_y) = dedup_avg(x, y);
    dedup_y = smooth(dedup_x, dedup_y, 0.1, 5);
    kplusp_total_interpolation =
        make_unique<InterpolateDataLinear<double>>(dedup_x, dedup_y);
  }
  const double p_lab = plab_from_s(mandelstam_s, kaon_mass, nucleon_mass);
  return (*kplusp_total_interpolation)(p_lab)
         - kplusp_elastic(mandelstam_s);
}


/* Parametrizations of strangeness exchange channels
 *
 * Taken from UrQMD (\iref{Graef:2014mra}).
 */

float kminusp_piminussigmaplus(double sqrts) {
  return 0.0788265 / Smash::square(sqrts - 1.38841);
}

float kminusp_piplussigmaminus(double sqrts) {
  return 0.0196741 / Smash::square(sqrts - 1.42318);
}

float kminusp_pi0sigma0(double sqrts) {
  return 0.55 * 0.0508208 / Smash::square(sqrts - 1.38837);
}

float kminusp_pi0lambda(double sqrts) {
  return 0.45 * 0.0508208 / Smash::square(sqrts - 1.38837);
}

// The other channels follow from the paramatreziation with the same strange
// product via isospin symmetry.

float kminusn_piminussigma0(double sqrts) {
  return 1./6 * 2 * kminusp_pi0sigma0(sqrts);
}

float kminusn_pi0sigmaminus(double sqrts) {
  return (0.25 + 1./6) * 2 * kminusp_piplussigmaminus(sqrts);
}

float kminusn_piminuslambda(double sqrts) {
  return 0.5 * kminusp_pi0lambda(sqrts);
}

// All K+ p and K+ n channels are forbidden by isospin.

// Two hyperon exchange, based on effective model by Feng Li,
// as in UrQMD (\iref{Graef:2014mra}).

float lambdalambda_ximinusp(double sqrts_sqrts0, double p_N, double p_lambda) {
  assert(p_lambda != 0);
  assert(sqrts_sqrts0 >= 0);
  return 37.15 / 2 * p_N / p_lambda * std::pow(sqrts_sqrts0, -0.16);
}

float lambdalambda_xi0n(double sqrts_sqrts0, double p_N, double p_lambda) {
  return lambdalambda_ximinusp(sqrts_sqrts0, p_N, p_lambda);
}

float lambdasigmaplus_xi0p(double sqrts_sqrts0) {
  assert(sqrts_sqrts0 >= 0);
  return 24.3781 * std::pow(sqrts_sqrts0, -0.479);
}

float lambdasigmaminus_ximinusn(double sqrts_sqrts0) {
  return lambdasigmaplus_xi0p(sqrts_sqrts0);
}

float lambdasigma0_ximinusp(double sqrts_sqrts0) {
  assert(sqrts_sqrts0 >= 0);
  if (sqrts_sqrts0 < 0.03336) {
    return 6.475 * std::pow(sqrts_sqrts0, -0.4167);
  } else {
    return 14.5054 * std::pow(sqrts_sqrts0, -0.1795);
  }
}

float lambdasigma0_xi0n(double sqrts_sqrts0) {
  return lambdasigma0_ximinusp(sqrts_sqrts0);
}

float sigma0sigma0_ximinusp(double sqrts_sqrts0) {
  assert(sqrts_sqrts0 >= 0);
  if (sqrts_sqrts0 < 0.09047) {
    return 5.625 * std::pow(sqrts_sqrts0, -0.318);
  } else {
    return 4.174 * std::pow(sqrts_sqrts0, -0.4421);
  }
}

// Note that there is a typo in the paper in equation (6):
// "Lambda Sigma0 -> Xi0 n" should be "Sigma0 Sigma0 -> Xi0 n".
float sigma0sigma0_xi0n(double sqrts_sqrts0) {
  return sigma0sigma0_ximinusp(sqrts_sqrts0);
}

float sigmaplussigmaminus_xi0p(double sqrts_sqrts0) {
  return 4 * sigma0sigma0_ximinusp(sqrts_sqrts0);
}

float sigma0sigmaminus_ximinusn(double sqrts_sqrts0) {
  return 4 * sigma0sigma0_ximinusp(sqrts_sqrts0);
}

float sigmaplussigmaminus_ximinusp(double sqrts_sqrts0) {
  return 14.194 * std::pow(sqrts_sqrts0, -0.442);
}

float sigmaplussigmaminus_xi0n(double sqrts_sqrts0) {
  return sigmaplussigmaminus_ximinusp(sqrts_sqrts0);
}

}  // namespace Smash
