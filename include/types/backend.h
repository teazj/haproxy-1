/*
 * include/types/backend.h
 * This file assembles definitions for backends
 *
 * Copyright (C) 2000-2009 Willy Tarreau - w@1wt.eu
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * exclusively.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _TYPES_BACKEND_H
#define _TYPES_BACKEND_H

#include <common/config.h>
#include <types/lb_fwlc.h>
#include <types/lb_fwrr.h>
#include <types/lb_map.h>
#include <types/server.h>

/* Parameters for lbprm.algo.
 * The low part of the value is unique for each algo so that applying the mask
 * BE_LB_ALGO returns a unique algorithm.
 * The high part indicates specific properties.
 */

/* Masks to extract algorithm properties */
#define BE_LB_ALGO	0x000007FF      /* mask to extract all algorithm flags */
#define BE_LB_PROP_DYN  0x00000100      /* mask to match dynamic algorithms */
#define BE_LB_PROP_L4   0x00000200      /* mask to match layer4-based algorithms */
#define BE_LB_PROP_L7   0x00000400      /* mask to match layer7-based algorithms */

/* the algorithms themselves */
#define BE_LB_ALGO_NONE 0x00000000              /* dispatch or transparent mode */
#define BE_LB_ALGO_RR	(BE_LB_PROP_DYN | 0x01) /* fast weighted round-robin mode (dynamic) */
#define BE_LB_ALGO_SH	(BE_LB_PROP_L4  | 0x02) /* balance on source IP hash */
#define BE_LB_ALGO_UH	(BE_LB_PROP_L7  | 0x03) /* balance on URI hash */
#define BE_LB_ALGO_PH	(BE_LB_PROP_L7  | 0x04) /* balance on URL parameter hash */
#define BE_LB_ALGO_LC	(BE_LB_PROP_DYN | 0x05) /* fast weighted leastconn mode (dynamic) */
#define BE_LB_ALGO_HH	(BE_LB_PROP_L7  | 0x06) /* balance on Http Header value */
#define BE_LB_ALGO_RCH	(BE_LB_PROP_L4  | 0x07) /* balance on RDP Cookie value */

/* various constants */

/* The scale factor between user weight and effective weight allows smooth
 * weight modulation even with small weights (eg: 1). It should not be too high
 * though because it limits the number of servers in FWRR mode in order to
 * prevent any integer overflow. The max number of servers per backend is
 * limited to about 2^32/255^2/scale ~= 66051/scale. A scale of 16 looks like
 * a good value, as it allows more than 4000 servers per backend while leaving
 * modulation steps of about 6% for servers with the lowest weight (1).
 */
#define BE_WEIGHT_SCALE 16

/* LB parameters for all algorithms */
struct lbprm {
	int algo;			/* load balancing algorithm and variants: BE_LB_ALGO_* */
	int tot_wact, tot_wbck;		/* total effective weights of active and backup servers */
	int tot_weight;			/* total effective weight of servers participating to LB */
	int tot_used;			/* total number of servers used for LB */
	int wmult;			/* ratio between user weight and effective weight */
	int wdiv;			/* ratio between effective weight and user weight */
	struct server *fbck;		/* first backup server when !PR_O_USE_ALL_BK, or NULL */
	struct lb_map map;		/* LB parameters for map-based algorithms */
	struct lb_fwrr fwrr;
	struct lb_fwlc fwlc;
	/* Call backs for some actions. Some may be NULL (thus should be ignored). */
	void (*update_server_eweight)(struct server *);  /* to be called after eweight change */
	void (*set_server_status_up)(struct server *);   /* to be called after status changes to UP */
	void (*set_server_status_down)(struct server *); /* to be called after status changes to DOWN */
	void (*server_take_conn)(struct server *);       /* to be called when connection is assigned */
	void (*server_drop_conn)(struct server *);       /* to be called when connection is dropped */
};

#endif /* _TYPES_BACKEND_H */

/*
 * Local variables:
 *  c-indent-level: 8
 *  c-basic-offset: 8
 * End:
 */
