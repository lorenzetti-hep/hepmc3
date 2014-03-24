#include <iostream>
#include <fstream>
#include "HepMC3/IO_GenEvent.h"
#include "HepMC3/GenEvent.h"
using std::cout;
using std::endl;

namespace HepMC3 {

void IO_GenEvent::write_event(const GenEvent *evt) {
    if ( !evt || m_io_error_state ) return;
	if ( m_mode != std::ios::out ) {
	    cout << "err3" << endl; // :TODO: error handling
	    return;
	}

    m_file << "E " << evt->event_number()
           << " "  << evt->vertices().size()
           << " "  << evt->particles().size()
           << endl;

    // Print all particles and vertices in the event
    // NOTE: assumption is made that barcodes of vertices and particles
    //       are in topological order!
    int highest_vertex_already_printed = 0;

    for( unsigned int i=0; i<evt->particles().size(); ++i) {

        int production_vertex = evt->particles()[i]->production_vertex_barcode();
        if( production_vertex < highest_vertex_already_printed ) {

            highest_vertex_already_printed = production_vertex;
            for( unsigned int j=0; j<evt->vertices().size(); ++j ) {
                if( evt->vertices()[j]->barcode() == production_vertex ) {
                    write_vertex(evt->vertices()[j]);
                    break;
                }
            }
        }
        write_particle(evt->particles()[i]);
    }
}

bool IO_GenEvent::fill_next_event(GenEvent *evt) {
    if ( !evt || m_io_error_state ) return 0;
	if ( m_mode != std::ios::in ) {
	    cout << "err4" << endl; // :TODO: error handling
	    return 0;
	}
    cout << "Read OK." << endl;

    return 1;
}

void IO_GenEvent::write_vertex(const GenVertex *v) {
    unsigned int in_size = v->particles_in().size();

    m_file << "V " << v->barcode()
           << " [";

    if(in_size) {
        for( unsigned int i=0; i<in_size-1; ++i ) {
            m_file << v->particles_in()[i]->barcode() << ",";
        }
        m_file << v->particles_in().back()->barcode();
    }
    m_file << "] ";

    m_file << "@ 0 0 0 0";
    m_file << endl;
}

void IO_GenEvent::write_particle(const GenParticle *p) {
    std::ios_base::fmtflags orig = m_file.flags();
    std::streamsize prec = m_file.precision();
    m_file.setf(std::ios::scientific, std::ios::floatfield);
    m_file.precision(4); // :TODO: set precision option

    m_file << "P "<< p->barcode()
           << " " << p->production_vertex_barcode()
           << " " << p->pdg_id()
           << " " << p->momentum().px()
           << " " << p->momentum().py()
           << " " << p->momentum().pz()
           << " " << p->momentum().e()
           << " " << p->generated_mass()
           << " " << p->status()
           << endl;

    m_file.flags(orig);
    m_file.precision(prec);
}

} // namespace HepMC3
