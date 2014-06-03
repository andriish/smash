/*
 *
 *    Copyright (c) 2014
 *      SMASH Team
 *
 *    GNU General Public License (GPLv3 or later)
 *
 */

#ifndef SRC_INCLUDE_THREEVECTOR_H_
#define SRC_INCLUDE_THREEVECTOR_H_

#include <array>
#include <cmath>

namespace Smash {

/**
 * The ThreeVector class represents a physical three-vector
 * \f$ \vec{x} = (x_1,x_2,x_3)\f$
 * with the components \f$ x_1,x_2,x_3 \f$.
 * It is related to the classes FourVector and Angles,
 * both of which can be converted into a ThreeVector using
 * the 'threevec()' method.
 */
class ThreeVector {
 public:
  /// default constructor (nulls all components)
  ThreeVector() {
    x_ = {0., 0., 0.};
  }
  /// copy constructor
  ThreeVector(double y1, double y2, double y3) {
    x_ = {y1, y2, y3};
  }
  /// retrieve first component
  double inline x1() const;
  /// set first component
  void inline set_x1(double x);
  /// retrieve second component
  double inline x2() const;
  /// set second component
  void inline set_x2(double y);
  /// retrieve third component
  double inline x3() const;
  /// set third component
  void inline set_x3(double z);
  /// calculate the square of the vector (which is a scalar)
  double inline sqr() const;
  /// calculate the absolute value
  double inline abs() const;
  /// negation: Returns \f$-\vec x\f$
  ThreeVector inline operator- () const;
  /// increase this vector by \f$\vec v: \vec x^\prime = \vec x + \vec v\f$
  ThreeVector inline operator+= (const ThreeVector &v);
  /// decrease this vector by \f$\vec v: \vec x^\prime = \vec x - \vec v\f$
  ThreeVector inline operator-= (const ThreeVector &v);
  /// scale this vector by \f$a: \vec x^\prime = a \cdot \vec x\f$
  ThreeVector inline operator*= (const double &a);
  /// divide this vector by \f$a: \vec x^\prime = \frac{1}{a} \cdot \vec x\f$
  ThreeVector inline operator/= (const double &a);
 private:
  /// the internal storage of the components.
  std::array<double, 3> x_;
};

double inline ThreeVector::x1() const {
  return x_[0];
}

void inline ThreeVector::set_x1(const double x) {
  x_[0] = x;
}

double inline ThreeVector::x2() const {
  return x_[1];
}

void inline ThreeVector::set_x2(const double y) {
  x_[1] = y;
}

double inline ThreeVector::x3() const {
  return x_[2];
}

void inline ThreeVector::set_x3(const double z) {
  x_[2] = z;
}

ThreeVector inline ThreeVector::operator- () const{
  ThreeVector neg(-x_[0],-x_[1],-x_[2]);
  return neg;
}

ThreeVector inline ThreeVector::operator+= (const ThreeVector &v) {
  x_[0] += v.x_[0];
  x_[1] += v.x_[1];
  x_[2] += v.x_[2];
  return *this;
}

ThreeVector inline operator+ (ThreeVector a, const ThreeVector &b) {
  a += b;
  return a;
}

ThreeVector inline ThreeVector::operator-= (const ThreeVector &v) {
  x_[0] -= v.x_[0];
  x_[1] -= v.x_[1];
  x_[2] -= v.x_[2];
  return *this;
}

ThreeVector inline operator- (ThreeVector a, const ThreeVector &b) {
  a -= b;
  return a;
}

ThreeVector inline ThreeVector::operator*= (const double &a) {
  x_[0] *= a;
  x_[1] *= a;
  x_[2] *= a;
  return *this;
}

inline ThreeVector operator* (ThreeVector a, const double &b) {
  a *= b;
  return a;
}

inline double operator* (ThreeVector a, const ThreeVector &b) {
  return a.x1()*b.x1() + a.x2()*b.x2() + a.x3()*b.x3();
}

ThreeVector inline ThreeVector::operator/= (const double &a) {
  x_[0] /= a;
  x_[1] /= a;
  x_[2] /= a;
  return *this;
}

ThreeVector inline operator/ (ThreeVector a, const double &b) {
  a /= b;
  return a;
}

double inline ThreeVector::sqr() const {
  return (*this)*(*this);
}

double inline ThreeVector::abs() const {
  return std::sqrt((*this)*(*this));
}

}  // namespace Smash

#endif  // SRC_INCLUDE_THREEVECTOR_H_