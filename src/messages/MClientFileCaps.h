// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef __MCLIENTFILECAPS_H
#define __MCLIENTFILECAPS_H

#include "msg/Message.h"


class MClientFileCaps : public Message {
 private:
  struct ceph_mds_file_caps h;
 public:
  bufferlist snapbl;

  int      get_caps() { return h.caps; }
  int      get_wanted() { return h.wanted; }
  capseq_t get_seq() { return h.seq; }
  capseq_t get_mseq() { return h.migrate_seq; }

  inodeno_t get_ino() { return inodeno_t(h.ino); }
  __u64 get_size() { return h.size;  }
  __u64 get_max_size() { return h.max_size;  }
  utime_t get_ctime() { return utime_t(h.ctime); }
  utime_t get_mtime() { return utime_t(h.mtime); }
  utime_t get_atime() { return utime_t(h.atime); }
  __u64 get_time_warp_seq() { return h.time_warp_seq; }

  int       get_migrate_seq() { return h.migrate_seq; }
  int       get_op() { return h.op; }

  //inodeno_t get_snap_realm() { return inodeno_t(h.snap_realm); }
  //snapid_t get_snap_created() { return snapid_t(h.snap_created); }
  //snapid_t get_snap_highwater() { return snapid_t(h.snap_highwater); }
  snapid_t get_snap_follows() { return snapid_t(h.snap_follows); }
  void set_snap_follows(snapid_t s) { h.snap_follows = s; }

  void set_caps(int c) { h.caps = c; }
  void set_wanted(int w) { h.wanted = w; }

  void set_max_size(__u64 ms) { h.max_size = ms; }

  void set_migrate_seq(unsigned m) { h.migrate_seq = m; }
  void set_op(int o) { h.op = o; }

  void set_size(loff_t s) { h.size = s; }
  void set_mtime(const utime_t &t) { t.encode_timeval(&h.mtime); }
  void set_atime(const utime_t &t) { t.encode_timeval(&h.atime); }

  MClientFileCaps() {}
  MClientFileCaps(int op,
		  inode_t& inode,
		  inodeno_t realm,
                  long seq,
                  int caps,
                  int wanted,
		  int mseq) :
    Message(CEPH_MSG_CLIENT_FILECAPS) {
    h.op = op;
    h.seq = seq;
    h.caps = caps;
    h.wanted = wanted;
    h.ino = inode.ino;
    h.size = inode.size;
    h.max_size = inode.max_size;
    h.migrate_seq = mseq;
    inode.mtime.encode_timeval(&h.mtime);
    inode.atime.encode_timeval(&h.atime);
    inode.ctime.encode_timeval(&h.ctime);
    h.time_warp_seq = inode.time_warp_seq;
  }

  const char *get_type_name() { return "Cfcap";}
  void print(ostream& out) {
    out << "client_file_caps(" << ceph_cap_op_name(h.op)
	<< " ino " << inodeno_t(h.ino)
	<< " seq " << h.seq 
	<< " caps " << cap_string(h.caps)
	<< " wanted" << cap_string(h.wanted)
	<< " size " << h.size << "/" << h.max_size
	<< " mtime " << utime_t(h.mtime)
	<< " tws " << h.time_warp_seq
	<< " follows " << snapid_t(h.snap_follows);
    if (h.migrate_seq)
      out << " mseq " << h.migrate_seq;
    out << ")";
  }
  
  void decode_payload() {
    bufferlist::iterator p = payload.begin();
    ::decode(h, p);
    ::decode(snapbl, p);
  }
  void encode_payload() {
    ::encode(h, payload);
    ::encode(snapbl, payload);
  }
};

#endif
