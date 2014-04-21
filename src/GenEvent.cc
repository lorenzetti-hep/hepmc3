/**
 *  @file GenEvent.cc
 *  @brief Implementation of \b class HepMC3::GenEvent
 *
 */
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/Setup.h"

#include "HepMC3/Search/FindParticles.h"

#include <vector>
#include <stdexcept> // std::out_of_range
#include <cmath>     // std::abs

#include <boost/range/iterator_range.hpp>
#include <boost/foreach.hpp>
using std::endl;

namespace HepMC3 {

GenEvent::GenEvent():
m_print_precision(2) {

    m_data.event_number = 0;

    // Create default version
    new_version("Version 0");
}

void GenEvent::print_version( unsigned char version, std::ostream& ostr ) const {
    ostr << "________________________________________________________________________________" << endl;
    ostr << "GenEvent: #" << m_data.event_number << endl;
    ostr << " Version: \"" << m_data.versions[version-1].name
         << "\" (version id: " << version << ", last version id: " << m_data.versions.size() << ")" <<endl;
    ostr << " Entries in this event: " << m_vertices.size() << " vertices, "
         << m_particles.size() << " particles." << endl;

    // Print a legend to describe the particle info
    ostr << "                                    GenParticle Legend" << endl;
    ostr << "     Barcode   PDG ID   "
         << "( Px,       Py,       Pz,     E )"
         << "   Stat-Subst  ProdVtx" << endl;
    ostr << "________________________________________________________________________________" << endl;

    // Find the current stream state
    std::ios_base::fmtflags orig = ostr.flags();
    std::streamsize         prec = ostr.precision();

    // Set precision
    ostr.precision(m_print_precision);

    for(unsigned int i=0; i<m_vertices.size(); ++i) {
        const GenVertex &v = m_vertices[i];

        v.print(ostr,1,version);
    }

    // Restore the stream state
    ostr.flags(orig);
    ostr.precision(prec);
    ostr << "________________________________________________________________________________" << endl;
}

void GenEvent::dump() const {
    std::cout<<"-----------------------------"<<std::endl;
    std::cout<<"--------- EVENT DUMP --------"<<std::endl;
    std::cout<<"-----------------------------"<<std::endl;
    std::cout<<std::endl;

    std::cout<<"GenParticle:"<<std::endl;
    for(unsigned int i=0; i<m_particles.size(); ++i) {
        m_particles[i].print();
    }
    std::cout<<"GenParticleData:"<<std::endl;
    for(unsigned int i=0; i<m_data.particle_data.size(); ++i) {
        std::cout<<i<<": ";
        m_data.particle_data[i].print();
    }
    std::cout<<"GenVertex:"<<std::endl;
    for(unsigned int i=0; i<m_vertices.size(); ++i) {
        std::cout<<i<<": ";
        m_vertices[i].print();
    }
    std::cout<<"GenVertexData:"<<std::endl;
    for(unsigned int i=0; i<m_data.vertex_data.size(); ++i) {
        std::cout<<i<<": ";
        m_data.vertex_data[i].print();
    }
}

GenParticle& GenEvent::particle(int barcode) const {
    if ( barcode <= 0 || barcode > (int)m_particles.size() ) throw std::out_of_range( "GenEvent::get_particle" );

    return m_particles[ barcode - 1 ];
}

GenVertex& GenEvent::vertex(int barcode) const {
    barcode = -barcode;
    if ( barcode <= 0 || barcode >  (int)m_vertices.size() ) throw std::out_of_range( "GenEvent::get_vertex" );

    return m_vertices[ barcode - 1 ];
}

GenParticle& GenEvent::new_particle( const GenParticleData *data ) {

    GenParticle      *p = m_particles.new_uninitialized_object();
    GenParticleData *pd = m_data.particle_data.new_object();

    new (p) GenParticle( *this, m_data.particle_data.size() - 1, *pd );

    if(data) *pd = *data; // copy

    return *p;
}

GenParticle& GenEvent::new_particle( const FourVector& momentum, int pdg_id, int status ) {

    GenParticle      *p = m_particles.new_uninitialized_object();
    GenParticleData *pd = m_data.particle_data.new_object();

    new (p) GenParticle( *this, m_data.particle_data.size() - 1, *pd );

    pd->momentum = momentum;
    pd->pdg_id   = pdg_id;
    pd->status   = status;

    return *p;
}

GenVertex& GenEvent::new_vertex( const GenVertexData *data ) {

    GenVertex      *v = m_vertices.new_uninitialized_object();
    GenVertexData *vd = m_data.vertex_data.new_object();

    new (v) GenVertex( *this, m_data.vertex_data.size() - 1, *vd );

    if(data) *vd = *data; // copy

    return *v;
}

void GenEvent::delete_particle(GenParticle &p) {
    if( p.is_deleted() ) return;

    p.mark_deleted();

    GenVertex *v = p.end_vertex();

    // Deleting particle invalidates rest of its decay tree
    if(v) delete_vertex(*v);
}

void GenEvent::delete_vertex(GenVertex &v) {
    if( v.is_deleted() ) return;

    v.mark_deleted();

    // Deleting vertex invalidates end_vertices of incoming particles
    BOOST_FOREACH( GenParticle *p, v.m_particles_in ) {
        if( !p->is_deleted() ) p->set_end_vertex(NULL);
    }

    // Deleting vertex invalidates rest of the decay tree
    BOOST_FOREACH( GenParticle *p, v.m_particles_out ) {
        delete_particle(*p);
    }

}

void GenEvent::new_version( const std::string name ) {

    GenEventVersionInfo v;
    v.name                 = name;
    v.first_particle_index = particles_count();
    v.first_vertex_index   = vertices_count();

    m_data.versions.push_back(v);
}

bool GenEvent::record_change(GenParticle& p) {

    // Check if this particle already exists in the newest version
    if( p.m_last_version->version_created() == last_version() ) return true;

    if( p.m_last_version->is_deleted() ) {
        ERROR( "GenEvent::record_change: Cannot change deleted particle (barcode="<<p.barcode()<<")" )
        return false;
    }

    // Create new particle as a copy of previous one
    GenParticle      *new_p = m_particles.new_uninitialized_object();
    GenParticleData *new_pd = m_data.particle_data.new_object(p.m_last_version->m_data);

    new (new_p) GenParticle( *this, m_data.particle_data.size() - 1, *new_pd );

    // Mark this particle as deleted and update last version pointer
    if( !p.m_last_version->is_deleted() ) p.m_last_version->m_version_deleted = last_version();

    m_data.version_links.push_back( std::pair<int,int>(new_p->barcode(),p.m_last_version->barcode()) );

    p.m_last_version->m_last_version = new_p;
    p.m_last_version = new_p;

    // Add new particle to production/end vertices
    GenVertex *v = new_p->production_vertex();
    if(v) v->m_particles_out.push_back(new_p);

    v = new_p->end_vertex();
    if(v) v->m_particles_in.push_back(new_p);

    return true;
}

bool GenEvent::record_change(GenVertex& v) {

    // Check if this vertex already exists in the newest version
    if( v.m_last_version->version_created() == last_version() ) return true;

    if( v.is_deleted() ) {
        ERROR( "GenEvent::record_change: Cannot change deleted vertex (barcode="<<v.barcode()<<")" )
        return false;
    }

    // Create new particle as a copy of previous one
    GenVertex      *new_v = m_vertices.new_uninitialized_object();
    GenVertexData *new_vd = m_data.vertex_data.new_object(v.m_data);

    new (new_v) GenVertex( *this, m_data.vertex_data.size() - 1, *new_vd );

    // Mark this vertex as deleted and update last version pointer
    if( !v.is_deleted() ) v.m_version_deleted = last_version();
    v.m_last_version = new_v;

    m_data.version_links.push_back( std::pair<int,int>(new_v->barcode(),v.barcode()) );

    // Add all particles from previous vertex to new vertex

    BOOST_FOREACH( GenParticle *p, v.particles_in() ) {
        if( p->version_deleted() <= last_version() ) continue;
        new_v->m_particles_in.push_back(p);
    }

    BOOST_FOREACH( GenParticle *p, v.particles_out() ) {
        if( p->version_deleted() <= last_version() ) continue;
        new_v->m_particles_out.push_back(p);
    }

    return true;
}

} // namespace HepMC3
