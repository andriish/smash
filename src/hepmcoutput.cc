/*
 *
 *    Copyright (c) 2014-2021
 *      SMASH Team
 *
 *    GNU General Public License (GPLv3 or later)
 *
 */

#include "smash/hepmcoutput.h"

#include "HepMC3/Print.h"
#include "HepMC3/WriterRootTree.h"

namespace smash {

/*!\Userguide
 * \page output_hepmc_ HepMC Output
 *
 * SMASH HepMC output is an implementation of the HepMC3 Event Record Library.
 * The aim is to provide a format compatible with other frameworks like Rivet
 * (https://rivet.hepforge.org). For resources regarding HepMC, see
 * http://hepmc.web.cern.ch/hepmc/ and https://arxiv.org/abs/1912.08005.
 *
 * Producing HepMC output requires HepMC3 to be installed. Download the tarball
 * from http://hepmc.web.cern.ch/hepmc/ and follow the instructions.
 *
 * \section output_hepmc_asciiv3_ ASCII HepMC Format
 *
 * HepMC generally structures each event into particles and vertices
 * connecting them, basically storing a graph of the event.

 * Two versions of HepMC are possible, if the HepMC format is specficified
 * under \key Particles. The output only provides a particle list of the
 * final state. For this only one central vertex is used. All initial state
 * particles are incoming particles and all final state
 * particles are outgoing particles of this vertex. Scatterings
 * happening during the SMASH event are not recorded. For the collider
 * modus, the intial state particles are combined into two single
 * colliding nucleus "particles" with a nuclear pdg code.
 *
 * In certain circumstances one may be interested in the full
 * event structure.  To that end, the output module provides the
 * HepMC format also for the \key Collisions content. With this format, the
 * full event tree is written.  Furthermore, as above, for collider modus,
 * we lump all incoming nucleons into nuclei, but split them out immediately
 * afterwards to allow tracking of the individual nucleons.
 *
 * In all cases, the module will track the number of binary, inelastic
 * collisions between incident nucleons as well as keep track of
 * participating incident nucleons.
 *
 * You can find a snippet of the configuration for this output in \ref
 * configuring_output_.
 *
 * The output is written in Asciiv3, the HepMC3 native plain text format. See
 * https://arxiv.org/abs/1912.08005 for documentation of the format.
 *
 * \note
 * - Since some HepMC readers (e.g. Rivet) need a value for the
 * nuclei-nuclei cross section, a dummy cross section of 1.0 is written to the
 * output.
 * - To avoid confusion with the definition of these quantities within the
 * Glauber model, in the header of an event we set the numbers of participants
 * and of collisions to -1.
 * - If you use Fermi motion and want to read in the HepMC
 * ouput into Rivet, you need to disable the check for the beam particle
 * energies with the \key --ignore-beams option. When using the Rivet output
 * this check is disabled by default.
 **/
HepMcOutput::HepMcOutput(const bf::path &path, std::string name,
                         const bool full_event)
    : HepMcInterface(name, full_event), filename_(path / (name + ".asciiv3")) {
  filename_unfinished_ = filename_;
  filename_unfinished_ += +".unfinished";
  output_file_ = make_unique<HepMC3::WriterRootTree>(filename_unfinished_.string(),
                                                  event_.run_info());
}
HepMcOutput::~HepMcOutput() {
  logg[LOutput].debug() << "Renaming file " << filename_unfinished_ << " to "
                        << filename_ << std::endl;
  bf::rename(filename_unfinished_, filename_);
}

void HepMcOutput::at_eventend(const Particles &particles,
                              const int32_t event_number,
                              const EventInfo &event) {
  HepMcInterface::at_eventend(particles, event_number, event);
  logg[LOutput].debug() << "Writing event " << event_number << " with "
                        << event_.particles().size() << " particles and "
                        << event_.vertices().size() << " vertices to output "
                        << std::endl;
  output_file_->write_event(event_);
}

}  // namespace smash
