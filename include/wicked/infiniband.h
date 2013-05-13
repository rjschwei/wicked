/*
 * infiniband definitions for netinfo
 *
 * Copyright (C) 2013 Marius Tomaschewski <mt@suse.de>
 */

#ifndef __WICKED_INFINIBAND_H__
#define __WICKED_INFINIBAND_H__

#include <wicked/types.h>

enum {
	NI_INFINIBAND_MODE_DATAGRAM  = 0,
	NI_INFINIBAND_MODE_CONNECTED = 1,
};

enum {
	NI_INFINIBAND_UMCAST_DISALLOWED = 0,
	NI_INFINIBAND_UMCAST_ALLOWED  = 1,
};

struct ni_infiniband {
	unsigned int	mode;
	unsigned int	umcast;
	uint16_t 	pkey;
	ni_netdev_ref_t	parent;
};

extern ni_infiniband_t *ni_infiniband_new(void);
extern void		ni_infiniband_free(ni_infiniband_t *);

extern const char *	ni_infiniband_get_mode_name(unsigned int);
extern int		ni_infiniband_get_mode_flag(const char *);

extern const char *	ni_infiniband_get_umcast_name(unsigned int);
extern int		ni_infiniband_get_umcast_flag(const char *);

#endif /* __WICKED_INFINIBAND_H__ */
