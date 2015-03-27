// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#include "RootTool.h"

RootTool::RootTool(const std::string &filename, std::ios::openmode mode):m_file_in(NULL),m_file_out(NULL),m_timer("ROOT event writing time") {

    m_filename = filename;

    if(mode == std::ios::in) {
        HEPMC3CODE( m_file_in = new ReaderRoot(m_filename); )

        m_timer = Timer("ROOT event parsing time");
    }
    else {
        m_file_out = new WriterRoot(m_filename);
    }
}

RootTool::~RootTool() {
    if(m_file_in)  delete m_file_in;
    if(m_file_out) delete m_file_out;
}

void RootTool::initialize() {}

int RootTool::process(GenEvent &hepmc) {
    if(m_file_in) {
        HEPMC3CODE( m_file_in->read_event( hepmc); )
        if( m_file_in->rdstate() ) return -1;
    }
    else if(m_file_out) {
        HEPMC3CODE( m_file_out->write_event( hepmc); )
        if( m_file_out->rdstate() ) return -1;
    }

    return 0;
}

void RootTool::finalize() {
    HEPMC3CODE(
        if(m_file_in)  m_file_in->close();
        if(m_file_out) m_file_out->close();
    )
}

bool RootTool::rdstate() {
    HEPMC3CODE(
        if(m_file_in)  return m_file_in->rdstate();
        if(m_file_out) return m_file_out->rdstate();
    )
    return true;
}